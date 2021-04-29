gcc open62541.c device_service_driver.c -I./csdk-1.1.1/include -lrt -lm -lpthread ./csdk-1.1.1/lib/libcsdk.so ./libcbor.so.0.7 ./libmicrohttpd.so.10 -std=gnu11 -o device-simple
./device-simple 38805 38806 388

sudo apt-get install libcurl3
这文件里的动态库都放到/usr/lib下面去


