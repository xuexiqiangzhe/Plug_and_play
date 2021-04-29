#include "open62541.h"
#include <signal.h>
#include "496model.h"
UA_NodeId objectId;
UA_NodeId newNodeId[8];

//加载496的信息模型
void load496model(UA_Server *server){
    objectId= UA_NODEID_STRING(1, "49600");
    newNodeId[0]= UA_NODEID_STRING(1, "49601");
    newNodeId[1]= UA_NODEID_STRING(1, "49602");
    newNodeId[2]= UA_NODEID_STRING(1, "49603");
    newNodeId[3]= UA_NODEID_STRING(1, "49604");
    newNodeId[4]= UA_NODEID_STRING(1, "49605");
    newNodeId[5]= UA_NODEID_STRING(1, "49606");
    newNodeId[6]= UA_NODEID_STRING(1, "49607");
    newNodeId[7]= UA_NODEID_STRING(1, "49608");

    UA_ObjectAttributes object           = UA_ObjectAttributes_default;
    object.displayName                   = UA_LOCALIZEDTEXT("en-US", "496");
    UA_Server_addObjectNode(server, objectId,
                            UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                            UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                            UA_QUALIFIEDNAME(1, "49600"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
                            object, NULL, NULL);      

    UA_VariableAttributes attr[8] = {UA_VariableAttributes_default,UA_VariableAttributes_default,UA_VariableAttributes_default,
    UA_VariableAttributes_default,UA_VariableAttributes_default,UA_VariableAttributes_default,
    UA_VariableAttributes_default,UA_VariableAttributes_default};
    UA_Int32 myInteger[8] = {49605,49608,496,310,0,0,0,0};
    
    //1～4SDO
    for(int i=0;i<=3;i++){
         UA_Variant_setScalar(&attr[i].value, &myInteger[i], &UA_TYPES[UA_TYPES_INT32]);
         UA_QualifiedName QlfName;
         switch (i)
         {
         case 0:
            attr[i].description = UA_LOCALIZEDTEXT("en-US","过程数据开始节点");
            attr[i].displayName = UA_LOCALIZEDTEXT("en-US","49601");
            QlfName = UA_QUALIFIEDNAME(1, "49601");
             break;
         case 1:
            attr[i].description = UA_LOCALIZEDTEXT("en-US","过程数据结束节点");
            attr[i].displayName = UA_LOCALIZEDTEXT("en-US","49602");
            QlfName = UA_QUALIFIEDNAME(1, "49602");
             break;
         case 2:
            attr[i].description = UA_LOCALIZEDTEXT("en-US","设备id");
            attr[i].displayName = UA_LOCALIZEDTEXT("en-US","49603");
            QlfName = UA_QUALIFIEDNAME(1, "49603");
             break;
         case 3:
            attr[i].description = UA_LOCALIZEDTEXT("en-US","厂家id");
            attr[i].displayName = UA_LOCALIZEDTEXT("en-US","49604");
            QlfName = UA_QUALIFIEDNAME(1, "49604");
             break;
         }
    attr[i].dataType = UA_TYPES[UA_TYPES_INT32].typeId;
    attr[i].accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;                    
    UA_Server_addVariableNode(server, newNodeId[i], objectId,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                              QlfName,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr[i], NULL, NULL);
    }
     
    
    //过程数据1-4
        for(int i=4;i<8;i++){
             char* position = (char *)malloc(sizeof("49600"));  //分配动态内存
            sprintf(position,"4960%d",i+1);
            UA_Variant_setScalar(&attr[i].value, &myInteger[i], &UA_TYPES[UA_TYPES_INT32]);
            char *name = (char *)malloc(500); //分配动态内存
            sprintf(name,"过程数据%d",i-4);
            attr[i].description = UA_LOCALIZEDTEXT("en-US",name);
            attr[i].displayName = UA_LOCALIZEDTEXT("en-US",position);
            attr[i].dataType = UA_TYPES[UA_TYPES_INT32].typeId;
            attr[i].accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;                    
            UA_Server_addVariableNode(server, newNodeId[i], objectId,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                              UA_QUALIFIEDNAME(1, position),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr[i], NULL, NULL);

        }
}

//卸载496的信息模型
void unload496model(UA_Server *server){
    for(int i=0;i<7;i++){
            UA_Server_deleteNode(server, newNodeId[i], UA_TRUE);
            UA_NodeId_clear(&newNodeId[i]);

        }
}
