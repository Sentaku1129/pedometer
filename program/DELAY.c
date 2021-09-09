#include "DELAY.h"
#include <intrins.h>


void Delay5ms()
{
	unsigned char i;
  for(i=0;i<5;i++)
		Delay1ms();
}

void Delay1ms(void)
{
    uchar a,b;
    for(b=4;b>0;b--)
        for(a=113;a>0;a--);
}

void Delayms(uchar a)
{
	uchar i;
	for(i=0;i<a;i++)
		Delay1ms();
}

void Delay5us()
{
	_nop_();_nop_();_nop_();_nop_();_nop_();
}