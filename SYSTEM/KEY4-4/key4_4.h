#ifndef _KEY4_4_H
#define _KEY4_4_H
#include "stm32f10x.h"
/*
*          | s10     -    0      |
*          | s11     -    上      |
*          | s12     -    下      |
*          | s13     -    确定     |
*          | s14     -    取消      |
*          | s15     -          |
*/

void KEY4_4_Init(void);
u8 Key_Scan(void);
u8 Key_Scan_1(void);
void Key4x4_GPIO_Config(void);


void Key4_4_PassWord_1(void); //输入密码
void Key4_4_PassWord(void); //录入密码
	
#endif

