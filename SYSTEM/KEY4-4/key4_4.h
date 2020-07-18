#ifndef _KEY4_4_H
#define _KEY4_4_H
#include "stm32f10x.h"
/*
*          | s10     -    0      |
*          | s11     -    ��      |
*          | s12     -    ��      |
*          | s13     -    ȷ��     |
*          | s14     -    ȡ��      |
*          | s15     -          |
*/

void KEY4_4_Init(void);
u8 Key_Scan(void);
u8 Key_Scan_1(void);
void Key4x4_GPIO_Config(void);


void Key4_4_PassWord_1(void); //��������
void Key4_4_PassWord(void); //¼������
	
#endif

