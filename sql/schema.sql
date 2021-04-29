-- 创建数据库
CREATE DATABASE Plug_and_play;
-- 使用数据库
use Plug_and_play;
CREATE TABLE IO_Link_Device(
  `device_id` BIGINT NOT NUll AUTO_INCREMENT COMMENT 'IO-Link设备id',
  `deviceservice_create_flag` int NOT NULL COMMENT '设备微服务开启标志',
  `appservice_monitor_flag` int NOT NULL COMMENT 'APP微服务监控设备标志',
  `js_show_flag` int NOT NULL COMMENT 'JS可视化标志',
  `io_link_port` int NOT NULL COMMENT 'IO-Link主站Port',
  `edgex_port` int NOT NULL COMMENT '设备微服务Port',
  `create_time` TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
  PRIMARY KEY (device_id)
)ENGINE=INNODB AUTO_INCREMENT=1000 DEFAULT CHARSET=utf8 COMMENT='IO-Link设备表';

/*-- 初始化数据
INSERT into IO_Link_Device(device_id,deviceservice_create_flag,appservice_monitor_flag,js_show_flag,io_link_port,edgex_port)
VALUES
  (1,0,0,0,1,0);
--寻找最大port
select max(edgex_port) from IO_Link_Device;
update IO_Link_Device set edgex_port=%d  deviceservice_create_flag=1 where device_id = %d;
select device_id from IO_Link_Device where  appservice_monitor_flag = 0;

--删除行
delete from IO_Link_Device where device_id = 388 or device_id = 496;*/
