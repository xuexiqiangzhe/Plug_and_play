window.onload = function() {//到根了把
    //加载初始窗口
    device_manager();
    //定时任务1s一次获取设备
    var id1 = setInterval(finddevice,1000);
   //定时任务0.5s一次获取过程数据
    var id2 = setInterval(processdata,500);
}
var devices
function finddevice () {
    var cmdUrl="http://127.0.0.1:8000/api/v1/device";
    var obj;
    //发get请求
    $.ajax(
        {
            url:cmdUrl,
            method:'get',
            dataType:"json",
            success: function(data){
              // console.log(JSON.stringify(data));
                obj = JSON.parse(JSON.stringify(data));
                for(var i=0;i<obj.data.length;i++){
                     if(obj.data[i]=="IO_Link_Device388"&&$("#"+"input_devicename_"+obj.data[i].toString()+"_1").val()!="电容传感器"){
                          addrow(obj.data[i].toString()+"_1","电容传感器","62541:38801","过程数据1",30);
                          addrow(obj.data[i].toString()+"_2","————————","————————","过程数据2",30);
                      }else if(obj.data[i]=="IO_Link_Device496"&&$("#"+"input_devicename_"+obj.data[i].toString()+"_1").val()!="编码器"){
                          addrow(obj.data[i].toString()+"_1","编码器","62541:49601","过程数据1",30);
                          addrow(obj.data[i].toString()+"_2","————————","————————","过程数据2",30);
                          addrow(obj.data[i].toString()+"_3","————————","————————","过程数据3",30);
                          addrow(obj.data[i].toString()+"_4","————————","————————","过程数据4",30);
                      }
                }
            },
            error:function(err){
                // console.log("GET fail");
            }
        });
}

function processdata(){
    var cmdUrl="http://127.0.0.1:8000/api/v1/PDO";
    var obj;
    //发post请求
    $.ajax(
        {
            url:cmdUrl,
            method:'get',
            success: function(data){
                obj = JSON.parse(JSON.stringify(data));

                for(var i=0;i<obj.data.length;i++){
                    var sing_edgex_message=JSON.parse(obj.data[i]);
                    console.log(sing_edgex_message)
                    if(sing_edgex_message.device=="IO_Link_Device388"){
                        //console.log(parseInt(sing_edgex_message.readings[1].value))
                        var newdatavalue="input_datavalue_"+sing_edgex_message.device.toString();
                        var newoutdatavalue="output_datavalue_"+sing_edgex_message.device.toString();
                       $("#"+newdatavalue+"_1").val(parseInt(sing_edgex_message.readings[0].value));
                        $("#"+newoutdatavalue+"_1").val(parseInt(sing_edgex_message.readings[0].value));
                       $("#"+newdatavalue+"_2").val(parseInt(sing_edgex_message.readings[1].value));
                        $("#"+newoutdatavalue+"_2").val(parseInt(sing_edgex_message.readings[1].value));
                    }else if(sing_edgex_message.device=="IO_Link_Device496"){
                        var newdatavalue="input_datavalue_"+sing_edgex_message.device.toString();
                        var newoutdatavalue="output_datavalue_"+sing_edgex_message.device.toString();
                        $("#"+newdatavalue+"_1").val(parseInt(sing_edgex_message.readings[0].value));
                        $("#"+newoutdatavalue+"_1").val(parseInt(sing_edgex_message.readings[0].value));
                        $("#"+newdatavalue+"_2").val(parseInt(sing_edgex_message.readings[1].value));
                        $("#"+newoutdatavalue+"_2").val(parseInt(sing_edgex_message.readings[1].value));
                       $("#"+newdatavalue+"_3").val(parseInt(sing_edgex_message.readings[2].value));
                        $("#"+newoutdatavalue+"_3").val(parseInt(sing_edgex_message.readings[2].value));
                        $("#"+newdatavalue+"_4").val(parseInt(sing_edgex_message.readings[3].value));
                        $("#"+newoutdatavalue+"_4").val(parseInt(sing_edgex_message.readings[3].value));
                    }
                }
            },
            error:function(err){
                // console.log("GET fail");
            }
        });


}
function device_manager() {
    console.log("开始加载设备管理窗口");
    var rowData = "<tr>";
    rowData += "<td>" +  1 + "</td>";
    rowData += "<td>" ;
    rowData += '设备名'+'&nbsp;<input type="text" class="form-control" name="targetresource" style="width:200px;height:30px;position:relative;top:10px;display:inline;">&nbsp;'
    rowData += "</td>";
    rowData += "<td>" ;
    rowData += '连接地址'+'&nbsp;<input type="text" class="form-control" name="threshold" style="width:200px;height:30px;position:relative;top:10px;display:inline;">&nbsp;'
    rowData += "</td>";
    rowData += "<td>" ;
    rowData += '数据名'+'&nbsp;<input type="text" class="form-control" name="targetdevicename" style="width:200px;height:30px;position:relative;top:10px;display:inline;">&nbsp;'
    rowData += "</td>";
    rowData += "<td>" ;
    rowData += '数据值'+

        '<input type="range" id="source_value"'
          +         'style="width:150px;height:30px;position:relative;top:5px;display:inline;">'
           +     '='
            +   ' <output type="text" id="x"></output>';
    rowData += "</td>";
    rowData += "<td>" ;
    rowData +='<button type="button" class="btn" style="background-color: #7ED321;position:relative;top:10px;width: 76px;height: 36px;color: #000000" onclick="kill_device_Function()">断开</button>'
    rowData += "</td>";
    rowData += "</tr>";
    for (var i=0;i<1;i++)
    {
        $("table").append(rowData);//为table元素添加一行
    }
    console.log("加载设备管理窗口完毕");
}

    // 7.删除方法
    function delTr(obj) {
        // 获取table节点
        var table = obj.parentNode.parentNode.parentNode;
        // 获取te节点
        var tr = obj.parentNode.parentNode;
        // 删除（并返回）当前节点的指定子节点。
        table.removeChild(tr);
    }

function addrow(id,name,addr,dataname,datavalue){
    var rowData = "<tr>";
    rowData += "<td>" +  id + "</td>";
    rowData += "<td>" ;
    rowData += '设备名'+'&nbsp;<input type="text" class="form-control" id="input_devicename" style="width:200px;height:30px;position:relative;top:10px;display:inline;">&nbsp;'
    rowData += "</td>";
    rowData += "<td>" ;
    rowData += '连接地址'+'&nbsp;<input type="text" class="form-control" id="input_addr" style="width:200px;height:30px;position:relative;top:10px;display:inline;">&nbsp;'
    rowData += "</td>";
    rowData += "<td>" ;
    rowData += '数据名'+'&nbsp;<input type="text" class="form-control" id="input_dataname" style="width:200px;height:30px;position:relative;top:10px;display:inline;">&nbsp;'
    rowData += "</td>";
    rowData += "<td>" ;
    rowData += '数据值'+

        '<input type="range" id="input_datavalue" max=255 min=0'
        +         'style="width:200px;height:50px;position:relative;top:5px;display:inline;">'
        +     '='
        +   ' <output type="text" id="output_datavalue"></output>';
    rowData += "</td>";
    rowData += "<td>" ;
    rowData +='<button type="button" class="btn" style="background-color: #7ED321;position:relative;top:10px;width: 76px;height: 36px;color: #000000" onclick="kill_device_Function()">断开</button>'
    rowData += "</td>";
    rowData += "</tr>";
    for (var i=0;i<1;i++)
    {
        $("table").append(rowData);//为table元素添加一行
    }

    //更改inputid
    var newdevicename="input_devicename_"+id;
    var newaddr="input_addr_"+id;
    var newdataname="input_dataname_"+id;
    var newdatavalue="input_datavalue_"+id;
    var newoutdatavalue="output_datavalue_"+id;
    document.getElementById("input_devicename").id=newdevicename;
    document.getElementById("input_addr").id=newaddr;
    document.getElementById("input_dataname").id=newdataname;
    document.getElementById("input_datavalue").id=newdatavalue;
    document.getElementById("output_datavalue").id=newoutdatavalue;
    //input output 赋值
    $("#"+newdevicename).val(name);
    $("#"+newaddr).val(addr);
    $("#"+newdataname).val(dataname);
    $("#"+newdatavalue).val(datavalue);
    $("#"+newoutdatavalue).val(datavalue);

}
//不用了
/*
function addFunction() {//人工格式化构建表格

    // 2.1 获取输入对象
    var idObject = document.getElementById("id");
    var nameObject = document.getElementById("name");
    var addrObject = document.getElementById("addr");
    var datanameObject = document.getElementById("dataname");
    var datavalueObject = document.getElementById("datavalue");

    // 2.2 获取输入值
    var id = idObject.value;
    var name = nameObject.value;
    var addr = addrObject.value;
    var dataname = datanameObject.value;
    var datavalue = datavalueObject.value;
    console.log(id,name,addr,dataname,datavalue)
//3.创建单元格，赋值单元格的标签体
// id 的单元格
    var td_id = document.createElement("td");              // 创建单元格
    var text_id = document.createTextNode(id);             // 赋值给单元格的标签体
    td_id.appendChild(text_id);
    // name 的单元格
    var td_name = document.createElement("td");
    var text_name = document.createTextNode(name);
    td_name.appendChild(text_name);
//addr 的单元格
    var td_addr = document.createElement("td");
    var text_addr = document.createTextNode(addr);
    td_addr.appendChild(text_addr);
    //dataname 的单元格
    var td_dataname = document.createElement("td");
    var text_dataname = document.createTextNode(dataname);
    td_dataname.appendChild(text_dataname);
    //datavalue 的单元格
    var td_datavalue = document.createElement("td");
    var text_datavalue = document.createTextNode(datavalue);
    td_datavalue.appendChild(text_datavalue);

// a标签的单元格
    var td_a = document.createElement("td");
    var ele_a = document.createElement("a");
    ele_a.setAttribute("href", "javascript:void(0);");
    ele_a.setAttribute("onclick", "delTr(this);");
    var text_a = document.createTextNode("删除");
    ele_a.appendChild(text_a);                         // 为a标签写入文本内容："删除"
    td_a.appendChild(ele_a);                           // 为td标签添加子标签（a标签）

    // 4.创建表格行
    var tr = document.createElement("tr");
    // 5.添加单元格到表格行中
    tr.appendChild(td_id);
    tr.appendChild(td_name);
    tr.appendChild(td_addr);
    tr.appendChild(td_dataname);
    tr.appendChild(td_datavalue);
    tr.appendChild(td_a);

    // 6.获取table
    var table = document.getElementsByTagName("table")[0];
    table.appendChild(tr);
}*/
