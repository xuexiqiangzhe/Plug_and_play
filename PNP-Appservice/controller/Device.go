package controller

import (
	"net/http"

	"github.com/gin-gonic/gin"
)

var Global_deviceNames []string

//返回注册了的设备名词数组
func Device(c *gin.Context) {
	c.Writer.Header().Add("Access-Control-Allow-Origin", "*") //允许访问规则服务器的域名
	/*返回*/
	c.JSON(http.StatusOK, &ResponseData{
		Data: Global_deviceNames,
	})
}
