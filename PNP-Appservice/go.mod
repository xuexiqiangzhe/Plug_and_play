module appservice

go 1.14

require (
	github.com/edgexfoundry/app-functions-sdk-go v1.3.0
	github.com/edgexfoundry/go-mod-core-contracts v0.1.122
	github.com/fsnotify/fsnotify v1.4.7
	github.com/gin-gonic/gin v1.6.3
	github.com/go-sql-driver/mysql v1.6.0
	github.com/jinzhu/gorm v1.9.16
	github.com/spf13/viper v1.7.1
	gopkg.in/ini.v1 v1.51.0
)

replace github.com/edgexfoundry/app-functions-sdk-go v1.3.0 => ../../app-functions-sdk-go
