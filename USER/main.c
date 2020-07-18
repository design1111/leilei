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
 //STM32F103ZE���İ�
 
 �����ʵ�� 

************************************************/
#define usart2_baund  9600//����2�����ʣ�����ָ��ģ�鲨���ʸ���

SysPara AS608Para;//ָ��ģ��AS608����
u16 ValidN;//ģ������Чָ�Ƹ���
void Add_FR(void);	//¼ָ��
void Del_FR(void);	//ɾ��ָ��
void press_FR(void);//ˢָ��
void ShowErrMessage(u8 ensure);//��ʾȷ���������Ϣ

 int main(void)
 {		

	 u8 ensure;
	u8 key_num;
	char *str;	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init();  	//��ʼ����ʱ����
	uart_init(115200);	//��ʼ������1������Ϊ115200������֧��USMART
	usart2_init(usart2_baund);//��ʼ������2,������ָ��ģ��ͨѶ
	PS_StaGPIO_Init();	//��ʼ��FR��״̬����
//	BEEP_Init();  			//��ʼ��������
	KEY_Init();					//������ʼ�� 
// 	LCD_Init();					//LCD��ʼ�� 	
//	W25QXX_Init();			//��ʼ��W25Q128
//	tp_dev.init();			//��ʼ��������
//	usmart_dev.init(72);		//��ʼ��USMART
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ�� 
//	exfuns_init();			//Ϊfatfs��ر��������ڴ�  
// 	f_mount(fs[1],"1:",1);  //����FLASH.
//	POINT_COLOR=RED;

	while(PS_HandShake(&AS608Addr))//��AS608ģ������
	{
		delay_ms(400);
//		LCD_Fill(0,40,240,80,WHITE);
//		Show_Str_Mid(0,40,"δ��⵽ģ��!!!",16,240);
		printf("δ��⵽ģ��\n");
		delay_ms(800);
//		LCD_Fill(0,40,240,80,WHITE);
//		Show_Str_Mid(0,40,"��������ģ��...",16,240);	
			printf("��������ģ��..\n");
	}
//	LCD_Fill(30,40,240,100,WHITE);
//	Show_Str_Mid(0,40,"ͨѶ�ɹ�!!!",16,240);
	printf("ͨѶ�ɹ�\n");
	str=mymalloc(SRAMIN,30);
//	sprintf(str,"������:%d   ��ַ:%x",usart2_baund,AS608Addr);
//	Show_Str(0,60,240,16,(u8*)str,16,0);
	ensure=PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
	if(ensure!=0x00)
		ShowErrMessage(ensure);//��ʾȷ���������Ϣ	
	ensure=PS_ReadSysPara(&AS608Para);  //������ 
	if(ensure==0x00)
	{
		mymemset(str,0,50);
	//	sprintf(str,"������:%d     �Աȵȼ�: %d",AS608Para.PS_max-ValidN,AS608Para.PS_level);
//		Show_Str(0,80,240,16,(u8*)str,16,0);
	}
	else
		ShowErrMessage(ensure);	
	myfree(SRAMIN,str);
//	AS608_load_keyboard(0,170,(u8**)kbd_menu);//�����������
	while(1)
	{
		key_num=KEY_Scan(0);	
		if(key_num)
		{
			if(key_num==1)Del_FR();		//ɾָ��
			if(key_num==3)Add_FR();		//¼ָ��									
		}
		if(PS_Sta)	 //���PS_Sta״̬���������ָ����
		{
			press_FR();//ˢָ��			
		}				 
	} 	

			 
	 
	 

 }
//��ʾȷ���������Ϣ
void ShowErrMessage(u8 ensure)
{
//	LCD_Fill(0,120,lcddev.width,160,WHITE);
//	Show_Str_Mid(0,120,(u8*)EnsureMessage(ensure),16,240);
	printf("�����룺%s\n",(u8*)EnsureMessage(ensure));
}

void Add_FR(void)
{
	u8 i,ensure ,processnum=0;
	u16 ID;
	
	while(i != 5)
	{
		if(0 == processnum)  //��һ��¼��ָ��
		{
			
			ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					
					ensure=PS_GenChar(CharBuffer1);//��������
//					BEEP=0;
					if(ensure==0x00)
					{
						i++;
						printf("ָ������\n");
						i=0;
						processnum=1;//�����ڶ���						
					}else ShowErrMessage(ensure);				
				}else ShowErrMessage(ensure);	
		}
		else if(1 == processnum)  //�ڶ���¼��ָ��
		{
			

				printf("�밴�ٰ�һ��ָ��\n");
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					ensure=PS_GenChar(CharBuffer2);//��������
					if(ensure==0x00)
					{
						i++;
						printf("ָ������\n");
						i=0;
						processnum=2;//����������
					}else ShowErrMessage(ensure);	
				}else ShowErrMessage(ensure);	
		}
		else if(2 == processnum)  //����ָ�ƶԱ�
		{
				i++;
				printf("�Ա�����ָ��\n");
				ensure=PS_Match();
				if(ensure==0x00) 
				{
					printf("�Աȳɹ�,����ָ��һ��\n");
					processnum=3;//�������Ĳ�
				}
				else 
				{
					printf("�Ա�ʧ�ܣ�������¼��ָ��\n");
					ShowErrMessage(ensure);
					i=0;
					processnum=0;//���ص�һ��		
				}
				delay_ms(1200);
		}
		else if(3 == processnum)  //����ָ��ģ��ɹ�
		{
				i++;
				printf("����ָ��ģ��\n");
				ensure=PS_RegModel();
				if(ensure==0x00) 
				{
					printf("����ָ��ģ��ɹ�\n");
					processnum=4;//�������岽
				}else {processnum=0;ShowErrMessage(ensure);}
				delay_ms(1200);
		}
		else if(4 == processnum)
		{
				i++;
//				printf("�����봢��ID,��Enter����\n");
				
				ensure=PS_StoreChar(CharBuffer2,1);//����ģ��
				if(ensure==0x00) 
				{		
					printf("¼��ָ�Ƴɹ�\n");
					PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
					printf("%d",AS608Para.PS_max-ValidN);
					delay_ms(1500);
					return ;
				}else {processnum=0;ShowErrMessage(ensure);}	
		}
		
		
	}
}




//ˢָ��
void press_FR(void)
{
	SearchResult seach;
	u8 ensure;
	char *str;
	ensure=PS_GetImage();
	if(ensure==0x00)//��ȡͼ��ɹ� 
	{	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //���������ɹ�
		{	
			ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
			if(ensure==0x00)//�����ɹ�
			{		
					printf("ˢָ�Ƴɹ�\n");
				str=mymalloc(SRAMIN,50);
//				sprintf(str,"ȷ�д���,ID:%d  ƥ��÷�:%d",seach.pageID,seach.mathscore);
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

////ɾ��ָ��
void Del_FR(void)
{
	u8  ensure;

	delay_ms(50);

	ensure=PS_DeletChar(1,1);
	if(ensure==0)
	{
		printf("ɾ��ָ�Ƴɹ�\n");
	}
	else
		ShowErrMessage(ensure);
	delay_ms(1200);
	PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
	delay_ms(50);
	
}

