#include "open62541.h"
#include "edgex/devsdk.h"

#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define ERR_CHECK(x)                                                                     \
    if(x.code) {                                                                         \
        fprintf(stderr, "Error: %d: %s\n", x.code, x.reason);                            \
        edgex_device_service_free(service);                                              \
        free(impl);                                                                      \
        return x.code;                                                                   \
    }

//驱动结构体
typedef struct template_driver {
    iot_logger_t *lc;
} template_driver;

UA_Client *client;
// edgex接口，初始化
static bool
template_init(void *impl, struct iot_logger_t *lc, const edgex_nvpairs *config) {
    template_driver *driver = (template_driver *)impl;
    driver->lc = lc;
    iot_log_debug(driver->lc, "Init");
    return true;
}

// edgex接口，发现
static void
template_discover(void *impl) {}

//读命令跳转入口
UA_Client *client;
int start_node;
int end_node;
static edgex_device_commandresult opcua_to_edgex(UA_Variant *value)
{
  edgex_device_commandresult result;
  memset(&result, 0, sizeof(edgex_device_commandresult));
  if (!value || !value->type)
  {
    return result;
  }

  switch (value->type->typeIndex)
  {
    case UA_TYPES_BOOLEAN:
      result.type = Bool;
      result.value.bool_result = *(UA_Boolean *)value->data;
      break;
    case UA_TYPES_STRING:
      result.type = String;
      UA_String data = *(UA_String *)value->data;
      char *convert = (char *)UA_malloc(sizeof (char) * data.length + 1);
      memcpy(convert, data.data, data.length);
      convert[data.length] = '\0';
      result.value.string_result = convert;
     break;
    case UA_TYPES_BYTE:
      result.type = Uint8;
      result.value.ui8_result = *(UA_Byte *)value->data;
       break;
    case UA_TYPES_UINT16:
      result.type = Uint16;
      result.value.ui16_result = *(UA_UInt16 *)value->data;
       break;
    case UA_TYPES_UINT32:
      result.type = Uint32;
      result.value.ui32_result = *(UA_UInt32 *)value->data;
      break;
    case UA_TYPES_UINT64:
      result.type = Uint64;
      result.value.ui64_result = *(UA_UInt64 *)value->data;
   break;
    case UA_TYPES_SBYTE:
      result.type = Int8;
      result.value.i8_result = *(UA_SByte *)value->data;
      break;
    case UA_TYPES_INT16:
      result.type = Int16;
      result.value.i16_result = *(UA_Int16 *)value->data;
      break;
    case UA_TYPES_INT32:
      result.type = Int32;
      result.value.i32_result = *(UA_Int32 *)value->data; //要装数据改这里
      break;
    case UA_TYPES_DATETIME:
    case UA_TYPES_INT64:
      result.type = Int64;
      result.value.i64_result = *(UA_Int64 *)value->data;
      break;
    case UA_TYPES_FLOAT:
      result.type = Float32;
      result.value.f32_result = *(UA_Float *)value->data;
     break;
    case UA_TYPES_DOUBLE:
      result.type = Float64;
      result.value.f64_result = *(UA_Double *)value->data;
      break;
    default:
      break;
  }
  return result;//原版result;
}
static bool
template_get_handler(void *impl, const char *devname, const edgex_protocols *protocols,
                     uint32_t nreadings, const edgex_device_commandrequest *requests,
                     edgex_device_commandresult *readings) {
    template_driver *driver = (template_driver *)impl;
        //注意readings的角标不能超过nreadings，nreadings由profile文件确定
        UA_Variant value;
        UA_Variant_init(&value);
        int j=0;
        for(int i=start_node;i<=end_node;i++,j++){
          char* idstring = (char *)malloc(sizeof("49600"));  //分配动态内存
            sprintf(idstring,"%d",i);
             UA_NodeId nodeId= UA_NODEID_STRING(1, idstring);//第一个数据的节点
             UA_StatusCode retval = UA_Client_readValueAttribute(client, nodeId, &value);
           printf("数据%d\n",*(UA_Int32 *)value.data);
            readings[j] = opcua_to_edgex(&value); 
        }
         readings[j].type = Int32;
       readings[j].value.i32_result = start_node/100;
        printf("数据%d\n",readings[j].value.i32_result);
    /* Clean up */
    UA_Variant_clear(&value);
    
    printf("gethandler跳转成功\n");
    return true;
}

//写命令的入口，暂时为空
static bool
template_put_handler(void *impl, const char *devname, const edgex_protocols *protocols,
                     uint32_t nvalues, const edgex_device_commandrequest *requests,
                     const edgex_device_commandresult *values) {
    return true;
}

//断开设备微服务，暂时留空
static bool
template_disconnect(void *impl, edgex_protocols *device) {
    return true;
}

//停止设备微服务，暂时留空
static void
template_stop(void *impl, bool force) {}

//主函数，基本流程
int
main(int argc, char *argv[]) {

  //初始化信息模型节点
  for  (int i  =  0;  i<argc;  i++) {
    if(i==1) 
      start_node=atoi(argv[i]);
    else if(i==2)
      end_node=atoi(argv[i]);
  }
    //启动OPCUA客户端
   client = UA_Client_new();
        UA_ClientConfig_setDefault(UA_Client_getConfig(client));
        UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://localhost:62541");
    if(retval != UA_STATUSCODE_GOOD) {
        UA_Client_delete(client);
        return (int)retval;
    }
    edgex_device_svcparams params = {"OPCUA-RVP", NULL, NULL, NULL, false};
    sigset_t set;
    int sigret;

    template_driver *impl = malloc(sizeof(template_driver));
    memset(impl, 0, sizeof(template_driver));

    //加载configuration配置文件，初始化pramas结构体，形参是指针，实参是地址;形参是引用，实参是变量。结果一样
    if(!edgex_device_service_processparams(&argc, argv, &params)) {
        return 0;
    }

    edgex_error e;
    e.code = 0;

    //设备微服务回调，以下是设备微服务的接口结构体
    edgex_device_callbacks templateImpls = {
        template_init,        /* Initialize */
        template_discover,    /* Discovery */
        template_get_handler, /* Get */
        template_put_handler, /* Put */
        template_disconnect,  /* Disconnect */
        template_stop         /* Stop */
    };

    //初始化新的设备微服务
    edgex_device_service *service =
        edgex_device_service_new(params.svcname, "1.0", impl, templateImpls, &e);
    ERR_CHECK(e);

    printf("device service initial successfully\n");

    // 设置覆盖标志
    edgex_device_service_set_overwrite(service, params.overwrite);
    printf("启动设备微服务\n");
    //启动设备微服务
    /* printf("字节数：%d\n", sizeof(params));
    printf("名称：%s，Profile地址：%s,注册地址%s,写标志%s，配置地址%s\n",
    params.svcname,params.profile,params.regURL,params.overwrite,params.confdir);*/
    unsigned char *p_char = (unsigned char *)&params;
    // (unsigned char *) &params + sizeof(params) 为 结构体p的首地址 加上 p的字节大小
    /* for (; p_char < (unsigned char *) &params + sizeof(params); p_char++) {
         printf("%u ", *p_char);
     }*/
    edgex_device_service_start(service, params.regURL, params.profile, params.confdir,
                               &e);
    ERR_CHECK(e);

    //等待停止
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigwait(&set, &sigret);

    //删除客户端
    UA_Client_delete(client); /* Disconnects the client internally */
    //停止设备微服务
    edgex_device_service_stop(service, true, &e);
    ERR_CHECK(e);

    edgex_device_service_free(service);
    free(impl);
    return 0;
}
