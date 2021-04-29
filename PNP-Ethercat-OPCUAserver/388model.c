#include "open62541.h"
#include <signal.h>
#include "388model.h"
UA_NodeId objectId;
UA_NodeId newNodeId1;//过程数据开始节点
UA_NodeId newNodeId2;//过程数据结束节点
UA_NodeId newNodeId3;//deviceid节点
UA_NodeId newNodeId4;//vendorid节点
UA_NodeId newNodeId5;//过程数据1
UA_NodeId newNodeId6;//过程数据2

//加载388的信息模型
void load388model(UA_Server *server) {
    objectId= UA_NODEID_STRING(1, "38800");
    newNodeId1= UA_NODEID_STRING(1, "38801");
    newNodeId2= UA_NODEID_STRING(1, "38802");
    newNodeId3= UA_NODEID_STRING(1, "38803");
    newNodeId4= UA_NODEID_STRING(1, "38804");
    newNodeId5= UA_NODEID_STRING(1, "38805");
    newNodeId6= UA_NODEID_STRING(1, "38806");

    UA_ObjectAttributes object           = UA_ObjectAttributes_default;
    object.displayName                   = UA_LOCALIZEDTEXT("en-US", "388");
    UA_Server_addObjectNode(server, objectId,
                            UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                            UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                            UA_QUALIFIEDNAME(1, "38800"), UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
                            object, NULL, NULL);      

    UA_VariableAttributes attr1 = UA_VariableAttributes_default;
    UA_Int32 myInteger1 = 38805;
    UA_Variant_setScalar(&attr1.value, &myInteger1, &UA_TYPES[UA_TYPES_INT32]);
    attr1.description = UA_LOCALIZEDTEXT("en-US","过程数据开始节点");
    attr1.displayName = UA_LOCALIZEDTEXT("en-US","38801");
    attr1.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
    attr1.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;                    
    UA_Server_addVariableNode(server, newNodeId1, objectId,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                              UA_QUALIFIEDNAME(1, "38801"),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr1, NULL, NULL);

    UA_VariableAttributes attr2 = UA_VariableAttributes_default;
    UA_Int32 myInteger2 = 38806;
    UA_Variant_setScalar(&attr2.value, &myInteger2, &UA_TYPES[UA_TYPES_INT32]);
    attr2.description = UA_LOCALIZEDTEXT("en-US","过程数据结束节点");
    attr2.displayName = UA_LOCALIZEDTEXT("en-US","38802");
    attr2.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
    attr2.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;                    
    UA_Server_addVariableNode(server, newNodeId2, objectId,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                              UA_QUALIFIEDNAME(1, "38802"),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr2, NULL, NULL);

     UA_VariableAttributes attr3 = UA_VariableAttributes_default;
    UA_Int32 myInteger3 = 388;
    UA_Variant_setScalar(&attr3.value, &myInteger3, &UA_TYPES[UA_TYPES_INT32]);
    attr3.description = UA_LOCALIZEDTEXT("en-US","设备id");
    attr3.displayName = UA_LOCALIZEDTEXT("en-US","38803");
    attr3.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
    attr3.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;                    
    UA_Server_addVariableNode(server, newNodeId3, objectId,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                              UA_QUALIFIEDNAME(1, "38803"),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr3, NULL, NULL);

     UA_VariableAttributes attr4 = UA_VariableAttributes_default;
    UA_Int32 myInteger4 = 310;
    UA_Variant_setScalar(&attr4.value, &myInteger4, &UA_TYPES[UA_TYPES_INT32]);
    attr4.description = UA_LOCALIZEDTEXT("en-US","厂家id");
    attr4.displayName = UA_LOCALIZEDTEXT("en-US","38804");
    attr4.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
    attr4.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;                    
    UA_Server_addVariableNode(server, newNodeId4, objectId,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                              UA_QUALIFIEDNAME(1, "38804"),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr4, NULL, NULL);

     UA_VariableAttributes attr5 = UA_VariableAttributes_default;
    UA_Int32 myInteger5 = 0;
    UA_Variant_setScalar(&attr5.value, &myInteger5, &UA_TYPES[UA_TYPES_INT32]);
    attr5.description = UA_LOCALIZEDTEXT("en-US","过程数据1");
    attr5.displayName = UA_LOCALIZEDTEXT("en-US","38805");
    attr5.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
    attr5.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;                    
    UA_Server_addVariableNode(server, newNodeId5, objectId,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                              UA_QUALIFIEDNAME(1, "38805"),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr5, NULL, NULL);
    
    UA_VariableAttributes attr6 = UA_VariableAttributes_default;
    UA_Int32 myInteger6 = 0;
    UA_Variant_setScalar(&attr6.value, &myInteger6, &UA_TYPES[UA_TYPES_INT32]);
    attr6.description = UA_LOCALIZEDTEXT("en-US","过程数据1");
    attr6.displayName = UA_LOCALIZEDTEXT("en-US","38806");
    attr6.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
    attr6.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;                    
    UA_Server_addVariableNode(server, newNodeId6, objectId,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                              UA_QUALIFIEDNAME(1, "38806"),
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), attr6, NULL, NULL);
}

//卸载388的信息模型
void unload388model(UA_Server *server) {
    UA_Server_deleteNode(server, newNodeId1, UA_TRUE);
    UA_NodeId_clear(&newNodeId1);
    UA_Server_deleteNode(server, newNodeId2, UA_TRUE);
    UA_NodeId_clear(&newNodeId2);
    UA_Server_deleteNode(server, newNodeId3, UA_TRUE);
    UA_NodeId_clear(&newNodeId3);
    UA_Server_deleteNode(server, newNodeId4, UA_TRUE);
    UA_NodeId_clear(&newNodeId4);
    UA_Server_deleteNode(server, newNodeId5, UA_TRUE);
    UA_NodeId_clear(&newNodeId5);
    UA_Server_deleteNode(server, newNodeId6, UA_TRUE);
    UA_NodeId_clear(&newNodeId6);
    UA_Server_deleteNode(server, objectId, UA_TRUE);
    UA_NodeId_clear(&objectId);
}
