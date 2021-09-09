/*File lcd1602.h*/		
 
#ifndef __LCD1602_H_
#define __LCD1602_H_
 
#define LCD1602_4PINS

#include <reg51.h>
#include <intrins.h>
 
#ifndef  uchar
#define  uchar unsigned char
#endif
 
#ifndef  uint
#define  uint unsigned int
#endif
 
 
#define LCD1602_DATAPINS  P0
sbit  LCD1602_RS = P2^6;
sbit  LCD1602_RW = P2^5;
sbit  LCD1602_E  = P2^7;
 
void Lcd1602_Delay1ms(uint c);  //��� 0us
void LcdWriteCom(uchar com); //д������
void LcdWriteData(uchar dat);	//д������
void LcdInit();			//��ʼ��
void display(uint step);
void init_display();
 
#endif