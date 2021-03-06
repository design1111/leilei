/*-------------------------------------------------*/
/*            超纬电子STM32系列开发板              */
/*-------------------------------------------------*/
/*                                                 */
/*          实现各种中断服务函数的源文件           */
/*                                                 */
/*-------------------------------------------------*/

#include "stm32f10x.h"  //包含需要的头文件
#include "main.h"       //包含需要的头文件
#include "usart1.h"     //包含需要的头文件
#include "usart3.h"     //包含需要的头文件
#include "mqtt.h"       //包含需要的头文件
#include "timer6.h"     //包含需要的头文件
#include "led.h"        //包含需要的头文件
#include "dht11.h"      //包含需要的头文件

/*-------------------------------------------------*/
/*函数名：串口3接收中断函数                        */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void USART3_IRQHandler(void)   
{                      
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET){  //如果USART_IT_RXNE标志置位，表示有数据到了，进入if分支
		if(Connect_flag==0){                                //如果Connect_flag等于0，当前还没有连接服务器，处于指令配置状态
			if(USART3->DR){                                 //处于指令配置状态时，非零值才保存到缓冲区	
				Usart3_RxBuff[Usart3_RxCounter]=USART3->DR; //保存到缓冲区	
				Usart3_RxCounter ++;                        //每接收1个字节的数据，Usart3_RxCounter加1，表示接收的数据总量+1 
			}		
		}else{		                                        //反之Connect_flag等于1，连接上服务器了	
			Usart3_RxBuff[Usart3_RxCounter] = USART3->DR;   //把接收到的数据保存到Usart3_RxBuff中				
			if(Usart3_RxCounter == 0){    					//如果Usart3_RxCounter等于0，表示是接收的第1个数据，进入if分支				
				TIM_Cmd(TIM4,ENABLE); 
			}else{                        					//else分支，表示果Usart3_RxCounter不等于0，不是接收的第一个数据
				TIM_SetCounter(TIM4,0);  
			}	
			Usart3_RxCounter ++;         				    //每接收1个字节的数据，Usart3_RxCounter加1，表示接收的数据总量+1 
		}
	}
} 
/*-------------------------------------------------*/
/*函数名：定时器4中断服务函数                      */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET){                //如果TIM_IT_Update置位，表示TIM4溢出中断，进入if	
		
		memcpy(&MQTT_RxDataInPtr[2],Usart3_RxBuff,Usart3_RxCounter);  //拷贝数据到接收缓冲区
		MQTT_RxDataInPtr[0] = Usart3_RxCounter/256;                   //记录数据长度高字节
		MQTT_RxDataInPtr[1] = Usart3_RxCounter%256;                   //记录数据长度低字节
		MQTT_RxDataInPtr+=RBUFF_UNIT;                                  //指针下移
		if(MQTT_RxDataInPtr==MQTT_RxDataEndPtr)                       //如果指针到缓冲区尾部了
			MQTT_RxDataInPtr = MQTT_RxDataBuf[0];                     //指针归位到缓冲区开头
		Usart3_RxCounter = 0;                                         //串口2接收数据量变量清零
		TIM_SetCounter(TIM3, 0);                                      //清零定时器6计数器，重新计时ping包发送时间
		TIM_Cmd(TIM4, DISABLE);                        				  //关闭TIM4定时器
		TIM_SetCounter(TIM4, 0);                        			  //清零定时器4计数器
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);     			  //清除TIM4溢出中断标志 	
	}
}
/*-------------------------------------------------*/
/*函数名：定时器6中断服务函数                      */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void TIM6_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET){   //如果TIM_IT_Update置位，表示TIM6溢出中断，进入if	
		switch(Ping_flag){                               //判断Ping_flag的状态
			case 0:										 //如果Ping_flag等于0，表示正常状态，发送Ping报文  
					MQTT_PingREQ(); 					 //添加Ping报文到发送缓冲区  
					break;
			case 1:										 //如果Ping_flag等于1，说明上一次发送到的ping报文，没有收到服务器回复，所以1没有被清除为0，可能是连接异常，我们要启动快速ping模式
					TIM6_ENABLE_2S(); 					 //我们将定时器6设置为2s定时,快速发送Ping报文
					MQTT_PingREQ();  					 //添加Ping报文到发送缓冲区  
					break;
			case 2:										 //如果Ping_flag等于2，说明还没有收到服务器回复
			case 3:				                         //如果Ping_flag等于3，说明还没有收到服务器回复
			case 4:				                         //如果Ping_flag等于4，说明还没有收到服务器回复	
					MQTT_PingREQ();  					 //添加Ping报文到发送缓冲区 
					break;
			case 5:										 //如果Ping_flag等于5，说明我们发送了多次ping，均无回复，应该是连接有问题，我们重启连接
					Connect_flag = 0;                    //连接状态置0，表示断开，没连上服务器
					TIM_Cmd(TIM6,DISABLE);               //关TIM6 				
					break;			
		}
		Ping_flag++;           		             		 //Ping_flag自增1，表示又发送了一次ping，期待服务器的回复
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update);      //清除TIM6溢出中断标志 	
	}
}
/*-------------------------------------------------*/
/*函数名：定时器2中断服务函数                      */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){   //如果TIM_IT_Update置位，表示TIM2溢出中断，进入if	
		TempHumi_State();
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);      //清除TIM2溢出中断标志 	
	}
}
/*-------------------------------------------------*/
/*函数名：不可屏蔽中断处理函数                     */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void NMI_Handler(void)
{

}

/*-------------------------------------------------*/
/*函数名：硬件出错后进入的中断处理函数             */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void HardFault_Handler(void)
{
 	u1_printf("程序跑飞，准备重启开发板");
	NVIC_SystemReset();
}
/*-------------------------------------------------*/
/*函数名：内存管理中断处理函数                     */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void MemManage_Handler(void)
{

}

/*-------------------------------------------------*/
/*函数名：预取指失败，存储器访问失败中断处理函数   */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void BusFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*函数名：未定义的指令或非法状态处理函数           */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void UsageFault_Handler(void)
{

}

/*-------------------------------------------------*/
/*函数名：软中断，SWI 指令调用的处理函数           */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SVC_Handler(void)
{
	
}

/*-------------------------------------------------*/
/*函数名：调试监控器处理函数                       */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void DebugMon_Handler(void)
{

}

/*-------------------------------------------------*/
/*函数名：可挂起的系统服务处理函数                 */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void PendSV_Handler(void)
{

}

/*-------------------------------------------------*/
/*函数名：SysTic系统嘀嗒定时器处理函数             */
/*参  数：无                                       */
/*返回值：无                                       */
/*-------------------------------------------------*/
void SysTick_Handler(void)
{

}
