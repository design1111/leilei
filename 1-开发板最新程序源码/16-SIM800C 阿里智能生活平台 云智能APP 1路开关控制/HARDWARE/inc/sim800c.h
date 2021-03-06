/*-------------------------------------------------*/
/*            超纬电子STM32系列开发板              */
/*-------------------------------------------------*/
/*                                                 */
/*             操作SIM800C功能的头文件             */
/*                                                 */
/*-------------------------------------------------*/

#ifndef __SIM800C_H
#define __SIM800C_H

#include "usart3.h"	    //包含需要的头文件

#define POWER_STA       GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_1)        //PC1的电平，可以判断800C是开机还是关机
#define POWER_KEY(x)    GPIO_WriteBit(GPIOC, GPIO_Pin_0, (BitAction)x)  //PC0控制800C的开关机

#define SIM800C_printf       u3_printf           //串口2控制 800C
#define SIM800C_RxCounter    Usart3_RxCounter    //串口2控制 800C
#define SIM800C_RX_BUF       Usart3_RxBuff       //串口2控制 800C
#define SIM800C_RXBUFF_SIZE  USART3_RXBUFF_SIZE  //串口2控制 800C
 
void SIM800C_GPIO_Init(void);
char SIM800C_Power(void);
char SIM800C_SendCmd(char *, int);
char SIM800C_CREG(int);
char SIM800C_CSQ(int timeout);
char SIM800C_CGATT(int timeout);
char SIM800C_ActivateNetwork(void);
char SIM800C_TCPConnect(int);
void SIM800C_TxData(unsigned char *);
char SIM800C_Connect_IoTServer(void);

#endif


