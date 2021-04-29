package service

//ethercat配置结构体
type EtherCATInfo struct {
	Vendor struct {
		ID int
	}
	Descriptions struct {
		Devices struct {
			Device struct {
				Type  string
				Name  string
				Sm    []string
				RxPdo struct {
					Index string
					Name  string
					Entry []struct {
						Index    string
						SubIndex int
						BitLen   int
						Name     string
						DataType string
					}
				}
				TxPdo struct {
					Index string
					Name  string
					Entry []struct {
						Index    string
						SubIndex int
						BitLen   int
						Name     string
						DataType string
					}
				}
			}
		}
	}
}

//toml配置结构体
type TomlEdgex_Struct struct {
	Service struct {
		Host                string        `json:"Host"`
		Port                int           `json:"Port"`
		ConnectRetries      int           `json:"ConnectRetries"`
		Labels              []interface{} `json:"Labels"`
		OpenMsg             string        `json:"OpenMsg"`
		MaxResultCount      int           `json:"MaxResultCount"`
		Timeout             int           `json:"Timeout"`
		EnableAsyncReadings bool          `json:"EnableAsyncReadings"`
		AsyncBufferSize     int           `json:"AsyncBufferSize"`
	} `json:"Service"`
	Registry struct {
		Host          string `json:"Host"`
		Port          int    `json:"Port"`
		CheckInterval string `json:"CheckInterval"`
		FailLimit     int    `json:"FailLimit"`
		FailWaitTime  int    `json:"FailWaitTime"`
	} `json:"Registry"`
	Clients struct {
		Data struct {
			Name     string `json:"Name"`
			Protocol string `json:"Protocol"`
			Host     string `json:"Host"`
			Port     int    `json:"Port"`
			Timeout  int    `json:"Timeout"`
		} `json:"Data"`
		Metadata struct {
			Name     string `json:"Name"`
			Protocol string `json:"Protocol"`
			Host     string `json:"Host"`
			Port     int    `json:"Port"`
			Timeout  int    `json:"Timeout"`
		} `json:"Metadata"`
		Logging struct {
			Name     string `json:"Name"`
			Protocol string `json:"Protocol"`
			Host     string `json:"Host"`
			Port     int    `json:"Port"`
		} `json:"Logging"`
	} `json:"Clients"`
	Device struct {
		DataTransform  bool   `json:"DataTransform"`
		InitCmd        string `json:"InitCmd"`
		InitCmdArgs    string `json:"InitCmdArgs"`
		MaxCmdOps      int    `json:"MaxCmdOps"`
		MaxCmdValueLen int    `json:"MaxCmdValueLen"`
		RemoveCmd      string `json:"RemoveCmd"`
		RemoveCmdArgs  string `json:"RemoveCmdArgs"`
		ProfilesDir    string `json:"ProfilesDir"`
	} `json:"Device"`
	Logging struct {
		EnableRemote bool   `json:"EnableRemote"`
		File         string `json:"File"`
		Level        string `json:"Level"`
	} `json:"Logging"`
	DeviceList []struct {
		Name        string   `json:"Name"`
		Profile     string   `json:"Profile"`
		Description string   `json:"Description"`
		Labels      []string `json:"Labels"`
		Protocols   struct {
			Other struct {
				Address string `json:"Address"`
				Port    int    `json:"Port"`
			} `json:"Other"`
		} `json:"Protocols"`
		AutoEvents []struct {
			Frequency string `json:"Frequency"`
			OnChange  bool   `json:"OnChange"`
			Resource  string `json:"Resource"`
		} `json:"AutoEvents"`
	} `json:"DeviceList"`
}

//yaml配置结构体
type YamlEdgex_Struct struct {
	Name            string   `yaml:"name"`
	Manufacturer    string   `yaml:"manufacturer"`
	Model           string   `yaml:"model"`
	Labels          []string `yaml:"labels"`
	Description     string   `yaml:"description"`
	DeviceResources []struct {
		Name        string `yaml:"name"`
		Description string `yaml:"description"`
		Properties  struct {
			Value struct {
				Type         string `yaml:"type"`
				ReadWrite    string `yaml:"readWrite"`
				DefaultValue string `yaml:"defaultValue"`
			} `yaml:"value"`
			Units struct {
				Type         string `yaml:"type"`
				ReadWrite    string `yaml:"readWrite"`
				DefaultValue string `yaml:"defaultValue"`
			} `yaml:"units"`
		} `yaml:"properties"`
	} `yaml:"deviceResources"`
	DeviceCommands []struct {
		Name string `yaml:"name"`
		Get  []struct {
			Operation      string `yaml:"operation"`
			DeviceResource string `yaml:"deviceResource"`
		} `yaml:"get"`
		Set []struct {
			Operation      string `yaml:"operation"`
			DeviceResource string `yaml:"deviceResource"`
			Parameter      string `yaml:"parameter"`
		} `yaml:"set,omitempty"`
	} `yaml:"deviceCommands"`
	CoreCommands []struct {
		Name string `yaml:"name"`
		Get  struct {
			Path      string `yaml:"path"`
			Responses []struct {
				Code           string   `yaml:"code"`
				Description    string   `yaml:"description"`
				ExpectedValues []string `yaml:"expectedValues"`
			} `yaml:"responses"`
		} `yaml:"get"`
		Put struct {
			Path           string   `yaml:"path"`
			ParameterNames []string `yaml:"parameterNames"`
			Responses      []struct {
				Code           string        `yaml:"code"`
				Description    string        `yaml:"description"`
				ExpectedValues []interface{} `yaml:"expectedValues,omitempty"`
			} `yaml:"responses"`
		} `yaml:"put,omitempty"`
	} `yaml:"coreCommands"`
}
