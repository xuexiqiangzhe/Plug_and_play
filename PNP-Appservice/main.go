//
// Copyright (c) 2019 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

package main

import (
	"appservice/controller"
	"database/sql"
	"encoding/json"
	"fmt"
	"os"

	"github.com/edgexfoundry/app-functions-sdk-go/pkg/transforms"
	_ "github.com/go-sql-driver/mysql"

	"github.com/edgexfoundry/app-functions-sdk-go/appcontext"
	"github.com/edgexfoundry/app-functions-sdk-go/appsdk"
	"github.com/edgexfoundry/go-mod-core-contracts/models"

	"appservice/routers"
	"errors"
)

const (
	serviceKey = "sampleFilterXmlMqtt"
)

var data []byte

var output_err error
var rows *sql.Rows
var db *sql.DB

func main() {

	//edgex的APPservice原生初始化
	// turn off secure mode for examples. Not recommended for production
	os.Setenv("EDGEX_SECURITY_SECRET_STORE", "false")
	// 1) First thing to do is to create an instance of the EdgeX SDK and initialize it.
	edgexSdk := &appsdk.AppFunctionsSDK{ServiceKey: serviceKey}
	if err := edgexSdk.Initialize(); err != nil {
		edgexSdk.LoggingClient.Error(fmt.Sprintf("SDK initialization failed: %v\n", err))
		os.Exit(-1)
	}

	// 2) shows how to access the application's specific configuration settings.
	/*deviceName, err := edgexSdk.GetAppSettingStrings("DeviceNames")
	//deviceNames=append(deviceNames, deviceName...)

	if err != nil {
		edgexSdk.LoggingClient.Error(err.Error())
		os.Exit(-1)
	}*/

	//初始监控controller.Global_deviceNames这些edgex设备
	edgexSdk.LoggingClient.Info(fmt.Sprintf("Filtering for devices %v", controller.Global_deviceNames))

	// Since we are using MQTT, we'll also need to set up the addressable model to
	// configure it to send to our broker. If you don't have a broker setup you can pull one from docker i.e:
	// docker run -it -p 1883:1883 -p 9001:9001  eclipse-mosquitto
	//MQTT接口配置，留着，不过即插即用demo不需要
	addressable := models.Addressable{
		Address:   "127.0.0.1",
		Port:      1883,
		Protocol:  "tcp",
		Publisher: "MyApp",
		User:      "",
		Password:  "",
		Topic:     "sampleTopicYXH",
	}
	mqttConfig := transforms.MqttConfig{}
	pair := transforms.KeyCertPair{
		KeyFile:  "PATH_TO_YOUR_KEY_FILE",
		CertFile: "PATH_TO_YOUR_CERT_FILE",
	}
	mqttSender := transforms.NewMQTTSender(edgexSdk.LoggingClient, addressable, &pair, mqttConfig, false)

	// 3) 这是我们的管道配置，函数的集合
	//可以传N个函数进去,每次触发监控事件，都挨个执行一次
	//设置了sdk.transforms。sdk.runtime这两个参数为接口组
	edgexSdk.SetFunctionsPipeline(
		transforms.NewFilter(controller.Global_deviceNames).FilterByDeviceName,
		outparams,
		mqttSender.MQTTSend,
	)
	fmt.Println("——————————Web前端线程——————————")
	go web_front()

	//数据库接口打开
	db, output_err = sql.Open("mysql", "root:1@tcp(127.0.0.1:3306)/Plug_and_play")
	if output_err != nil {
		fmt.Println(output_err)
		return
	}
	defer rows.Close()

	fmt.Println("——————————Edgex SDK“开始”并开始监听事件——————————")
	// 4) 最后，我们将继续告诉SDK“开始”并开始监听事件
	//触发管道。
	err := edgexSdk.MakeItRun()
	if err != nil {
		edgexSdk.LoggingClient.Error("MakeItRun returned error: ", err.Error())
		os.Exit(-1)
	}
	fmt.Println("————————关闭了两个线程————————")
	os.Exit(0)
}

func outparams(edgexcontext *appcontext.Context, params ...interface{}) (bool, interface{}) {
	if len(params) < 1 {
		// We didn't receive a result
		return false, nil
	}
	event, ok := params[0].(models.Event)
	//更新appservice
	if !ok {
		return false, errors.New("type received is not an Event")
	}
	data, output_err = json.Marshal(event)
	//fmt.Println(string(data))
	//查询未被APPservice监控的edgex设备名称
	sqlStr := "select device_id from IO_Link_Device where  appservice_monitor_flag = 0;"
	rows, output_err = db.Query(sqlStr)
	if output_err != nil {
		fmt.Println(output_err)
	}
	var deviceid string
	//每扫到一个未被监控的device，动态数组增加一个devicename
	for rows.Next() {
		output_err = rows.Scan(&deviceid)
		if output_err != nil {
			fmt.Println(output_err, "fault\n")
			break
		}
		controller.Device_number++
		devicename := fmt.Sprintf("IO_Link_Device%s", deviceid)
		controller.Global_deviceNames = append(controller.Global_deviceNames, devicename)
		//写数据库设备注册表，把appservice监控标志位ie置1
		sqlStr = fmt.Sprintf("update IO_Link_Device set appservice_monitor_flag=1 where device_id = %s;", deviceid)
		_, output_err = db.Exec(sqlStr)
		if output_err != nil {
			fmt.Printf("update failed, err:%v\n", output_err)
		}
	}

	//controller.Datastring这个双向链表里添加过程数据
	controller.Mu.Lock()
	if controller.Device_number != 0 && controller.Datastring.Cursize == controller.Device_number { //满了

		ele := controller.Datastring.LL.Back()
		if ele != nil {
			//fmt.Println("rm的结构体", ele.Value.(string))
			controller.Datastring.LL.Remove(ele)
			controller.Datastring.Cursize--
		}
	}
	//fmt.Println(controller.Global_deviceNames,"Front\n",string(data))
	controller.Datastring.LL.PushFront(string(data))
	controller.Datastring.Cursize++
	controller.Mu.Unlock()

	// Leverage the built in logging service in EdgeX
	return true, event
}
func web_front() {
	/*if len(os.Args) < 2 {
		fmt.Println("Usage：./appservice conf/config.ini")
		return
	}
	// 加载配置文件
	if err := setting.Init(os.Args[1]); err != nil {
		fmt.Printf("load config from file failed, err:%v\n", err)
		return
	}*/

	//fmt.Println("端口：", setting.Conf.Port)
	// 注册路由
	r := routers.SetupRouter()
	//启动APPservice的Webserver，端口三8000
	if err := r.Run(fmt.Sprintf(":%d", 8000)); err != nil {
		fmt.Printf("appservice front web startup failed, err:%v\n", err)
	}
}
