package controller

import (
	"device-manage-device/service"
	"fmt"
	"log"
	"net/http"
	"os/exec"
	"strconv"
	"sync"

	"github.com/gin-gonic/gin"
	_ "github.com/go-sql-driver/mysql"
)

type Group struct {
	mu sync.Mutex // protects m
	wg sync.WaitGroup
}

func (g *Group) StartDeviceSevice(c *gin.Context) {
	//获取POST的表单数据
	a, _ := c.GetPostForm("deviceid")
	b, _ := c.GetPostForm("begin_node")
	d, _ := c.GetPostForm("end_node")
	deviceid, _ := strconv.Atoi(a)
	begin_node, _ := strconv.Atoi(b)
	end_node, _ := strconv.Atoi(d)
	//fmt.Println("zhuceqingqiu", deviceid, begin_node, end_node)

	/*解析配置--xml2struct
	var eth_str service.EtherCATInfo
	xml_message = "<?xml version=\"1.0\" ?>\n<EtherCATInfo>\n  <!-- Slave 0 -->\n  <Vendor>\n    <Id>2013</Id>\n  </Vendor>\n  <Descriptions>\n    <Devices>\n      <Device>\n        <Type ProductCode=\"#x00000001\" RevisionNo=\"#x00000001\">EP3E-EC</Type>\n        <Name><![CDATA[EP3E-EC]]></Name>\n        <Sm Enable=\"1\" StartAddress=\"#x1000\" ControlByte=\"#x26\" DefaultSize=\"128\" />\n        <Sm Enable=\"1\" StartAddress=\"#x1400\" ControlByte=\"#x22\" DefaultSize=\"128\" />\n        <Sm Enable=\"0\" StartAddress=\"#x1800\" ControlByte=\"#x24\" DefaultSize=\"6\" />\n        <Sm Enable=\"0\" StartAddress=\"#x1c00\" ControlByte=\"#x20\" DefaultSize=\"6\" />\n        <RxPdo Sm=\"2\" Fixed=\"1\" Mandatory=\"1\">\n          <Index>#x1601</Index>\n          <Name>2nd receive PDO-Mapping</Name>\n          <Entry>\n            <Index>#x6040</Index>\n            <SubIndex>0</SubIndex>\n            <BitLen>16</BitLen>\n            <Name></Name>\n            <DataType>UINT16</DataType>\n          </Entry>\n          <Entry>\n            <Index>#x607a</Index>\n            <SubIndex>0</SubIndex>\n            <BitLen>32</BitLen>\n            <Name></Name>\n            <DataType>UINT32</DataType>\n          </Entry>\n          <Entry>\n            <Index>#x0000</Index>\n            <BitLen>0</BitLen>\n          </Entry>\n          <Entry>\n            <Index>#x0000</Index>\n            <BitLen>0</BitLen>\n          </Entry>\n          <Entry>\n            <Index>#x0000</Index>\n            <BitLen>0</BitLen>\n          </Entry>\n          <Entry>\n            <Index>#x0000</Index>\n            <BitLen>0</BitLen>\n          </Entry>\n          <Entry>\n            <Index>#x0000</Index>\n            <BitLen>0</BitLen>\n          </Entry>\n          <Entry>\n            <Index>#x0000</Index>\n            <BitLen>0</BitLen>\n          </Entry>\n          <Entry>\n            <Index>#x0000</Index>\n            <BitLen>0</BitLen>\n          </Entry>\n          <Entry>\n            <Index>#x0000</Index>\n            <BitLen>0</BitLen>\n          </Entry>\n        </RxPdo>\n        <TxPdo Sm=\"3\" Fixed=\"1\" Mandatory=\"1\">\n          <Index>#x1a01</Index>\n          <Name>2nd Transmit PDO Mapping</Name>\n          <Entry>\n            <Index>#x6041</Index>\n            <SubIndex>0</SubIndex>\n            <BitLen>16</BitLen>\n            <Name></Name>\n            <DataType>UINT16</DataType>\n          </Entry>\n          <Entry>\n            <Index>#x6064</Index>\n            <SubIndex>0</SubIndex>\n            <BitLen>32</BitLen>\n            <Name></Name>\n            <DataType>UINT32</DataType>\n          </Entry>\n          <Entry>\n            <Index>#x0000</Index>\n            <BitLen>0</BitLen>\n          </Entry>\n          <Entry>\n            <Index>#x0000</Index>\n            <BitLen>0</BitLen>\n          </Entry>\n          <Entry>\n            <Index>#x0000</Index>\n            <BitLen>0</BitLen>\n          </Entry>\n          <Entry>\n            <Index>#x0000</Index>\n            <BitLen>0</BitLen>\n          </Entry>\n          <Entry>\n            <Index>#x0000</Index>\n            <BitLen>0</BitLen>\n          </Entry>\n          <Entry>\n            <Index>#x0000</Index>\n            <BitLen>0</BitLen>\n          </Entry>\n          <Entry>\n            <Index>#x0000</Index>\n            <BitLen>0</BitLen>\n          </Entry>\n          <Entry>\n            <Index>#x0000</Index>\n            <BitLen>0</BitLen>\n          </Entry>\n        </TxPdo>\n      </Device>\n    </Devices>\n  </Descriptions>\n</EtherCATInfo>\n\n"
	if err := xml.Unmarshal([]byte(xml_message), &eth_str); err != nil {
		fmt.Println(err)
		return
	} else {
		fmt.Println("转换成功", eth_str)
	}*/

	/*启动配置--toml-configure*/
	fmt.Println("解析toml")
	toml_struct := service.Decode_toml()
	toml_struct.DeviceList[0].Name = fmt.Sprintf("IO_Link_Device%d", deviceid)    //配置edgex设备名称
	toml_struct.DeviceList[0].AutoEvents[0].Frequency = "1s"                      //配置数据采集周期
	toml_struct.DeviceList[0].Profile = fmt.Sprintf("IO_Link_Device%d", deviceid) //配置profile
	var edgex_port int

	//防止并发注册设备请求，如请求1和请求2并发，请求1拿锁，请求2等待请求1结束再拿锁
	g.wg.Wait()
	g.wg.Add(1)
	//得到mysql表中最大的port+1
	edgex_port = service.Find_maxport(deviceid) + 1

	if edgex_port == 0 {
		return
	}
	//配置设备微服务port
	toml_struct.Service.Port = edgex_port
	fmt.Println(toml_struct)
	fmt.Println("解析toml end")
	if err := service.Endcode_toml(toml_struct); err != nil { //好处：err的生命周期在此if
		log.Fatal(err)
	} else {
		println("生成toml文件成功")
	}

	/*启动配置--yaml-configure*/
	/*fmt.Println("解析yaml")
	var y *service.YamlEdgex_Struct
	//读取yaml配置文件
	yaml_conf := y.Yaml_Decode(deviceid)
	fmt.Println("解析yaml end")
	yaml_conf.Labels[0] = "YXHHHH"
	if err := yaml_conf.Yaml_Encode(); err != nil {
		log.Fatal(err)
	} else {
		println("生成yaml文件成功")
	}*/
	//选择profile，拷贝到设备微服务的res目录下
	var profile_path = fmt.Sprintf("./res/profile_IO_Link_Device%d.yaml ", deviceid)
	var target_profile_path = fmt.Sprintf("./device-simple/res/")
	command := "cp " + profile_path + target_profile_path
	exec.Command("/bin/bash", "-c", command)

	//启动设备微服务，传参三个，PDO开始节点，PDO结束节点，IO-Link设备名称
	var svcname = fmt.Sprintf("OPCUA-IO-Link%d", deviceid)
	command = fmt.Sprintf("./device-simple/dir_size.sh %d %d %s", begin_node, end_node, svcname)
	cmd := exec.Command("/bin/bash", "-c", command)
	g.wg.Done()
	output, err := cmd.Output()
	if err != nil {
		fmt.Printf("Execute Shell:%s failed with error:%s\n", command, err.Error())
		return
	} else {
		fmt.Printf("Execute Shell:%s finished with output:\n%s", command, string(output))
	}

	//返回响应
	c.JSON(http.StatusOK, nil)

}
