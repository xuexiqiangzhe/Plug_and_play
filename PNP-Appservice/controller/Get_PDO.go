package controller

import (
	"container/list"
	"net/http"
	"sync"

	"github.com/gin-gonic/gin"
)

var Mu sync.Mutex
var Device_number = 0

type L struct {
	Cursize int
	LL      *list.List
}

//双向链表实例化
var Datastring = &L{
	Cursize: 0,
	LL:      list.New(),
}

//返回响应的结构体
type ResponseData struct {
	Data interface{} `json:"data,pdo"`
}

func Get_PDO(c *gin.Context) {
	c.Writer.Header().Add("Access-Control-Allow-Origin", "*") //允许访问规则服务器的域名
	var PDO []string
	Mu.Lock()
	//设备数量等于双向链表当前大小，且大小不为0，则继续
	if Device_number != 0 && Device_number == Datastring.Cursize {
		//遍历链表装入结构体对象
		for e := Datastring.LL.Front(); e != nil; e = e.Next() {
			//fmt.Println(Datastring.Cursize,Device_number, e.Value.(string))
			PDO = append(PDO, e.Value.(string))
		}
		Mu.Unlock()
		//返回响应和PDO数据
		c.JSON(http.StatusOK, &ResponseData{
			Data: PDO,
		})
	} else {
		Mu.Unlock()
		c.JSON(http.StatusLocked, nil)
	}
}
