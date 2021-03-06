/*-------------------------------------------------*/
/*            超纬电子STM32系列开发板              */
/*-------------------------------------------------*/
/*                                                 */
/*            onenet_http功能的源文件              */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"    //包含需要的头文件
#include "onenet_http.h"  //包含需要的头文件
#include "stdio.h"        //包含需要的头文件
#include "stdarg.h"		  //包含需要的头文件 
#include "string.h"       //包含需要的头文件
#include "usart1.h"       //包含需要的头文件
#include "dht11.h"  	  //包含需要的头文件

char *ServerIP = "183.230.40.33";          //OneNet服务器 IP地址
int   ServerPort = 80;                     //OneNet服务器 端口号
char RXbuff[2048];                         //接收数据缓冲区
char TXbuff[2048];                         //发送数据缓冲区

/*-------------------------------------------------*/
/*函数名：OneNet服务器 HTTP GET报文                */
/*参  数：device_id： 设备ID                       */
/*返回值：0：正确  其他：错误                      */
/*-------------------------------------------------*/
void OneNet_GET(char *device_id)
{
	char temp[128];
	
	memset(TXbuff,0,2048);   //清空缓冲区
    memset(temp,0,128);      //清空缓冲区                                             
	sprintf(TXbuff,"GET /devices/%s/datapoints HTTP/1.1\r\n",device_id);//构建报文
	sprintf(temp,"api-key:%s\r\n",API_KEY);                             //构建报文
	strcat(TXbuff,temp);                                                //追加报文
	strcat(TXbuff,"Host:api.heclouds.com\r\n\r\n");                     //追加报文
}
/*-------------------------------------------------*/
/*函数名：OneNet服务器 HTTP POST报文               */
/*参  数：device_id： 设备ID                       */
/*返回值：0：正确  其他：错误                      */
/*-------------------------------------------------*/
void OneNet_POST(char *device_id)
{
	char temp[128]; 
	char databuff[128]; 
	char humidity;           //定义一个变量，保存湿度值
	char temperature;        //定义一个变量，保存温度值
	
	memset(TXbuff,0,2048);   //清空缓冲区
    memset(temp,0,128);      //清空缓冲区
    memset(databuff,0,128);  //清空缓冲区
	sprintf(TXbuff,"POST /devices/%s/datapoints?type=3 HTTP/1.1\r\n",device_id);//构建报文
	sprintf(temp,"api-key:%s\r\n",API_KEY);          //构建报文
	strcat(TXbuff,temp);                             //追加报文
	strcat(TXbuff,"Host:api.heclouds.com\r\n");      //追加报文	

    DHT11_Read_Data(&temperature,&humidity);	    //读取温湿度值
	sprintf(databuff,"{\"temp_data\":%d,\"humi_data\":%d}",temperature,humidity);   //构建上报数据

	sprintf(temp,"Content-Length:%d\r\n\r\n",strlen(databuff));  //构建报文
	strcat(TXbuff,temp);                                         //追加报文
	sprintf(temp,"%s\r\n\r\n",databuff);                         //构建报文
	strcat(TXbuff,temp);                                         //追加报文
	u1_printf("%s\r\n",databuff);
}
