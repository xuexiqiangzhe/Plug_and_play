在Goland IDE下
go build
然后就可启动
./device-manage-device




sudo chmod -R 777 ./device-simple/
流程：
POST（注册信息）的表单的格式如下，-d就行了。
curl -d "name=jb&age=18" http://localhost:8080/api/v1/upload
然后解析模板配置文件，生成配置文件。
调用shell启动设备微服务。
当本工程关闭的时候，调用shell启动的设备微服务才会被关闭。

查看一级子目录
tree -L 1
.
├── controller  	//controller层
├── device-sdk-go  	//edgex的go sdk包，未来可能用
├── dir_size.sh 	//模拟脚本
├── device-manage-device//可执行文件
├── go.mod
├── go.sum
├── main.go  		//主程序入口
├── read.me
├── res   		//toml yaml配置文件导入与生成
├── router		//路由配置
├── service		//业务逻辑
├── static		//静态界面css js jpg
└── templates		//静态界面html


