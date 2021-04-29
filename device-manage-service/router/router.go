package router

import (

	//ginSwagger "github.com/swaggo/gin-swagger"
	//"github.com/swaggo/gin-swagger/swaggerFiles"

	"device-manage-device/controller"
	//"github.com/gin-contrib/pprof"
	"github.com/gin-gonic/gin"
)

// SetupRouter 路由
func SetupRouter() *gin.Engine {
	router := gin.Default()

	//增加路由组
	v1 := router.Group("/api/v1")
	//这些接口待开发，没用
	v1.GET("/randomnumber", controller.Getrandomnumber)
	v1.GET("/device", controller.Device)
	v1.GET("/data", controller.SendProcessdata)

	//接口解析IODD获取PDO长度
	v1.GET("/IODD_PDO_LEN", controller.IODD_PDO_LEN)

	//接收设备注册请求，开启自动配置
	var groups = controller.Group{}
	v1.POST("/upload", groups.StartDeviceSevice)

	return router
}
