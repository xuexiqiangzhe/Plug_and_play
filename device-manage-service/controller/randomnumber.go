package controller

import (
	"github.com/gin-gonic/gin"
"math/rand"
	"net/http"
)

type ResponseData struct {
	Data interface{} `json:"data,omitempty"`
}

func Getrandomnumber(c *gin.Context) {
	randomnuber := rand.Intn(100)
	c.Writer.Header().Add("Access-Control-Allow-Origin", "*")//允许访问规则服务器的域名
	c.JSON(http.StatusOK, &ResponseData{
		Data: randomnuber,
	})
	return
	// 返回响应
}