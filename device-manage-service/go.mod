module device-manage-device

go 1.13

require (
	github.com/BurntSushi/toml v0.3.1
	github.com/beevik/etree v1.1.0
	github.com/edgexfoundry/device-sdk-go v1.0.0
	github.com/gin-contrib/pprof v1.3.0
	github.com/gin-gonic/gin v1.6.2
	github.com/go-sql-driver/mysql v1.6.0
	github.com/robertkrimen/otto v0.0.0-20200922221731-ef014fd054ac
	github.com/swaggo/gin-swagger v1.3.0
	go.uber.org/zap v1.16.0
	gopkg.in/go-playground/validator.v9 v9.29.1 // indirect
	gopkg.in/sourcemap.v1 v1.0.5 // indirect
	gopkg.in/yaml.v2 v2.2.8
)

replace github.com/edgexfoundry/device-sdk-go => ./device-sdk-go
