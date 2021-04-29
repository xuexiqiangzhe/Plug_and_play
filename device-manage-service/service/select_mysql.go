package service

import (
	"database/sql"
	"fmt"

	_ "github.com/go-sql-driver/mysql"
)

//查询mysql中记录的edgex设备注册的最大端口数值
func Find_maxport(deviceid int) int {

	//查询最高port
	db, err := sql.Open("mysql", "root:1@tcp(localhost:3306)/Plug_and_play")
	/*//设置数据库最大连接数
	  db.SetConnMaxLifetime(100)
	  //设置上数据库最大闲置连接数
	  db.SetMaxIdleConns(10)*/
	//验证连接
	if err != nil {
		fmt.Println("open database fail")
		return -1
	}
	fmt.Println("connnect DB success")
	//sql语句
	sqlStr := "select max(edgex_port) from IO_Link_Device"
	//调用方法
	rows, err := db.Query(sqlStr)
	//错误处理
	if err != nil {
		fmt.Println(err)
	}
	//关闭rows释放持有的数据库链接
	defer rows.Close()
	//使用for循环进行结果的读取
	var edgex_port = 1
	for rows.Next() {
		err := rows.Scan(&edgex_port)
		fmt.Printf("maxport:%d\n", edgex_port)
		if err != nil {
			fmt.Println(err)
		}
		if edgex_port == 0 {
			edgex_port = 49999
		}

	}
	//更新数据库标志位和edgex_port
	sqlStr = fmt.Sprintf("update IO_Link_Device set edgex_port=%d, deviceservice_create_flag=1 where device_id = %d",
		edgex_port+1, deviceid)
	_, err = db.Exec(sqlStr)
	if err != nil {
		fmt.Printf("update failed, err:%v\n", err)
	}
	return edgex_port
}
