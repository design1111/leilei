/*-----------------------------------------------------*/
/*              超纬电子STM32系列开发板                */
/*-----------------------------------------------------*/
/*                     程序结构                        */
/*-----------------------------------------------------*/
/*USER     ：包含程序的main函数，是整个程序的入口      */
/*HARDWARE ：包含开发板各种功能外设的驱动程序          */
/*CORE     ：包含STM32的核心程序，官方提供，我们不修改 */
/*STLIB    ：官方提供的库文件，我们不修改              */
/*-----------------------------------------------------*/
/*                                                     */
/*           程序main函数，入口函数源文件              */
/*                                                     */
/*-----------------------------------------------------*/

#include "stm32f10x.h"  //包含需要的头文件
#include "main.h"       //包含需要的头文件
#include "delay.h"      //包含需要的头文件
#include "usart1.h"     //包含需要的头文件
#include "usart2.h"     //包含需要的头文件
#include "timer4.h"     //包含需要的头文件
#include "wifi.h"	    //包含需要的头文件
#include "key.h"        //包含需要的头文件

/*-----------------------------------------------------------------------------*/
/*                              根据自己的网络环境修改                         */
/*-----------------------------------------------------------------------------*/
char *ServerIP = "192.168.10.2";           //存放服务器IP或是域名
int  ServerPort = 5050;                    //存放服务器的端口号区
/*-----------------------------------------------------------------------------*/

char  Data_buff[2048];     //数据缓冲区

int main(void) 
{	
	int time;
	
	Delay_Init();                   //延时功能初始化              
	Usart1_Init(9600);              //串口1功能初始化，波特率9600
	Usart2_Init(115200);            //串口2功能初始化，波特率115200	
	TIM4_Init(300,7200);            //TIM4初始化，定时时间 300*7200*1000/72000000 = 30ms
	KEY_Init();                     //按键初始化
	WiFi_ResetIO_Init();            //初始化WiFi的复位IO
    while(WiFi_ConnectServer_AP()){  //循环，初始化，连接服务器，直到成功
		Delay_Ms(2000);              //延时
	}      
	WiFi_RxCounter=0;                          //WiFi接收数据量变量清零                        
	memset(WiFi_RX_BUF,0,WiFi_RXBUFF_SIZE);    //清空WiFi接收缓冲区               
	Connect_flag = 1;                          //Connect_flag=1,表示连接上服务器	
	
	while(1){                      		
		if(Usart2_RxCompleted==1){		                          //如果Usart2_RxCompleted等于1，表示接收数据完成
			Usart2_RxCompleted = 0;                               //清除标志位
			u1_printf("服务器发来%d字节数据\r\n",(Data_buff[0]*256+Data_buff[1])); //串口输出信息
			u1_printf("数据:%s\r\n",&Data_buff[2]);               //串口输出信息   
			WiFi_printf("您发送的数据数:%s\r\n",&Data_buff[2]);	  //把接收到的数据，返回给服务器
	    }
		if(time>=1000){                                           //当time大于等于1000的时候，大概经过1s的时间
			time=0;                                               //清除time计数
			WiFi_printf("我是客户端，请发送数据!\r\n");           //给服务器发送数据
		}
		Delay_Ms(1);  //延时1ms
		time++;       //time计数器+1
	} 
}
