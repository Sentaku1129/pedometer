#include "IIC.h"
#include "EEPROM.h"
#include "DELAY.h"




//******单字节写入*******************************************
void EEPROM_Single_Write(unsigned char REG_Address,unsigned char REG_data)
{
    Start();                  //起始信号
    SendByte(EEPROM_SlaveAddress);   //发送设备地址
    SendByte(REG_Address);    //内部寄存器地址 
    SendByte(REG_data);       //内部数据地址
    Stop();                   //结束信号
}

//********单字节读取*****************************************
unsigned char EEPROM_Single_Read(unsigned char REG_Address)
{  
		unsigned char REG_data;
   	Start();                          //起始信号
  	SendByte(EEPROM_SlaveAddress);           //发送设备地址
   	SendByte(REG_Address);            //发送寄存器地址
   	Start();                          //起始信号
   	SendByte(EEPROM_SlaveAddress+1);         //发送设备地址
   	REG_data=RecvByte();              //接收一字节数据
   	SendACK(1);   					 			 	 //发送应答位
   	Stop();                      		  //结束信号
   	return REG_data; 
}


/**************************************
向IIC总线发送一字节数据
**************************************/
void EX_SendByte(unsigned char dat)
{
    uchar i;
    for (i=0; i<8; i++)         //8位计数器
    {
        dat <<= 1;              //移除数据最高位
        SDA = CY;               //送数据口
        SCL = 1;                //拉高时钟线
        Delay5us();             //延时
        SCL = 0;                //拉低时钟线
        Delay5us();             //延时
    }
    RecvACK();
}

//******外部中断单字节写入*******************************************
void EX_EEPROM_Single_Write(unsigned char REG_Address,unsigned char REG_data)
{
    Start();                  //起始信号
    EX_SendByte(EEPROM_SlaveAddress);   //发送设备地址
    EX_SendByte(REG_Address);    //内部寄存器地址 
    EX_SendByte(REG_data);       //内部数据地址
    Stop();                   //结束信号
}

