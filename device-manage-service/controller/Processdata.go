package controller

import (
	"github.com/gin-gonic/gin"
	"net/http"
)

type objdata []struct {
	id       int
	deviceid string
	process  []int
}

var globaldata objdata

func updateProcessdata(data string) {
	//	globaldata
}
func SendProcessdata(c *gin.Context) {

	c.Writer.Header().Add("Access-Control-Allow-Origin", "*") //允许访问规则服务器的域名
	c.JSON(http.StatusOK, &ResponseData{
		Data: 0,
	})
	return
}
