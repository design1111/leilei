/*-----------------------------------------------------*/
/*              超纬电子STM32系列开发板                */
/*-----------------------------------------------------*/
/*                     程序结构                        */
/*-----------------------------------------------------*/
/*USER     ：包含程序的main函数，是整个程序的入口      */
/*HARDWARE ：包含开发板各种功能外设的驱动程序          */
/*CORE     ：包含STM32的核心程序，官方提供，我们不修改 */
/*STLIB    ：官方提供的库文件，我们不修改              */
/*HMAC     ：网上关于哈希加密算法程序                  */
/*-----------------------------------------------------*/
/*                                                     */
/*           程序main函数，入口函数源文件              */
/*                                                     */
/*-----------------------------------------------------*/

#include "stm32f10x.h"  //包含需要的头文件
#include "main.h"       //包含需要的头文件
#include "delay.h"      //包含需要的头文件
#include "usart1.h"     //包含需要的头文件
#include "usart3.h"     //包含需要的头文件
#include "mqtt.h"       //包含需要的头文件
#include "timer4.h"     //包含需要的头文件
#include "timer6.h"     //包含需要的头文件
#include "sim800c.h"	//包含需要的头文件
#include "iic.h"        //包含需要的头文件
#include "led.h"        //包含需要的头文件

int main(void) 
{		
	Delay_Init();                   //延时功能初始化              
	Usart1_Init(9600);              //串口1功能初始化，波特率9600
	Usart3_Init(115200);            //串口2功能初始化，波特率115200	
	TIM4_Init(300,7200);            //TIM4初始化，定时时间 300*7200*1000/72000000 = 30ms	
	LED_Init();	                    //初始化LED指示灯	
	IIC_Init();	                    //初始化IIC接口
	AliIoT_Parameter_Init();	    //初始化连接阿里云IoT平台MQTT服务器的参数	
	while(1)                        //主循环
	{		
		/*--------------------------------------------------------------------*/
		/*   Connect_flag=1同服务器建立了连接,我们可以发布数据和接收推送了    */
		/*--------------------------------------------------------------------*/
		if(Connect_flag==1){     
			/*-------------------------------------------------------------*/
			/*                     处理发送缓冲区数据                      */
			/*-------------------------------------------------------------*/
				if(MQTT_TxDataOutPtr != MQTT_TxDataInPtr){                //if成立的话，说明发送缓冲区有数据了
				//3种情况可进入if
				//第1种：0x10 连接报文
				//第2种：0x82 订阅报文，且ConnectPack_flag置位，表示连接报文成功
				//第3种：SubcribePack_flag置位，说明连接和订阅均成功，其他报文可发
				if((MQTT_TxDataOutPtr[2]==0x10)||((MQTT_TxDataOutPtr[2]==0x82)&&(ConnectPack_flag==1))||(SubcribePack_flag==1)){    
					u1_printf("发送数据:0x%x\r\n",MQTT_TxDataOutPtr[2]);  //串口提示信息
					MQTT_TxData(MQTT_TxDataOutPtr);                       //发送数据
					MQTT_TxDataOutPtr += TBUFF_UNIT;                      //指针下移
					if(MQTT_TxDataOutPtr==MQTT_TxDataEndPtr)              //如果指针到缓冲区尾部了
						MQTT_TxDataOutPtr = MQTT_TxDataBuf[0];            //指针归位到缓冲区开头
				}  				
			}//处理发送缓冲区数据的else if分支结尾
			
			/*-------------------------------------------------------------*/
			/*                     处理接收缓冲区数据                      */
			/*-------------------------------------------------------------*/
			if(MQTT_RxDataOutPtr != MQTT_RxDataInPtr){  //if成立的话，说明接收缓冲区有数据了														
				u1_printf("接收到数据:");
				/*-----------------------------------------------------*/
				/*                    处理CONNACK报文                  */
				/*-----------------------------------------------------*/				
				//if判断，如果第一个字节是0x20，表示收到的是CONNACK报文
				//接着我们要判断第4个字节，看看CONNECT报文是否成功
				if(MQTT_RxDataOutPtr[2]==0x20){             			
				    switch(MQTT_RxDataOutPtr[5]){					
						case 0x00 : u1_printf("CONNECT报文成功\r\n");                            //串口输出信息	
								    ConnectPack_flag = 1;                                        //CONNECT报文成功，订阅报文可发
									break;                                                       //跳出分支case 0x00                                              
						case 0x01 : u1_printf("连接已拒绝，不支持的协议版本，准备重启\r\n");     //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接
									break;                                                       //跳出分支case 0x01   
						case 0x02 : u1_printf("连接已拒绝，不合格的客户端标识符，准备重启\r\n"); //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接
									break;                                                       //跳出分支case 0x02 
						case 0x03 : u1_printf("连接已拒绝，服务端不可用，准备重启\r\n");         //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接
									break;                                                       //跳出分支case 0x03
						case 0x04 : u1_printf("连接已拒绝，无效的用户名或密码，准备重启\r\n");   //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接						
									break;                                                       //跳出分支case 0x04
						case 0x05 : u1_printf("连接已拒绝，未授权，准备重启\r\n");               //串口输出信息
									Connect_flag = 0;                                            //Connect_flag置零，重启连接						
									break;                                                       //跳出分支case 0x05 		
						default   : u1_printf("连接已拒绝，未知状态，准备重启\r\n");             //串口输出信息 
									Connect_flag = 0;                                            //Connect_flag置零，重启连接					
									break;                                                       //跳出分支case default 								
					}				
				}			
				//if判断，第一个字节是0x90，表示收到的是SUBACK报文
				//接着我们要判断订阅回复，看看是不是成功
				else if(MQTT_RxDataOutPtr[2]==0x90){ 
						switch(MQTT_RxDataOutPtr[6]){					
						case 0x00 :
						case 0x01 : u1_printf("订阅成功\r\n");            //串口输出信息
							        SubcribePack_flag = 1;                //SubcribePack_flag置1，表示订阅报文成功，其他报文可发送
									Ping_flag = 0;                        //Ping_flag清零
   								    TIM6_ENABLE_30S();                    //启动30s的PING定时器
									LED_State();                          //判断4个开关状态，并发布给服务器
									break;                                //跳出分支                                             
						default   : u1_printf("订阅失败，准备重启\r\n");  //串口输出信息 
									Connect_flag = 0;                     //Connect_flag置零，重启连接
									break;                                //跳出分支 								
					}					
				}
				//if判断，第一个字节是0xD0，表示收到的是PINGRESP报文
				else if(MQTT_RxDataOutPtr[2]==0xD0){ 
					u1_printf("PING报文回复\r\n"); 		  //串口输出信息 
					if(Ping_flag==1){                     //如果Ping_flag=1，表示第一次发送
						 Ping_flag = 0;    				  //要清除Ping_flag标志
					}else if(Ping_flag>1){ 				  //如果Ping_flag>1，表示是多次发送了，而且是2s间隔的快速发送
						Ping_flag = 0;     				  //要清除Ping_flag标志
						TIM6_ENABLE_30S(); 				  //PING定时器重回30s的时间
					}				
				}	
				//if判断，如果第一个字节是0x30，表示收到的是服务器发来的推送数据
				//我们要提取控制命令
				else if((MQTT_RxDataOutPtr[2]==0x30)){ 
					u1_printf("服务器等级0推送\r\n"); 		   //串口输出信息 
					MQTT_DealPushdata_Qs0(MQTT_RxDataOutPtr);  //处理等级0推送数据
				}				
				//if判断，如果一共接收了10个字节，第一个字节是0x0D，有可能收到了 CLOSED 表示连接断开
				//我们进入else if，接着判断
				else if((MQTT_RxDataOutPtr[1]==10)&&(MQTT_RxDataOutPtr[2]==0x0D)){
					MQTT_RxDataOutPtr[12] = '\0';                   //加入字符串结束符
					if(strstr(SIM800C_RX_BUF,"CLOSED")){            //如果搜索到了CLOSED，表示连接断开了						
						Connect_flag = 0;                           //连接状态置0，表示断开，需要重连服务器
					}			
				}
				//if判断，如果一共接收了15个字节，第一个字节是0x0D，有可能收到了 +PDP: DEACT 表示GPRS断开
				//我们进入else if，接着判断
				else if((MQTT_RxDataOutPtr[1]==15)&&(MQTT_RxDataOutPtr[1]==0x0D)){
					MQTT_RxDataOutPtr[17] = '\0';                   //加入字符串结束符
					u1_printf("GPRS断开\r\n"); 		                //串口输出信息
					if(strstr(SIM800C_RX_BUF,"+PDP: DEACT")){       //如果搜索到+PDP: DEACT，表示GPRS断开了						
						Connect_flag = 0;                           //连接状态置0，表示断开，需要重连服务器
					}			
				}	
				
				MQTT_RxDataOutPtr +=RBUFF_UNIT;                     //指针下移
				if(MQTT_RxDataOutPtr==MQTT_RxDataEndPtr)            //如果指针到缓冲区尾部了
					MQTT_RxDataOutPtr = MQTT_RxDataBuf[0];          //指针归位到缓冲区开头                        
			}//处理接收缓冲区数据的else if分支结尾
			
			/*-------------------------------------------------------------*/
			/*                     处理命令缓冲区数据                      */
			/*-------------------------------------------------------------*/
			if(MQTT_CMDOutPtr != MQTT_CMDInPtr){                             //if成立的话，说明命令缓冲区有数据了			       
				u1_printf("命令:%s\r\n",&MQTT_CMDOutPtr[2]);                 //串口输出信息
				if(strstr((char *)MQTT_CMDOutPtr+2,"\"params\":{\"PowerSwitch\":1}")){	       //如果搜索到"params":{"PowerSwitch":1}说明服务器下发打开开关1	
					LED1_ON;                                                                   //打开LED1
					LED1_State();                                                              //判断开关状态，并发布给服务器  
				}else if(strstr((char *)MQTT_CMDOutPtr+2,"\"params\":{\"PowerSwitch\":0}")){   //如果搜索到"params":{"PowerSwitch":0}说明服务器下发关闭开关1
					LED1_OFF;                                                                  //关闭LED1
					LED1_State();                                                              //判断开关状态，并发布给服务器  
				}else if(strstr((char *)MQTT_CMDOutPtr+2,"\"params\":{\"PowerSwitch_2\":1}")){ //如果搜索到"params":{"PowerSwitch_2":1}说明服务器下发打开开关2	
					LED2_ON;                                                                   //打开LED2
					LED2_State();                                                              //判断开关状态，并发布给服务器  
				}else if(strstr((char *)MQTT_CMDOutPtr+2,"\"params\":{\"PowerSwitch_2\":0}")){ //如果搜索到"params":{"PowerSwitch_2":0}说明服务器下发关闭开关2
					LED2_OFF;                                                                  //关闭LED2
					LED2_State();                                                              //判断开关状态，并发布给服务器  
				}else if(strstr((char *)MQTT_CMDOutPtr+2,"\"params\":{\"PowerSwitch_3\":1}")){ //如果搜索到"params":{"PowerSwitch_3":1}说明服务器下发打开开关3	
					LED3_ON;                                                                   //打开LED3
					LED3_State();                                                              //判断开关状态，并发布给服务器  
				}else if(strstr((char *)MQTT_CMDOutPtr+2,"\"params\":{\"PowerSwitch_3\":0}")){ //如果搜索到"params":{"PowerSwitch_3":0}说明服务器下发关闭开关3
					LED3_OFF;                                                                  //关闭LED3
					LED3_State();                                                              //判断开关状态，并发布给服务器  
				}else if(strstr((char *)MQTT_CMDOutPtr+2,"\"params\":{\"PowerSwitch_4\":1}")){ //如果搜索到"params":{"PowerSwitch_4":1}说明服务器下发打开开关4	
					LED4_ON;                                                                   //打开LED4
					LED4_State();                                                              //判断开关状态，并发布给服务器  
				}else if(strstr((char *)MQTT_CMDOutPtr+2,"\"params\":{\"PowerSwitch_4\":0}")){ //如果搜索到"params":{"PowerSwitch_4":0}说明服务器下发关闭开关4
					LED4_OFF;                                                                  //关闭LED4
					LED4_State();                                                              //判断开关状态，并发布给服务器  
				}
				MQTT_CMDOutPtr += CBUFF_UNIT;                             	 //指针下移
				if(MQTT_CMDOutPtr==MQTT_CMDEndPtr)           	             //如果指针到缓冲区尾部了
					MQTT_CMDOutPtr = MQTT_CMDBuf[0];          	             //指针归位到缓冲区开头				
			}//处理命令缓冲区数据的else if分支结尾		
		}//Connect_flag=1的if分支的结尾
		
		/*--------------------------------------------------------------------*/
		/*      Connect_flag=0同服务器断开了连接,我们要重启连接服务器         */
		/*--------------------------------------------------------------------*/
		else{ 
			u1_printf("需要连接服务器\r\n");                 //串口输出信息
			TIM_Cmd(TIM4,DISABLE);                           //关闭TIM4 
			TIM_Cmd(TIM6,DISABLE);                           //关闭TIM6  
			SIM800C_RxCounter=0;                             //800C接收数据量变量清零                        
			memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);    //清空800C接收缓冲区 
			if(SIM800C_Connect_IoTServer()==0){   			 //如果800C连接云服务器函数返回0，表示正确，进入if
				u1_printf("建立TCP连接成功\r\n");            //串口输出信息
				Connect_flag = 1;                            //Connect_flag置1，表示连接成功	
				SIM800C_RxCounter=0;                         //800C接收数据量变量清零                        
				memset(SIM800C_RX_BUF,0,SIM800C_RXBUFF_SIZE);//清空800C接收缓冲区 
				MQTT_Buff_Init();                            //初始化缓冲区                    
			}				
		}
	}
}
/*-------------------------------------------------*/
/*函数名：判断开关1状态，并发布给服务器            */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void LED1_State(void)
{
	char temp[TBUFF_UNIT];                   //定义一个临时缓冲区
		
	if(LED1_IN_STA) sprintf(temp,"{\"method\":\"thing.event.property.post\",\"id\":\"203302322\",\"params\":{\"PowerSwitch\":0},\"version\":\"1.0.0\"}");  //如果LED1是高电平，说明是熄灭状态，需要回复关闭状态给服务器
	else            sprintf(temp,"{\"method\":\"thing.event.property.post\",\"id\":\"203302322\",\"params\":{\"PowerSwitch\":1},\"version\":\"1.0.0\"}");  //如果LED1是低电平，说明是点亮状态，需要回复打开状态给服务器
	MQTT_PublishQs0(P_TOPIC_NAME,temp,strlen(temp));   //添加数据，发布给服务器		
}
/*-------------------------------------------------*/
/*函数名：判断开关2状态，并发布给服务器            */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void LED2_State(void)
{
	char temp[TBUFF_UNIT];                   //定义一个临时缓冲区
		
	if(LED2_IN_STA) sprintf(temp,"{\"method\":\"thing.event.property.post\",\"id\":\"203302322\",\"params\":{\"PowerSwitch_2\":0},\"version\":\"1.0.0\"}");  //如果LED2是高电平，说明是熄灭状态，需要回复关闭状态给服务器
	else            sprintf(temp,"{\"method\":\"thing.event.property.post\",\"id\":\"203302322\",\"params\":{\"PowerSwitch_2\":1},\"version\":\"1.0.0\"}");  //如果LED2是低电平，说明是点亮状态，需要回复打开状态给服务器
	MQTT_PublishQs0(P_TOPIC_NAME,temp,strlen(temp));   //添加数据，发布给服务器		
}
/*-------------------------------------------------*/
/*函数名：判断开关3状态，并发布给服务器            */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void LED3_State(void)
{
	char temp[TBUFF_UNIT];                   //定义一个临时缓冲区
		
	if(LED3_IN_STA) sprintf(temp,"{\"method\":\"thing.event.property.post\",\"id\":\"203302322\",\"params\":{\"PowerSwitch_3\":0},\"version\":\"1.0.0\"}");  //如果LED3是高电平，说明是熄灭状态，需要回复关闭状态给服务器
	else            sprintf(temp,"{\"method\":\"thing.event.property.post\",\"id\":\"203302322\",\"params\":{\"PowerSwitch_3\":1},\"version\":\"1.0.0\"}");  //如果LED3是低电平，说明是点亮状态，需要回复打开状态给服务器
	MQTT_PublishQs0(P_TOPIC_NAME,temp,strlen(temp));   //添加数据，发布给服务器		
}
/*-------------------------------------------------*/
/*函数名：判断开关4状态，并发布给服务器            */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void LED4_State(void)
{
	char temp[TBUFF_UNIT];                   //定义一个临时缓冲区
		
	if(LED4_IN_STA) sprintf(temp,"{\"method\":\"thing.event.property.post\",\"id\":\"203302322\",\"params\":{\"PowerSwitch_4\":0},\"version\":\"1.0.0\"}");  //如果LED4是高电平，说明是熄灭状态，需要回复关闭状态给服务器
	else            sprintf(temp,"{\"method\":\"thing.event.property.post\",\"id\":\"203302322\",\"params\":{\"PowerSwitch_4\":1},\"version\":\"1.0.0\"}");  //如果LED4是低电平，说明是点亮状态，需要回复打开状态给服务器
	MQTT_PublishQs0(P_TOPIC_NAME,temp,strlen(temp));   //添加数据，发布给服务器		
}
/*-------------------------------------------------*/
/*函数名：判断4个开关状态，并发布给服务器          */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void LED_State(void)
{
	char temp[TBUFF_UNIT];                   //定义一个临时缓冲区
	char sta1,sta2,sta3,sta4;                //定义4个变量保存各个led的状态
	
	if(LED1_IN_STA) sta1 = 0;                //如果是高电平，是关闭状态，状态值=0
	else sta1 = 1;                           //如果是低电平，是打开状态，状态值=1
	if(LED2_IN_STA) sta2 = 0;                //如果是高电平，是关闭状态，状态值=0
	else sta2 = 1;                           //如果是低电平，是打开状态，状态值=1
	if(LED3_IN_STA) sta3 = 0;                //如果是高电平，是关闭状态，状态值=0
	else sta3 = 1;                           //如果是低电平，是打开状态，状态值=1
	if(LED4_IN_STA) sta4 = 0;                //如果是高电平，是关闭状态，状态值=0
	else sta4 = 1;                           //如果是低电平，是打开状态，状态值=1
		
	sprintf(temp,"{\"method\":\"thing.event.property.post\",\"id\":\"203302322\",\"params\":{\"PowerSwitch\":%d,\"PowerSwitch_2\":%d,\"PowerSwitch_3\":%d,\"PowerSwitch_4\":%d},\"version\":\"1.0.0\"}",sta1,sta2,sta3,sta4);  //需要回复状态给服务器
	
	MQTT_PublishQs0(P_TOPIC_NAME,temp,strlen(temp));   //添加数据，发布给服务器		
}
