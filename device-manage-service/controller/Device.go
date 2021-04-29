package controller

import (
	"database/sql"
	"fmt"
	"net/http"
	"strconv"

	"github.com/beevik/etree"
	"github.com/gin-gonic/gin"
)

type wait_reg []struct {
	PDO_LEN string
	//frontid string
}

func Device(c *gin.Context) {
	c.Writer.Header().Add("Access-Control-Allow-Origin", "*") //允许访问规则服务器的域名
	//查询没被js显示的
	db, err := sql.Open("mysql", "root:root@tcp(127.0.0.1:3306)/Plug_and_play")
	//sql语句
	sqlStr := "select device_id from IO_Link_Device where js_show_flag = 0"
	//调用方法
	rows, err := db.Query(sqlStr)
	//错误处理
	if err != nil {
		fmt.Println(err)
	}
	//关闭rows释放持有的数据库链接
	defer rows.Close()
	//使用for循环进行结果的读取
	var still_unshow_devices []string
	var i = 0
	for rows.Next() {
		var device_id string
		err := rows.Scan(device_id)
		if err != nil {
			fmt.Println(err)
		} else {
			still_unshow_devices = append(still_unshow_devices, device_id)
			//更新数据库标志位和js_show_flag
			fmt.Printf("device_un_show:%d\n", device_id)
			sqlStr = fmt.Sprintf("update IO_Link_Device set js_show_flag=1 where device_id = %d;", device_id)
			_, err = db.Exec(sqlStr)
			if err != nil {
				fmt.Printf("update failed, err:%v\n", err)
			}
		}
		i++
	}

	// 返回响应
	c.JSON(http.StatusOK, &still_unshow_devices)
	return

}

func IODD_PDO_LEN(c *gin.Context) {
	//允许访问规则服务器的域名
	c.Writer.Header().Add("Access-Control-Allow-Origin", "*")
	deviceids := c.Query("deviceid")
	deviceid, _ := strconv.Atoi(deviceids)
	//获取IODD
	filepath := fmt.Sprintf("../IODD/ifm-000%x.xml", deviceid)
	doc := etree.NewDocument()
	if err := doc.ReadFromFile(filepath); err != nil {
		panic(err)
	}
	//解析得到IODD的总bitLength，即PDO总长度，单位是bit
	root := doc.SelectElement("IODevice").SelectElement("ProfileBody").SelectElement("DeviceFunction").SelectElement("ProcessDataCollection").SelectElement("ProcessData").SelectElement("ProcessDataIn")
	atrr := root.SelectAttr("bitLength")
	fmt.Println("PDO_LEN:", atrr.Value)
	//返回响应
	c.JSON(http.StatusOK, atrr.Value)
	return

}
