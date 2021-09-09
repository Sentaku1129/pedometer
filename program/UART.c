#include	<reg51.h>
#include 	<stdio.h>

#include	"UART.h"

void UsartInit()
{
	SCON=0X50;			//����Ϊ������ʽ1
	TMOD=0X20;			//���ü�����������ʽ2
	PCON=0X80;			//�����ʼӱ�
	TH1=0XF4;				//������4800
	TL1=0XF4;
	//ES=1;						//�򿪽����ж�
	EA=1;						//�����ж�
	IT0=1;
	EX0=1;
	TR1=1;					//�򿪼�����
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

//���ڷ�����������
void send_num(unsigned int str)
{
	unsigned char buf[10];
	unsigned char i;
	sprintf(buf,"%d",str);
	for(i=0;i<10&&buf[i]!='\0';i++)
		send_charcter(buf[i]);
	send_charcter(' ');
}
//���ڷ����ַ���
void send_string(unsigned char *buf)
{
	unsigned char i;
	for(i=0;i<20&&buf[i]!='\0';i++)
		send_charcter(buf[i]);
	send_charcter('\r');
	send_charcter('\n');
}

//ֽ�ɻ����ֿ��ӻ���ʾ
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

