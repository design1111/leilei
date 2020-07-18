#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "key4_4.h"
#include "malloc.h"
#include "usart2.h"
#include "AS608.h"
#include "timer.h"
#include "stm32f10x.h"
 
/************************************************
 //STM32F103ZE核心板
 
 跑马灯实验 

************************************************/
#define usart2_baund  9600//串口2波特率，根据指纹模块波特率更改

SysPara AS608Para;//指纹模块AS608参数
u16 ValidN;//模块内有效指纹个数
void Add_FR(void);	//录指纹
void Del_FR(void);	//删除指纹
void press_FR(void);//刷指纹
void ShowErrMessage(u8 ensure);//显示确认码错误信息

 int main(void)
 {		

	 u8 ensure;
	u8 key_num;
	char *str;	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init();  	//初始化延时函数
	uart_init(115200);	//初始化串口1波特率为115200，用于支持USMART
	usart2_init(usart2_baund);//初始化串口2,用于与指纹模块通讯
	PS_StaGPIO_Init();	//初始化FR读状态引脚
//	BEEP_Init();  			//初始化蜂鸣器
	KEY_Init();					//按键初始化 
// 	LCD_Init();					//LCD初始化 	
//	W25QXX_Init();			//初始化W25Q128
//	tp_dev.init();			//初始化触摸屏
//	usmart_dev.init(72);		//初始化USMART
	my_mem_init(SRAMIN);		//初始化内部内存池 
//	exfuns_init();			//为fatfs相关变量申请内存  
// 	f_mount(fs[1],"1:",1);  //挂载FLASH.
//	POINT_COLOR=RED;

	while(PS_HandShake(&AS608Addr))//与AS608模块握手
	{
		delay_ms(400);
//		LCD_Fill(0,40,240,80,WHITE);
//		Show_Str_Mid(0,40,"未检测到模块!!!",16,240);
		printf("未检测到模块\n");
		delay_ms(800);
//		LCD_Fill(0,40,240,80,WHITE);
//		Show_Str_Mid(0,40,"尝试连接模块...",16,240);	
			printf("尝试连接模块..\n");
	}
//	LCD_Fill(30,40,240,100,WHITE);
//	Show_Str_Mid(0,40,"通讯成功!!!",16,240);
	printf("通讯成功\n");
	str=mymalloc(SRAMIN,30);
//	sprintf(str,"波特率:%d   地址:%x",usart2_baund,AS608Addr);
//	Show_Str(0,60,240,16,(u8*)str,16,0);
	ensure=PS_ValidTempleteNum(&ValidN);//读库指纹个数
	if(ensure!=0x00)
		ShowErrMessage(ensure);//显示确认码错误信息	
	ensure=PS_ReadSysPara(&AS608Para);  //读参数 
	if(ensure==0x00)
	{
		mymemset(str,0,50);
	//	sprintf(str,"库容量:%d     对比等级: %d",AS608Para.PS_max-ValidN,AS608Para.PS_level);
//		Show_Str(0,80,240,16,(u8*)str,16,0);
	}
	else
		ShowErrMessage(ensure);	
	myfree(SRAMIN,str);
//	AS608_load_keyboard(0,170,(u8**)kbd_menu);//加载虚拟键盘
	while(1)
	{
		key_num=KEY_Scan(0);	
		if(key_num)
		{
			if(key_num==1)Del_FR();		//删指纹
			if(key_num==3)Add_FR();		//录指纹									
		}
		if(PS_Sta)	 //检测PS_Sta状态，如果有手指按下
		{
			press_FR();//刷指纹			
		}				 
	} 	

			 
	 
	 

 }
//显示确认码错误信息
void ShowErrMessage(u8 ensure)
{
//	LCD_Fill(0,120,lcddev.width,160,WHITE);
//	Show_Str_Mid(0,120,(u8*)EnsureMessage(ensure),16,240);
	printf("错误码：%s\n",(u8*)EnsureMessage(ensure));
}

void Add_FR(void)
{
	u8 i,ensure ,processnum=0;
	u16 ID;
	
	while(i != 5)
	{
		if(0 == processnum)  //第一次录入指纹
		{
			
			ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					
					ensure=PS_GenChar(CharBuffer1);//生成特征
//					BEEP=0;
					if(ensure==0x00)
					{
						i++;
						printf("指纹正常\n");
						i=0;
						processnum=1;//跳到第二步						
					}else ShowErrMessage(ensure);				
				}else ShowErrMessage(ensure);	
		}
		else if(1 == processnum)  //第二次录入指纹
		{
			

				printf("请按再按一次指纹\n");
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					ensure=PS_GenChar(CharBuffer2);//生成特征
					if(ensure==0x00)
					{
						i++;
						printf("指纹正常\n");
						i=0;
						processnum=2;//跳到第三步
					}else ShowErrMessage(ensure);	
				}else ShowErrMessage(ensure);	
		}
		else if(2 == processnum)  //两次指纹对比
		{
				i++;
				printf("对比两次指纹\n");
				ensure=PS_Match();
				if(ensure==0x00) 
				{
					printf("对比成功,两次指纹一样\n");
					processnum=3;//跳到第四步
				}
				else 
				{
					printf("对比失败，请重新录入指纹\n");
					ShowErrMessage(ensure);
					i=0;
					processnum=0;//跳回第一步		
				}
				delay_ms(1200);
		}
		else if(3 == processnum)  //生成指纹模块成功
		{
				i++;
				printf("生成指纹模板\n");
				ensure=PS_RegModel();
				if(ensure==0x00) 
				{
					printf("生成指纹模板成功\n");
					processnum=4;//跳到第五步
				}else {processnum=0;ShowErrMessage(ensure);}
				delay_ms(1200);
		}
		else if(4 == processnum)
		{
				i++;
//				printf("请输入储存ID,按Enter保存\n");
				
				ensure=PS_StoreChar(CharBuffer2,1);//储存模板
				if(ensure==0x00) 
				{		
					printf("录入指纹成功\n");
					PS_ValidTempleteNum(&ValidN);//读库指纹个数
					printf("%d",AS608Para.PS_max-ValidN);
					delay_ms(1500);
					return ;
				}else {processnum=0;ShowErrMessage(ensure);}	
		}
		
		
	}
}




//刷指纹
void press_FR(void)
{
	SearchResult seach;
	u8 ensure;
	char *str;
	ensure=PS_GetImage();
	if(ensure==0x00)//获取图像成功 
	{	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //生成特征成功
		{	
			ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
			if(ensure==0x00)//搜索成功
			{		
					printf("刷指纹成功\n");
				str=mymalloc(SRAMIN,50);
//				sprintf(str,"确有此人,ID:%d  匹配得分:%d",seach.pageID,seach.mathscore);
				myfree(SRAMIN,str);
			}
			else 
				ShowErrMessage(ensure);					
	  }
		else
			ShowErrMessage(ensure);

	 delay_ms(600);
	}

}

////删除指纹
void Del_FR(void)
{
	u8  ensure;

	delay_ms(50);

	ensure=PS_DeletChar(1,1);
	if(ensure==0)
	{
		printf("删除指纹成功\n");
	}
	else
		ShowErrMessage(ensure);
	delay_ms(1200);
	PS_ValidTempleteNum(&ValidN);//读库指纹个数
	delay_ms(50);
	
}

