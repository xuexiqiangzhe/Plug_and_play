package routers

import (
	"appservice/controller"
	"net/http"

	"github.com/gin-gonic/gin"
)

func SetupRouter() *gin.Engine {
	r := gin.Default()
	//加载静态文件
	r.LoadHTMLFiles("./templates/second.html")
	// 告诉gin框架模板文件引用的静态文件去哪里找
	r.Static("/static", "./static")
	//注册可视化界面路由
	r.GET("/second.html", func(c *gin.Context) {
		c.HTML(http.StatusOK, "second.html", nil)
	})
	// 告诉gin框架去哪里找模板文件
	r.StaticFile("/favicon.ico", "./static/favicon.ico")

	// v1
	v1Group := r.Group("/api/v1")
	{
		v1Group.GET("/device", controller.Device) //返回需要可视化的设备
		v1Group.GET("/PDO", controller.Get_PDO)   //收集PDO
	}
	return r
}
