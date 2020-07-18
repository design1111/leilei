#include "key4_4.h"
#include "usart.h"

int key[20]; //录入密码
int key_1[20];

void KEY4_4_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; 
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
        GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
}

void KEY4_4_Scan(void)
{
	
	
}

/****************************************
* 文件名  ：key4x4.c
* 描述    ：按键应用函数库        
* 硬件连接：-------------------------
*          | PF8  - R1          |
*          | PF9  - R2                         |
*          | PF10 - R3          |
*          | PF11 - R4                     |
*          | PF12 - C1          |
*          | PF13 - C2                     |
*          | PF14 - C3          |
*          | PF15 - C4                         |
*           -------------------------

* 表达显示：-------------------------
*          | S1  - 1          |
*          | S2  - 2                         |
*          | S3 - 3          |
*          | S4 - 4                     |
*          | S5 - 5          |
*          | S6 - 6                     |
*          | S7 - 7          |
*          | S8 - 8                         |
*          | S9  - 9          |
*          | S10  - 10                         |
*          | S11 - 11          |
*          | S12 - 12                     |
*          | S13 - 13          |
*          | S14 - 14                     |
*          | S15 - 15          |
*          | S16 - 16                         |
*           -------------------------

* 库版本  ：ST3.5.0
**********************************************************************************/

//#include "key4x4.h" 
#include "delay.h" 

/*
* 函数名：Key_GPIO_Config
* 描述  ：配置按键用到的I/O口
* 输入  ：无
* 输出  ：无
*/
void Key4x4_GPIO_Config(void)
{
          GPIO_InitTypeDef GPIO_InitStructure;
          
          /*开启按键端口的时钟*/
          RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF,ENABLE);

        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15; 
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
        GPIO_Init(GPIOF, &GPIO_InitStructure);

          GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 |GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;        
          GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
          GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
          GPIO_Init(GPIOF, &GPIO_InitStructure);
}

u8 Key_Scan(void)
{
        u16 temp;
        u8 ucBackValue=0;
        //====扫描第一列==========
        GPIO_Write(GPIOF,0xfe00); 
        temp=GPIO_ReadInputData(GPIOF); //读出整个口得到的值
        temp=temp&0xf000;    //屏蔽低4位
        if (temp!=0xf000)      //假如高4位不全是1
        { 
                delay_ms(20);      // 延时消抖再读
                temp=GPIO_ReadInputData(GPIOF); 
                temp=temp&0xf000;
                if (temp!=0xf000)     //消抖后如果再次高4位确定不是全1
                { 
                        temp=GPIO_ReadInputData(GPIOF)&0xff00; //读出此次按键的值
                        switch(temp)
                        { 
                                case 0xee00:
                                ucBackValue = 1;break;
                                case 0xde00:
                                ucBackValue = 2;break;
                                case 0xbe00:
                                ucBackValue = 3;break;
                                case 0x7e00:
                                ucBackValue = 4;break;
                                default:break;
                        }
                        while(temp!=0xf000)   //等待按键放开，初始必然被执行
                        { 
                                temp=GPIO_ReadInputData(GPIOF); 
                                temp=temp&0xf000;
                        }

                }
        }

        //====第二列送0==========
        GPIO_Write(GPIOF,0xfd00); 
        temp=GPIO_ReadInputData(GPIOF); //读出整个口得到的值
        temp=temp&0xf000;    //屏蔽低4位
        if (temp!=0xf000)      //假如高4位不全是1
        { 
                delay_ms(20);      // 延时消抖再读
                temp=GPIO_ReadInputData(GPIOF); 
                temp=temp&0xf000;
                if (temp!=0xf000)     //消抖后如果再次高4位确定不是全1
                { 
                        temp=GPIO_ReadInputData(GPIOF)&0xff00; //读出此次按键的值
                        switch(temp)
                        { 
                                case 0xed00:
                                        ucBackValue = 5; break;//
                                case 0xdd00:
                                        ucBackValue = 6; break;
                                case 0xbd00:
                                        ucBackValue = 7; break;
                                case 0x7d00:
                                        ucBackValue = 8; break;
                                default:break;
                }
                        while(temp!=0xf000)   //等待按键放开
                        { 
                                temp=GPIO_ReadInputData(GPIOF); 
                                temp=temp&0xf000;
                        }
                }
        }
        //====第3列送0==========
        GPIO_Write(GPIOF,0xfb00); 
        temp=GPIO_ReadInputData(GPIOF); //读出整个口得到的值
        temp=temp&0xf000;    //屏蔽低4位
        if (temp!=0xf000)      //假如高4位不全是1
        { 
                delay_ms(20);      // 延时消抖再读
                temp=GPIO_ReadInputData(GPIOF); 
                temp=temp&0xf000;
                if (temp!=0xf000)     //消抖后如果再次高4位确定不是全1
                { 
                        temp=GPIO_ReadInputData(GPIOF)&0xff00; //读出此次按键的值
                        switch(temp)
                        { 
                                case 0xeb00:
                                ucBackValue = 9; break;
                                case 0xdb00:
                                ucBackValue = 10; break;
                                case 0xbb00:
                                ucBackValue = 11; break;
                                case 0x7b00:
                                ucBackValue = 12; break;
                                default:break;
                }
                while(temp!=0xf000)   //等待按键放开
                { 
                        temp=GPIO_ReadInputData(GPIOF); 
                        temp=temp&0xf000;
                }
                }
        }
        //====第4列送0==========
        GPIO_Write(GPIOF,0xf700); 
        temp=GPIO_ReadInputData(GPIOF); //读出整个口得到的值
        temp=temp&0xf000;    //屏蔽低4位
        if (temp!=0xf000)      //假如高4位不全是1
        { 
                delay_ms(20);       // 延时消抖再读
                temp=GPIO_ReadInputData(GPIOF); 
                temp=temp&0xf000;
                if (temp!=0xf000)     //消抖后如果再次高4位确定不是全1
                { 
                        temp=GPIO_ReadInputData(GPIOF)&0xff00; 
                        switch(temp)
                        { 
                                case 0xe700:
                                ucBackValue = 13; break;
                                case 0xd700:
                                ucBackValue = 14; break;
                                case 0xb700:
                                ucBackValue = 15; break;
                                case 0x7700:
                                ucBackValue = 16; break;
                                default:break;
                        }
                        while(temp!=0xf000)   //等待按键放开
                        { 
                                temp=GPIO_ReadInputData(GPIOF); 
                                temp=temp&0xf000;
                        }
                }
        }
        return ucBackValue;
}








u8 Key_Scan_1(void)
{
        u16 temp;
        u8 ucBackValue=0;
        //====扫描第一列==========
        GPIO_Write(GPIOF,0xfe00); 
        temp=GPIO_ReadInputData(GPIOF); //读出整个口得到的值
        temp=temp&0xf000;    //屏蔽低4位
        if (temp!=0xf000)      //假如高4位不全是1
        { 
                delay_ms(20);      // 延时消抖再读
                temp=GPIO_ReadInputData(GPIOF); 
                temp=temp&0xf000;
                if (temp!=0xf000)     //消抖后如果再次高4位确定不是全1
                { 
                        temp=GPIO_ReadInputData(GPIOF)&0xff00; //读出此次按键的值
                        switch(temp)
                        { 
                                case 0xee00:
                                ucBackValue = 1;break;
                                case 0xde00:
                                ucBackValue = 2;break;
                                case 0xbe00:
                                ucBackValue = 3;break;
                                case 0x7e00:
                                ucBackValue = 4;break;
                                default:break;
                        }
                        while(temp!=0xf000)   //等待按键放开，初始必然被执行
                        { 
                                temp=GPIO_ReadInputData(GPIOF); 
                                temp=temp&0xf000;
                        }

                }
        }

        //====第二列送0==========
        GPIO_Write(GPIOF,0xfd00); 
        temp=GPIO_ReadInputData(GPIOF); //读出整个口得到的值
        temp=temp&0xf000;    //屏蔽低4位
        if (temp!=0xf000)      //假如高4位不全是1
        { 
                delay_ms(20);      // 延时消抖再读
                temp=GPIO_ReadInputData(GPIOF); 
                temp=temp&0xf000;
                if (temp!=0xf000)     //消抖后如果再次高4位确定不是全1
                { 
                        temp=GPIO_ReadInputData(GPIOF)&0xff00; //读出此次按键的值
                        switch(temp)
                        { 
                                case 0xed00:
                                        ucBackValue = 5; break;//
                                case 0xdd00:
                                        ucBackValue = 6; break;
                                case 0xbd00:
                                        ucBackValue = 7; break;
                                case 0x7d00:
                                        ucBackValue = 8; break;
                                default:break;
                }
                        while(temp!=0xf000)   //等待按键放开
                        { 
                                temp=GPIO_ReadInputData(GPIOF); 
                                temp=temp&0xf000;
                        }
                }
        }
        //====第3列送0==========
        GPIO_Write(GPIOF,0xfb00); 
        temp=GPIO_ReadInputData(GPIOF); //读出整个口得到的值
        temp=temp&0xf000;    //屏蔽低4位
        if (temp!=0xf000)      //假如高4位不全是1
        { 
                delay_ms(20);      // 延时消抖再读
                temp=GPIO_ReadInputData(GPIOF); 
                temp=temp&0xf000;
                if (temp!=0xf000)     //消抖后如果再次高4位确定不是全1
                { 
                        temp=GPIO_ReadInputData(GPIOF)&0xff00; //读出此次按键的值
                        switch(temp)
                        { 
                                case 0xeb00:
                                ucBackValue = 9; break;
//                                case 0xdb00:
//                                ucBackValue = 10; break;
//                                case 0xbb00:
//                                ucBackValue = 11; break;
//                                case 0x7b00:
//                                ucBackValue = 12; break;
                                default:break;
                }
                while(temp!=0xf000)   //等待按键放开
                { 
                        temp=GPIO_ReadInputData(GPIOF); 
                        temp=temp&0xf000;
                }
                }
        }
//        //====第4列送0==========
//        GPIO_Write(GPIOF,0xf700); 
//        temp=GPIO_ReadInputData(GPIOF); //读出整个口得到的值
//        temp=temp&0xf000;    //屏蔽低4位
//        if (temp!=0xf000)      //假如高4位不全是1
//        { 
//                delay_ms(20);       // 延时消抖再读
//                temp=GPIO_ReadInputData(GPIOF); 
//                temp=temp&0xf000;
//                if (temp!=0xf000)     //消抖后如果再次高4位确定不是全1
//                { 
//                        temp=GPIO_ReadInputData(GPIOF)&0xff00; 
//                        switch(temp)
//                        { 
//                                case 0xe700:
//                                ucBackValue = 13; break;
//                                case 0xd700:
//                                ucBackValue = 14; break;
//                                case 0xb700:
//                                ucBackValue = 15; break;
//                                case 0x7700:
//                                ucBackValue = 16; break;
//                                default:break;
//                        }
//                        while(temp!=0xf000)   //等待按键放开
//                        { 
//                                temp=GPIO_ReadInputData(GPIOF); 
//                                temp=temp&0xf000;
//                        }
//                }
//        }
        return ucBackValue;
}

//void Key4_4_PassWord(void) //录入密码
//{
//				printf("开始输入密码\n");
//				delay_ms(10);
//				for(int i=0; i<6; i++)
//				{
//					int a1 = Key_Scan();

//					while(!(key[i] = Key_Scan()));
//					delay_ms(10);
//					
//				}
//				
//				printf("录入密码成功\n");
//	
//}
//void Key4_4_PassWord_1(void) //输入密码
//{
//				printf("开始输入密码\n");
//				delay_ms(10);
//				for(int i=0; i<6; i++)
//				{
//					int a1 = Key_Scan();

//					while(!(key[i] = Key_Scan()));
//					delay_ms(10);
//					
//				}
//				
//				printf("录入密码成功\n");
//}


//u8 KEY4_4Scan(u8 mode)
//{	 
//	static u8 key_up=1;//按键按松开标志
//	if(mode)key_up=1;  //支持连按		  
//	if(key_up&&(KEY0==0||KEY1==0||WK_UP==1))
//	{
//		delay_ms(10);//去抖动 
//		key_up=0;
//		if(KEY0==0)return KEY0_PRES;
//		else if(KEY1==0)return KEY1_PRES;
//		//else if(KEY2==0)return KEY2_PRES;
//		else if(WK_UP==1)return WKUP_PRES;
//	}else if(KEY0==1&&KEY1==1&&WK_UP==0)key_up=1; 	    
// 	return 0;// 无按键按下
//}
