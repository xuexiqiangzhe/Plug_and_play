package service

import (
	"fmt"
	"github.com/BurntSushi/toml"
	"gopkg.in/yaml.v2"
	"io/ioutil"
	"log"
	"os"
)

//解toml
func Decode_toml() *TomlEdgex_Struct {
	var cp TomlEdgex_Struct
	var path string = "./res/configuration.toml"
	if _, err := toml.DecodeFile(path, &cp); err != nil {
		log.Fatal(err)
	}
	return &cp
}

//生成toml
func Endcode_toml(toml_struct *TomlEdgex_Struct) error {
	fileName := "./device-simple/res/configuration.toml"
	file, err := os.Create(fileName) //打开已经存在的文件or创建文件
	if err != nil {
		fmt.Println(err)
		return err
	}
	defer file.Close()
	encode := toml.NewEncoder(file)
	if err = encode.Encode(toml_struct); err != nil {
		return err
	}
	return nil
}

//解yaml
func (y *YamlEdgex_Struct) Yaml_Decode(deviceid int) *YamlEdgex_Struct {
	//应该是 绝对地址
	var path string = fmt.Sprintf("/res/profile_IO_Link_Device%d.yaml", deviceid)
	str, _ := os.Getwd()
	str += path
	if _, err := ioutil.ReadFile(str); err != nil {
		log.Fatal(err)
	} //读取地址路径文件
	yamlFile, _ := ioutil.ReadFile(str) //不接受返回值err
	yaml.Unmarshal(yamlFile, &y)        //Go的指针的地址要加取址符号
	return y
}

//生成yaml
func (y *YamlEdgex_Struct) Yaml_Encode() error {
	fileName := "./device-simple/res/Simple-Driver_generated.yaml"
	file, err := os.Create(fileName) //打开已经存在的文件or创建文件
	if err != nil {
		fmt.Println(err)
		return err
	}
	defer file.Close()
	encode := yaml.NewEncoder(file)
	if err = encode.Encode(&y); err != nil {
		return err
	}
	return nil
}
