#ifndef __I2C_H_
#define __I2C_H_

#define EEPROM_SlaveAddress 0xA0

sbit k3=P3^2;  //定义按键K3
sbit led=P2^2;	 //定义P20口是led

void EEPROM_Single_Write(unsigned char REG_Address,unsigned char REG_data);
unsigned char EEPROM_Single_Read(unsigned char REG_Address);
void EX_EEPROM_Single_Write(unsigned char REG_Address,unsigned char REG_data);
void EX_SendByte(unsigned char dat);

#endif
