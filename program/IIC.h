/*File IIC.h*/		
 
#ifndef __IIC_H_
#define __IIC_H_

#include <reg52.h>

 
#ifndef  uchar
#define  uchar unsigned char
#endif
 
#ifndef  uint
#define  uint unsigned int
#endif

sbit SDA=P2^0;
sbit SCL=P2^1;

void Start();
void Stop();
void SendACK(bit ack);
bit RecvACK();
void SendByte(uchar dat);
uchar RecvByte();

#endif