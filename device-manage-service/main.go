package main

import (
	"fmt"
	//"github.com/gin-gonic/gin"
	//"net/http"
	"device-manage-device/router"
)

func main() {

	//设置路由
	r := router.SetupRouter()
	//设备管理为服务运行在8080端口
	err := r.Run(fmt.Sprintf(":%d", 8080))
	if err != nil {
		fmt.Printf("run server failed, err:%v\n", err)
		return
	}

}
