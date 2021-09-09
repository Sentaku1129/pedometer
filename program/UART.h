/*file UART.h*/

#ifndef __UART_H_
#define __UART_H_

void UsartInit();
void send_charcter(char buf);
void send_x();
void send_y();
void send_z();
void send_num(unsigned int str);
void send_string(unsigned char *buf);

void Paper_aircraft(unsigned int a,unsigned int b,unsigned int c,unsigned int d);

#endif