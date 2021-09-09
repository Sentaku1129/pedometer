#include	<reg51.h>
#include 	<stdio.h>

#include	"UART.h"

void UsartInit()
{
	SCON=0X50;			//设置为工作方式1
	TMOD=0X20;			//设置计数器工作方式2
	PCON=0X80;			//波特率加倍
	TH1=0XF4;				//波特率4800
	TL1=0XF4;
	//ES=1;						//打开接收中断
	EA=1;						//打开总中断
	IT0=1;
	EX0=1;
	TR1=1;					//打开计数器
}

void send_charcter(char buf)
{
	SBUF=buf;
	while(!TI);
	TI=0;
}

/*
void send_x()
{
	send_charcter('x');
	send_charcter(':');
	send_charcter(' ');
}

void send_y()
{
	send_charcter('x');
	send_charcter(':');
	send_charcter(' ');
}

void send_z()
{
	send_charcter('x');
	send_charcter(':');
	send_charcter(' ');
}

*/

//串口发送整形数字
void send_num(unsigned int str)
{
	unsigned char buf[10];
	unsigned char i;
	sprintf(buf,"%d",str);
	for(i=0;i<10&&buf[i]!='\0';i++)
		send_charcter(buf[i]);
	send_charcter(' ');
}
//串口发送字符串
void send_string(unsigned char *buf)
{
	unsigned char i;
	for(i=0;i<20&&buf[i]!='\0';i++)
		send_charcter(buf[i]);
	send_charcter('\r');
	send_charcter('\n');
}

//纸飞机助手可视化显示
void Paper_aircraft(unsigned int a,unsigned int b,unsigned int c,unsigned int d)
{
	send_charcter('{');
	send_string("plotter:");
	send_num(a);
	send_charcter(',');
	send_num(b);
	send_charcter(',');
	send_num(c);
	send_charcter(',');
	send_num(d);
	send_charcter('}');
}

