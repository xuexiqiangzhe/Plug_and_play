#include <ecrt.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "open62541.h"
#include <signal.h>
#include "388model.h"
#include "496model.h"
#include <pthread.h>

//mysql相关
#include <mysql/mysql.h>/*注意要包含这个头文件*/
#pragma comment(lib, "libmysql")
/*定义了一些数据库连接需要的宏*/
#define HOST "localhost"
#define USERNAME "root"
#define PASSWORD "1"
#define DATABASE "Plug_and_play"
//mysql写接口封装
void exe_sql(char *sql)
{
    MYSQL my_connection; /*这是一个数据库连接*/
    int res;             /*执行sql語句后的返回标志*/
    /*初始化mysql连接my_connection*/
    mysql_init(&my_connection);
    /*这里就是用了mysql.h里的一个函数，用我们之前定义的那些宏建立mysql连接，并
    返回一个值，返回不为空证明连接是成功的*/
    if (mysql_real_connect(&my_connection, HOST, USERNAME, PASSWORD, DATABASE, 0, NULL,
                           CLIENT_FOUND_ROWS))
    { /*连接成功*/
        printf("数据库执行exe_sql连接成功！n");
        /*这句话是设置查询编码为utf8，这样支持中文*/
        mysql_query(&my_connection, "set names utf8");
        /*下面这句话就是用mysql_query函数来执行我们刚刚传入的sql語句，
        这会返回一个int值，如果为0，证明語句执行成功*/
        res = mysql_query(&my_connection, sql);
        if (res)
        { /*现在就代表执行失败了*/
            printf("Error： mysql_query !\n");
            /*不要忘了关闭连接*/
            mysql_close(&my_connection);
        }
        else
        { /*现在就代表执行成功了*/
            /*mysql_affected_rows会返回执行sql后影响的行数*/
            printf("%d 行受到影响！\n", mysql_affected_rows(&my_connection));
            /*不要忘了关闭连接*/
            mysql_close(&my_connection);
        }
    }
    else
    {
        /*数据库连接失败*/
        printf("数据库执行exe_sql连接失败！\n");
    }
}
//mysql读接口封装
void query_sql(char *sql)
{
    MYSQL my_connection;  /*这是一个数据库连接*/
    int res;              /*执行sql語句后的返回标志*/
    MYSQL_RES *res_ptr;   /*指向查询结果的指针*/
    MYSQL_FIELD *field;   /*字段结构指针*/
    MYSQL_ROW result_row; /*按行返回的查询信息*/
    int row, column;      /*查询返回的行数和列数*/
    int i, j;             /*只是控制循环的两个变量*/
    /*初始化mysql连接my_connection*/
    mysql_init(&my_connection);
    /*这里就是用了mysql.h里的一个函数，用我们之前定义的那些宏建立mysql连接，并
    返回一个值，返回不为空证明连接是成功的*/
    if (mysql_real_connect(&my_connection, HOST, USERNAME, PASSWORD, DATABASE, 0, NULL,
                           CLIENT_FOUND_ROWS))
    { /*连接成功*/
        printf("数据库查询query_sql连接成功！\n");
        /*这句话是设置查询编码为utf8，这样支持中文*/
        mysql_query(&my_connection, "set names utf8");
        res = mysql_query(&my_connection, sql);
        if (res)
        {
            /*现在就代表执行失败了*/
            printf("Error： mysql_query !\n");
            /*不要忘了关闭连接*/
            mysql_close(&my_connection);
        }
        else
        { /*现在就代表执行成功了*/
            /*将查询的結果给res_ptr*/
            res_ptr = mysql_store_result(&my_connection);
            /*如果结果不为空，就把结果print*/
            if (res_ptr)
            {
                /*取得結果的行数和*/
                column = mysql_num_fields(res_ptr);
                row = mysql_num_rows(res_ptr) + 1;
                printf("查询到 %lu 行 \n", row);
                /*输出結果的字段名*/
                for (i = 0; field = mysql_fetch_field(res_ptr); i++)
                    printf("%st", field->name);
                printf("\n");
                /*按行输出結果*/
                for (i = 1; i < row; i++)
                {
                    result_row = mysql_fetch_row(res_ptr);
                    for (j = 0; j < column; j++)
                        printf("%st", result_row[j]);
                    printf("\n");
                }
            }
            /*不要忘了关闭连接*/
            mysql_close(&my_connection);
        }
    }
}

//EtherCAT相关
//从站配置所用的参数
#define EL6224AP 0, 1                   //EL6224在ethercat总线上的位置
#define EL6224VP 0x00000002, 0x18503052 // EL6224的vendor ID和产品标识(Product code)

// CoE对象字典
#define RXPDO 0x1600
#define TXPDO 0x1A00

struct EL6224 el6224;
int monitor_change =1;
pthread_t ethercat;
//迈信伺服驱动器里PDO入口的偏移量
/*我们需要定义一些变量去关联需要用到的从站的PD0对象*/
struct DRIVERVARIABLE
{
    int port1data[4]; //port1数据
    int port2data[4]; //port2数据
    int port3data[4]; //port3数据
 int port4data[4]; //port4数据
};

//迈信伺服电机结构体
struct EL6224
{
    //关于ethercat master
    ec_master_t *master;            //主站
    ec_master_state_t master_state; //主站状态

    ec_domain_t *domain;            //域
    ec_domain_state_t domain_state; //域状态

    ec_slave_config_t *EL6224_EP3E;            //从站配置，这里只有一台迈信伺服
    ec_slave_config_state_t EL6224_EP3E_state; //从站配置状态
    int port1data[4];                 //port1数据
int port2data[4];                 //port2数据
    int port3data[4];                 //port3数据
    int port4data[4];                 //port4数据

    uint8_t *domain_pd;                    // Process Data
    struct DRIVERVARIABLE drive_variables; //从站驱动器变量
};

static void
check_domain_state(ec_domain_t *domain, ec_domain_state_t *domain_state)
{
    ec_domain_state_t ds;
    ecrt_domain_state(domain, &ds);
    if (ds.working_counter != domain_state->working_counter)
    {
        // printf("Domain: WC %u.\n", ds.working_counter);
    }
    if (ds.wc_state != domain_state->wc_state)
    {
        // printf("Domain: State %u.\n", ds.wc_state);
    }
    *domain_state = ds;
}
static void
check_master_state(ec_master_t *master, ec_master_state_t *master_state)
{
    ec_master_state_t ms;
    ecrt_master_state(master, &ms);
    if (ms.slaves_responding != master_state->slaves_responding)
    {
        // printf("%u slave(s).\n", ms.slaves_responding);
    }
    if (ms.al_states != master_state->al_states)
    {
        // printf("AL states: 0x%02X.\n", ms.al_states);
    }
    if (ms.link_up != master_state->link_up)
    {
        // printf("Link is %s.\n", ms.link_up ? "up" : "down");
    }
    *master_state = ms;
}
static void
check_slave_config_states(ec_slave_config_t *slave,
                          ec_slave_config_state_t *slave_state)
{
    ec_slave_config_state_t s;
    ecrt_slave_config_state(slave, &s);
    if (s.al_state != slave_state->al_state)
    {
        //  printf("EP3E: State 0x%02X.\n", s.al_state);
    }
    if (s.online != slave_state->online)
    {
        // printf("EP3E: %s.\n", s.online ? "online" : "offline");
    }
    if (s.operational != slave_state->operational)
    {
        // printf("EP3E: %soperational.\n", s.operational ? "" : "Not ");
    }
    *slave_state = s;
}

//OPCUA相关
UA_Boolean running = 1;

struct portdevice
{
    bool port[4];
    int device_port[4];
    int nodeid_begin[4];
};
struct portdevice ports;
//信号
static void stopHandler(int sign)
{
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "received ctrl-c");
    running = false;
}

//初始化EtherCAT主站函数
static void
init_EtherCAT_master(struct EL6224 *el6224)
{
    //变量与对应PDO数据对象关联
    ec_pdo_entry_reg_t domain_regs[] = {
        {EL6224AP, EL6224VP, 0x6000, 0x01, &el6224->drive_variables.port1data[0]},
        {EL6224AP, EL6224VP, 0x6000, 0x02, &el6224->drive_variables.port1data[1]},
        {EL6224AP, EL6224VP, 0x6000, 0x03, &el6224->drive_variables.port1data[2]},
{EL6224AP, EL6224VP, 0x6000, 0x04, &el6224->drive_variables.port1data[3]},

        {EL6224AP, EL6224VP, 0x6010, 0x01, &el6224->drive_variables.port2data[0]},
        {EL6224AP, EL6224VP, 0x6010, 0x02, &el6224->drive_variables.port2data[1]},
 {EL6224AP, EL6224VP, 0x6010, 0x03, &el6224->drive_variables.port2data[2]},
        {EL6224AP, EL6224VP, 0x6010, 0x04, &el6224->drive_variables.port2data[3]},

        {EL6224AP, EL6224VP, 0x6020, 0x01, &el6224->drive_variables.port3data[0]},
        {EL6224AP, EL6224VP, 0x6020, 0x02, &el6224->drive_variables.port3data[1]},
        {EL6224AP, EL6224VP, 0x6020, 0x03, &el6224->drive_variables.port3data[2]},
        {EL6224AP, EL6224VP, 0x6020, 0x04, &el6224->drive_variables.port3data[3]},


        {EL6224AP, EL6224VP, 0x6030, 0x01, &el6224->drive_variables.port4data[0]},
        {EL6224AP, EL6224VP, 0x6030, 0x02, &el6224->drive_variables.port4data[1]},
        {EL6224AP, EL6224VP, 0x6030, 0x03, &el6224->drive_variables.port4data[2]},
        {EL6224AP, EL6224VP, 0x6030, 0x04, &el6224->drive_variables.port4data[3]},

        {}};

    //填充相关PDOS信息
    ec_pdo_entry_info_t slave_1_pdo_entries[] = {
        {0x6000, 0x01, 8}, 
        {0x6000, 0x02, 8}, 
         {0x6000, 0x03, 8}, 
          {0x6000, 0x04, 8}, 

        {0x6010, 0x01, 8},
        {0x6010, 0x02, 8}, 
         {0x6010, 0x03, 8}, 
          {0x6010, 0x04, 8}, 

        {0x6020, 0x01, 8}, 
        {0x6020, 0x02, 8}, 
         {0x6020, 0x03, 8}, 
          {0x6020, 0x04, 8}, 

        {0x6030, 0x01, 8}, 
        {0x6030, 0x02, 8},
        {0x6030, 0x03, 8}, 
        {0x6030, 0x04, 8}, 
    };

    ec_pdo_info_t slave_1_pdos[] = {
        {0x1A00, 4, slave_1_pdo_entries + 0},
        {0x1A01, 4, slave_1_pdo_entries + 4},
        {0x1A02, 4, slave_1_pdo_entries + 8},
        {0x1A03, 4, slave_1_pdo_entries + 12},
    };
  
    ec_sync_info_t slave_1_syncs[] = {
        {0, EC_DIR_OUTPUT, 0, NULL, EC_WD_DISABLE},
        {1, EC_DIR_INPUT, 0, NULL, EC_WD_DISABLE},
        {2, EC_DIR_OUTPUT, 0, NULL, EC_WD_DISABLE},
        {3, EC_DIR_INPUT, 4, slave_1_pdos + 0, EC_WD_DISABLE},
        {0xff}};
 
    //创建ethercat主站master
    el6224->master = ecrt_request_master(0);
    if (!el6224->master)
    {
        printf("Failed to create ethercat master!\n");
        exit(EXIT_FAILURE); //创建失败，退出线程
    }

    //创建域domain
    el6224->domain = ecrt_master_create_domain(el6224->master);
    if (!el6224->domain)
    {
        printf("Failed to create master domain!\n");
        exit(EXIT_FAILURE); //创建失败，退出线程
    }

    //配置从站
    if (!(el6224->EL6224_EP3E =
              ecrt_master_slave_config(el6224->master, EL6224AP, EL6224VP)))
    {
        printf("Failed to get slave configuration for EP3E!\n");
        exit(EXIT_FAILURE); //配置失败，退出线程
    }

    //配置PDOs
    printf("Configuring PDOs...\n");
    if (ecrt_slave_config_pdos(el6224->EL6224_EP3E, EC_END /*没改*/, slave_1_syncs))
    {
        printf("Failed to configure EP3E PDOs!\n");
        exit(EXIT_FAILURE); //配置失败，退出线程
    }
    else
    {
        printf("*Success to configuring EP3E PDOs*\n"); //配置成功
    }

    //注册PDO entry
    if (ecrt_domain_reg_pdo_entry_list(el6224->domain, domain_regs))
    {
        printf("PDO entry registration failed!\n");
        exit(EXIT_FAILURE); //注册失败，退出线程
    }
    else
    {
        printf("*Success to configuring EP3E PDO entry*\n"); //注册成功
    }

    //激活主站master
    printf("Activating master...\n");
    if (ecrt_master_activate(el6224->master))
    {
        exit(EXIT_FAILURE); //激活失败，退出线程
    }
    else
    {
        printf("*Master activated*\n"); //激活成功
    }
    if (!(el6224->domain_pd = ecrt_domain_data(el6224->domain)))
    {
        exit(EXIT_FAILURE);
    }
}

//打印二进制到命令行，方便调试用
void printb(int n)
{
    int a[20];
    int i=0;
    while (n) {
        a[i] = n&1;
        i++;
        n >>= 1;
    }
    i = i - 1;
    while(i >= 0){
        printf("%d", a[i]);
        i--;
    }
    printf("\n");
}
//线程2Ethercat
void *ethercat_thread(void *arg)
{
    //等待OPCUA服务器运行
    sleep(1);
    UA_Server *server = (UA_Server *)arg;
    printf("*It's working now*\n");

    //monitor_change是给monitor线程提供一个接口，控制本线程是否开启循环。
    while (running&&monitor_change)
    {
        //接收过程数据
        ecrt_master_receive(el6224.master);
        ecrt_domain_process(el6224.domain);

        //检查过程数据状态（可选）
        check_domain_state(el6224.domain, &el6224.domain_state);
        //检查主站状态
        check_master_state(el6224.master, &el6224.master_state);
        //检查从站配置状态
        check_slave_config_states(el6224.EL6224_EP3E, &el6224.EL6224_EP3E_state);

        //读取数据PORT1，四个8位的数。
        el6224.port1data[0] =
            EC_READ_U8(el6224.domain_pd + el6224.drive_variables.port1data[0]); 
        el6224.port1data[1] =
            EC_READ_U8(el6224.domain_pd + el6224.drive_variables.port1data[1]); 
        el6224.port1data[2] =
            EC_READ_U8(el6224.domain_pd + el6224.drive_variables.port1data[2]); 
        el6224.port1data[3] =
            EC_READ_U8(el6224.domain_pd + el6224.drive_variables.port1data[3]); 
        //通用的，会将这4个8位的数，加载到port1的设备的OPCUA信息模型中
        if( ports.port[0] == 1){
             char *idstring = (char *)malloc(sizeof("3800")); //分配动态内存
             //第一个数据的节点，如388设备的是3880
            sprintf(idstring, "%d", ports.nodeid_begin[0] + 1);
            UA_NodeId NodeId = UA_NODEID_STRING(1, idstring); 
             UA_Variant subCounter;
             UA_Variant_init(&subCounter);
             UA_Server_readValue(server, NodeId, &subCounter);
             //读取如OPCUA节点38801，得PDO开始的节点
            int PDO_begin = *(int *)subCounter.data;
            //最后一个数据的节点
            sprintf(idstring, "%d", ports.nodeid_begin[0] + 2);
            NodeId = UA_NODEID_STRING(1, idstring); 
             UA_Server_readValue(server, NodeId, &subCounter);
            //读取如OPCUA节点38802，得PDO结束的节点
            int PDO_end = *(int *)subCounter.data;
            UA_Variant_clear(&subCounter);
            
            //port1的数据写入信息模型节点PDO_begin～PDO_end
            for (int j=0,i = PDO_begin; i <=PDO_end; i++,j++){
            char *idstring = (char *)malloc(sizeof("3800")); 
            sprintf(idstring, "%d", i);
            UA_NodeId NodeId = UA_NODEID_STRING(1, idstring); 
            UA_Variant value;
            UA_Variant_setScalar(&value, &el6224.port1data[j], &UA_TYPES[UA_TYPES_INT32]);
            UA_Server_writeValue(server, NodeId, value); //不存在节点则写入跳过
            }
          //  printf("Port1开启\n电容%d\n",(el6224.port1data[0]*256+ el6224.port1data[1])>>3);

        }
        

        //读取数据PORT2
        el6224.port2data[0] =
            EC_READ_U8(el6224.domain_pd + el6224.drive_variables.port2data[0]); //状态字
        el6224.port2data[1] =
            EC_READ_U8(el6224.domain_pd + el6224.drive_variables.port2data[1]); //状态字
         el6224.port2data[2] =
            EC_READ_U8(el6224.domain_pd + el6224.drive_variables.port2data[2]); //状态字
        el6224.port2data[3] =
            EC_READ_U8(el6224.domain_pd + el6224.drive_variables.port2data[3]); //状态字

        //读取数据PORT3
        el6224.port3data[0] =
            EC_READ_U8(el6224.domain_pd + el6224.drive_variables.port3data[0]); //状态字
        el6224.port3data[1] =
            EC_READ_U8(el6224.domain_pd + el6224.drive_variables.port3data[1]); //状态字
        el6224.port3data[2] =
            EC_READ_U8(el6224.domain_pd + el6224.drive_variables.port3data[2]); //状态字
        el6224.port3data[3] =
            EC_READ_U8(el6224.domain_pd + el6224.drive_variables.port3data[3]); //状态字

        //读取数据PORT4
        el6224.port4data[0] =
            EC_READ_U8(el6224.domain_pd + el6224.drive_variables.port4data[0]); //状态字
        el6224.port4data[1] =
            EC_READ_U8(el6224.domain_pd + el6224.drive_variables.port4data[1]); //状态字
        el6224.port4data[2] =
            EC_READ_U8(el6224.domain_pd + el6224.drive_variables.port4data[2]); //状态字
        el6224.port4data[3] =
            EC_READ_U8(el6224.domain_pd + el6224.drive_variables.port4data[3]); //状态字
        //port4的数据写入信息模型
        if( ports.port[3] == 1){
             char *idstring = (char *)malloc(sizeof("3800")); //分配动态内存
              UA_Variant subCounter;
             UA_Variant_init(&subCounter);
             //通过port4的信息模型节点，找到第一个过程数据数据的节点
            sprintf(idstring, "%d", ports.nodeid_begin[3] + 1);
            UA_NodeId NodeId = UA_NODEID_STRING(1, idstring); 
             UA_Server_readValue(server, NodeId, &subCounter);
            int PDO_begin = *(int *)subCounter.data;
           //通过port4的信息模型节点，找到最后一个过程数据数据的节点
            sprintf(idstring, "%d", ports.nodeid_begin[3] + 2);
            NodeId = UA_NODEID_STRING(1, idstring);
             UA_Server_readValue(server, NodeId, &subCounter);
            int PDO_end = *(int *)subCounter.data;
            UA_Variant_clear(&subCounter);

            //port4的数据写入信息模型
            for (int j=0,i = PDO_begin; i <=PDO_end; i++,j++){
            char *idstring = (char *)malloc(sizeof("3800")); 
            sprintf(idstring, "%d", i);
            UA_NodeId NodeId = UA_NODEID_STRING(1, idstring);
            UA_Variant value;
            UA_Variant_setScalar(&value, &el6224.port4data[j], &UA_TYPES[UA_TYPES_INT32]);
            UA_Server_writeValue(server, NodeId, value); //不存在节点则写入跳过
            }
           int rotate_speed=el6224.port4data[0]*256+ el6224.port4data[1];
        /*  if(rotate_speed<=32760 )
          printf("Port4开启\n编码器转速%ld\t", rotate_speed);
          else
         printf("编码器转速-%ld\t", 65536-rotate_speed);
        printf("编码器计数器%ld\n", el6224.port4data[2]*64+( el6224.port4data[3]>>2));*/

        }
        //调试
        printf("port1数据 %d\t%d\t%d\t%d\tport2数据 %d\t%d\t%d\t%d\tport3数据 %d\t%d\t%d\t%d\tport4数据 %d\t%d\t%d\t%d\n",
               el6224.port1data[0], el6224.port1data[1],el6224.port1data[2], el6224.port1data[3],
               el6224.port2data[0], el6224.port2data[1],el6224.port2data[2], el6224.port2data[3],
               el6224.port3data[0], el6224.port3data[1],el6224.port3data[2], el6224.port3data[3],
               el6224.port4data[0], el6224.port4data[1], el6224.port4data[2], el6224.port4data[3]);
        //把数据发回Ethercat从站即EL6224，使其保持OP状态
        ecrt_domain_queue(el6224.domain);
        ecrt_master_send(el6224.master);
        usleep(1000);
    }
    //注销EtherCAT主站
    printf("注销EtherCAT主站\n");
    ecrt_master_deactivate(el6224.master);
}

//二、OPCUA相关
//加载模型
void loadmodel(UA_Server *server, int deviceid, int port)
{
    switch (deviceid)
    {
    case 388:
        load388model(server); //388的信息模型在地址空间中的节点被写死了38800～38899
        break;
    case 496:
        load496model(server);
        break;
    }
}
//卸载模型
void unloadmodel(UA_Server *server, int deviceid)
{
    switch (deviceid)
    {
    case 388:
        unload388model(server); //388的信息模型在地址空间中的节点被写死了38800～38899
        break;
    case 496:
        unload496model(server);
        break;
    }
}

//封装的发送POST请求接口
#define IPSTR "127.0.0.1"
#define PORT 8080
#define BUFSIZE 1024
void POST(void *arg,int deviceid)
{
    UA_Server *server = (UA_Server *)arg;
        char *idstring = (char *)malloc(sizeof("3800")); //分配动态内存
        UA_Variant subCounter;
        UA_Variant_init(&subCounter);
        //通过port4的信息模型节点，找到第一个过程数据数据的节点
        sprintf(idstring, "%d",deviceid*100 + 1);
            UA_NodeId NodeId = UA_NODEID_STRING(1, idstring); 
             UA_Server_readValue(server, NodeId, &subCounter);
            int PDO_begin = *(int *)subCounter.data;
           //通过port4的信息模型节点，找到最后一个过程数据数据的节点
            sprintf(idstring, "%d", deviceid*100 +  2);
            NodeId = UA_NODEID_STRING(1, idstring);
             UA_Server_readValue(server, NodeId, &subCounter);
        int PDO_end = *(int *)subCounter.data;

        int sockfd, ret, i, h;
        struct sockaddr_in servaddr;
        char str1[4096], str2[4096], buf[BUFSIZE], *str;
        socklen_t len;
        struct timeval  tv;
 
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
                printf("创建网络连接失败,本线程即将终止---socket error!\n");
                return;
        };
 
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(PORT);
        if (inet_pton(AF_INET, IPSTR, &servaddr.sin_addr) <= 0 ){
                printf("创建网络连接失败,本线程即将终止--inet_pton error!\n");
                return;
        };
 
        if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
                printf("POST到服务器失败,connect error!\n");
                return;
        }
        printf("与远端建立了连接\n");
 
        memset(str2, 0, 4096);
        sprintf(str2, "deviceid=%d&begin_node=%d&end_node=%d",deviceid,PDO_begin,PDO_end);
        str=(char *)malloc(128);
        len = strlen(str2);
        sprintf(str, "%d", len);
 
        memset(str1, 0, 4096);
        strcat(str1, "POST /api/v1/upload HTTP/1.1\n");
        strcat(str1, "Host: www.webxml.com.cn\n");
        strcat(str1, "Content-Type: application/x-www-form-urlencoded\n");
        strcat(str1, "Content-Length: ");
        strcat(str1, str);
        strcat(str1, "\n\n");
        //str2的值为post的数据
        strcat(str1, str2);
        strcat(str1, "\r\n\r\n");
        printf("%s\n",str1);
 
        ret = write(sockfd,str1,strlen(str1));
        if (ret < 0) {
                printf("发送失败！错误代码是%d，错误信息是'%s'\n",errno, strerror(errno));
                return;
        }else{
                printf("消息发送成功，共发送了%d个字节！\n\n", ret);
        }
        close(sockfd);
 
 
        return;
}

//监控插入线程
void *monitordeviceplug(void *arg)
{
    //睡一秒等OPCUA服务器先运行起来
    sleep(1);
    UA_Server *server = (UA_Server *)arg;
    //ports设备插入标志初始置0
    ports.port[0] = 0;
    ports.port[1] = 0;
    ports.port[2] = 0;
    ports.port[3] = 0;

    while (running)
    {
        //轮训EL6224的四个端口
        for (int i = 1; i <= 4; i++)
        {
            //如果该端口设备插入标志位为0，则尝试进行初始化
            if (ports.port[i - 1] == 0) 
            {
                FILE *fp;
                char buffer[80];
                char *cmd = (char *)malloc(500); //分配动态内存
                //shell读SDO寄存器0x90n0 0x24，代表PDO长度
                sprintf(cmd, "ethercat upload -p 1 0x90%d0 0x24", i - 1);
                fp = popen(cmd, "r"); //运行shell指令
                //获得返回值，装入buffer
                fgets(buffer, sizeof(buffer), fp);
                //printf("命令行操作返回值：%s\n", buffer);
                //把buffer转int，得到PDO长度
                int PDO_LEN_FLAG = 0;
                for (int j = 5; j < sizeof(buffer); j++)
                {
                    if (buffer[j] == ' ' || buffer[j] == 10)
                        break;
                    PDO_LEN_FLAG = PDO_LEN_FLAG * 10 + buffer[j] - 48;
                }
                //如果PDO长度不为0，说明SDO被加载，代表该端口有设备插入了。
                if (PDO_LEN_FLAG != 0)
                {
                    //PDO长度写入0x80n0 0x24
                    sprintf(cmd, "ethercat download -p 1 0x80%d0 0x24 %d", i - 1, PDO_LEN_FLAG);
                    fp = popen(cmd, "r");
                    sleep(1);

                    //此时，可以稳定的读取到deviceid
                    sprintf(cmd, "ethercat upload -p 1 0x90%d0 0x04", i - 1);
                    fp = popen(cmd, "r"); //运行脚本文件，获得返回值，装入buffer，方法1
                    fgets(buffer, sizeof(buffer), fp);
                    pclose(fp);
                    //解析buffer为int
                    int j = 11;
                    ports.device_port[i - 1] = 0;
                    for (; j < sizeof(buffer); j++)
                    {

                        if (buffer[j] == ' ' || buffer[j] == 10)
                            break;
                        ports.device_port[i - 1] = ports.device_port[i - 1] * 10 + buffer[j] - 48;
                    }
                    //二重保障，deviceid结果不是0或者256,说明该端口设备插入
                    if (ports.device_port[i - 1] != 0 && ports.device_port[i - 1] != 256) 
                    {
                        //OPCUA信息模型起始节点被我定义为deviceid*100
                        ports.nodeid_begin[i - 1] = ports.device_port[i - 1] * 100;
                        printf("端口%d，设备已经插入，设备id：%d\n", i, ports.device_port[i - 1]);

                        //加载信息模型
                        if (ports.device_port[i - 1] < 0 || ports.device_port[i - 1] > 10000)
                        { //无效id
                            printf("捕捉到无效id……\n");
                            break;
                        }
                        loadmodel(server, ports.device_port[i - 1], i);

                        //加载数据库表
                        char *sql = (char *)malloc(500); //分配动态内存
                        sprintf(sql, "INSERT into IO_Link_Device(device_id,deviceservice_create_flag,appservice_monitor_flag,js_show_flag,io_link_port,edgex_port)\nVALUES\n(%d,0,0,0,%d,0);",
                                ports.device_port[i - 1], i);
                        exe_sql(sql);

                        //注销EtherCAT线程，回收ethercat主站
                        monitor_change=0;
                        usleep(100000);
                        pthread_join(ethercat, NULL);
                        //端口已启用标志
                        ports.port[i - 1] = 1;
                        //重新配置PDO等，PDO配置写死了的
                        ec_pdo_entry_reg_t domain_regs[] = {
                            {EL6224AP, EL6224VP, 0x6000, 0x01, &el6224.drive_variables.port1data[0]},
                            {EL6224AP, EL6224VP, 0x6000, 0x02, &el6224.drive_variables.port1data[1]},
                            {EL6224AP, EL6224VP, 0x6000, 0x03, &el6224.drive_variables.port1data[2]},
                            {EL6224AP, EL6224VP, 0x6000, 0x04, &el6224.drive_variables.port1data[3]},

                            {EL6224AP, EL6224VP, 0x6010, 0x01, &el6224.drive_variables.port2data[0]},
                            {EL6224AP, EL6224VP, 0x6010, 0x02, &el6224.drive_variables.port2data[1]},
                            {EL6224AP, EL6224VP, 0x6010, 0x03, &el6224.drive_variables.port2data[2]},
                            {EL6224AP, EL6224VP, 0x6010, 0x04, &el6224.drive_variables.port2data[3]},

                            {EL6224AP, EL6224VP, 0x6020, 0x01, &el6224.drive_variables.port3data[0]},
                            {EL6224AP, EL6224VP, 0x6020, 0x02, &el6224.drive_variables.port3data[1]},
                            {EL6224AP, EL6224VP, 0x6020, 0x03, &el6224.drive_variables.port3data[2]},
                            {EL6224AP, EL6224VP, 0x6020, 0x04, &el6224.drive_variables.port3data[3]},

                            {EL6224AP, EL6224VP, 0x6030, 0x01, &el6224.drive_variables.port4data[0]},
                            {EL6224AP, EL6224VP, 0x6030, 0x02, &el6224.drive_variables.port4data[1]},
                            {EL6224AP, EL6224VP, 0x6030, 0x03, &el6224.drive_variables.port4data[2]},
                            {EL6224AP, EL6224VP, 0x6030, 0x04, &el6224.drive_variables.port4data[3]},

                            {}};

                        ec_pdo_entry_info_t slave_1_pdo_entries[] = {
                            {0x6000, 0x01, 8},
                            {0x6000, 0x02, 8},
                            {0x6000, 0x03, 8},
                            {0x6000, 0x04, 8},

                            {0x6010, 0x01, 8},
                            {0x6010, 0x02, 8},
                            {0x6010, 0x03, 8},
                            {0x6010, 0x04, 8},

                            {0x6020, 0x01, 8},
                            {0x6020, 0x02, 8},
                            {0x6020, 0x03, 8},
                            {0x6020, 0x04, 8},

                            {0x6030, 0x01, 8},
                            {0x6030, 0x02, 8},
                            {0x6030, 0x03, 8},
                            {0x6030, 0x04, 8},
                        };

                        ec_pdo_info_t slave_1_pdos[] = {
                            {0x1A00, 4, slave_1_pdo_entries + 0},
                            {0x1A01, 4, slave_1_pdo_entries + 4},
                            {0x1A02, 4, slave_1_pdo_entries + 8},
                            {0x1A03, 4, slave_1_pdo_entries + 12},
                        };

                        ec_sync_info_t slave_1_syncs[] = {
                            {0, EC_DIR_OUTPUT, 0, NULL, EC_WD_DISABLE},
                            {1, EC_DIR_INPUT, 0, NULL, EC_WD_DISABLE},
                            {2, EC_DIR_OUTPUT, 0, NULL, EC_WD_DISABLE},
                            {3, EC_DIR_INPUT, 4, slave_1_pdos + 0, EC_WD_DISABLE},
                            {0xff}};

                        //创建域domain
                        el6224.domain = ecrt_master_create_domain(el6224.master);
                        if (!el6224.domain)
                        {
                            printf("Failed to create master domain!\n");
                            exit(EXIT_FAILURE); //创建失败，退出线程
                        }

                        //配置从站
                        if (!(el6224.EL6224_EP3E =
                                  ecrt_master_slave_config(el6224.master, EL6224AP, EL6224VP)))
                        {
                            printf("Failed to get slave configuration for EP3E!\n");
                            exit(EXIT_FAILURE); //配置失败，退出线程
                        }

                        //配置PDOs
                        printf("Configuring PDOs...\n");
                        if (ecrt_slave_config_pdos(el6224.EL6224_EP3E, EC_END /*没改*/, slave_1_syncs))
                        {
                            printf("Failed to configure EP3E PDOs!\n");
                            exit(EXIT_FAILURE); //配置失败，退出线程
                        }
                        else
                        {
                            printf("*Success to configuring EP3E PDOs*\n"); //配置成功
                        }

                        //注册PDO entry
                        if (ecrt_domain_reg_pdo_entry_list(el6224.domain, domain_regs))
                        {
                            printf("PDO entry registration failed!\n");
                            exit(EXIT_FAILURE); //注册失败，退出线程
                        }
                        else
                        {
                            printf("*Success to configuring EP3E PDO entry*\n"); //注册成功
                        }

                        //激活主站master
                        printf("Activating master...\n");
                        if (ecrt_master_activate(el6224.master))
                        {
                            exit(EXIT_FAILURE); //激活失败，退出线程
                        }
                        else
                        {
                            printf("*Master activated*\n"); //激活成功
                        }
                        if (!(el6224.domain_pd = ecrt_domain_data(el6224.domain)))
                        {
                            exit(EXIT_FAILURE);
                        }
                        //允许开启Ethercat线程
                        monitor_change=1;
                        //开启Ethercat线程
                        pthread_create(&ethercat, NULL, ethercat_thread, server);

                        //向设备管理为服务发起注册请求
                        POST(server,ports.device_port[i - 1]);
                        usleep(500);
                    }
                }
            } /*else{//如果已经有设备插入，看是否需要注销
                FILE *fp;
                char buffer[80];
                //扫描porti的设备id和厂家id
                char* cmd=sprintf("ethercat upload -p 1 0x80%d0 0x04",i-1);
                fp = popen(cmd, "r");//运行脚本文件，获得返回值，装入buffer
                fgets(buffer, sizeof(buffer), fp);
                pclose(fp);
                int size=sizeof(buffer);
                for(int i=0;i<size;i++){
                    if(buffer[i]!='0')
                        goto notunload;//设备还在，不卸载信息模型
                }
                    unloadmodel(server,ports.device_port[i-1]);
                    ports.device_port[i-1]=-1;
                    ports.port[i-1]=-1;
                    ports.nodeid_begin[i-1]=-1;
                notunload:;
            }*/
        }
        //每个1秒一次扫描
        sleep(1);
    }
    //程序ctrl+c后要删除数据库表
    for (int i = 1; i <= 4; i++)
        {
            if(ports.port[i - 1] != 0){
                char *sql = (char *)malloc(500); //分配动态内存
                    sprintf(sql, "delete from IO_Link_Device where device_id = %d;",ports.device_port[i - 1]);
                    exe_sql(sql);
            }      
        }
}


int main(int argc, char **argv)
{
    //注册信号，收到ctrl+c，则结束所有线程，通过running标志位
    signal(SIGINT, stopHandler); 
    //OPCUA服务器初始化
    UA_Server *server = UA_Server_new();
    UA_ServerConfig *config = UA_Server_getConfig(server);
    UA_ServerConfig_setMinimal(config, 62541, NULL);

    //IO-Link初始化配置0x80n0 0x28寄存器
    char *cmd = (char *)malloc(100); //分配动态内存,要足够大
    for(int i=0;i<=3;i++){
            FILE *fp;
            char buffer[80];
            sprintf(cmd, "ethercat download -p1 0x80%d0 0x28 0x0023", i);
            //printf("%s\n", cmd);
            fp = popen(cmd, "r"); //运行脚本文件，获得返回值，装入buffer，方法1
                fgets(buffer, sizeof(buffer), fp);
                printf("命令行操作返回值：%s\n", buffer);
                pclose(fp);
    }

    //线程1：monitor线程，扫描设备，模型管理
    pthread_t monitor;
    pthread_create(&monitor, NULL, monitordeviceplug, server);

    //线程2：EtherCAT线程，获取PDO
    init_EtherCAT_master(&el6224);//初始化Ethercat全局结构体
    pthread_create(&ethercat, NULL, ethercat_thread, server);

    //线程3：运行OPCUAserver，main在这阻塞
    UA_StatusCode retval = UA_Server_run(server, &running);

    //ctrl+c后，等待线程资源回收完毕如ethercat，再销毁线程
    sleep(2);
    //退出线程1、2
    int returnValue = pthread_join(monitor, NULL);
    if (returnValue != 0) {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,"\nPthread Join Failed for User thread:%d\n", returnValue);
    }
    returnValue = pthread_join(ethercat, NULL);
    if (returnValue != 0) {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,"\nPthread Join Failed for User thread:%d\n", returnValue);
    }
    //回收OPCUA服务器
    UA_Server_delete(server);
    
    return 0;
}
