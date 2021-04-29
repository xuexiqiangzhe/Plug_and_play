运行本工程
在硬件接线正确以后，
PC（EtherCAT主站）+EK1100（EtherCAT从站）+EL6224（IO-Link主站）+EI5084和RVP510（IO-Link传感器）
解释：EtherCAT主站通过程序扫描EL6224的PDO，PC通过命令行获取SDO（当且仅当插上网线则获取成功）。

1.启动mysql数据库 3306端口，一般开机自启动了。
2.启动edgex核心层，~/go/src/github/edgexfoundry/edgex-go目录下
make run
3.运行Igh主站：
sudo /etc/init.d/ethercat start
4.进入对应目录运行
（1）device-manage-device
（2）appservice

打开可视界面
http://127.0.0.1:8000/second.html

5.进入对应目录运行总线程序：
./EL6224
