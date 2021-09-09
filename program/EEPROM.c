#include "IIC.h"
#include "EEPROM.h"
#include "DELAY.h"




//******���ֽ�д��*******************************************
void EEPROM_Single_Write(unsigned char REG_Address,unsigned char REG_data)
{
    Start();                  //��ʼ�ź�
    SendByte(EEPROM_SlaveAddress);   //�����豸��ַ
    SendByte(REG_Address);    //�ڲ��Ĵ�����ַ 
    SendByte(REG_data);       //�ڲ����ݵ�ַ
    Stop();                   //�����ź�
}

//********���ֽڶ�ȡ*****************************************
unsigned char EEPROM_Single_Read(unsigned char REG_Address)
{  
		unsigned char REG_data;
   	Start();                          //��ʼ�ź�
  	SendByte(EEPROM_SlaveAddress);           //�����豸��ַ
   	SendByte(REG_Address);            //���ͼĴ�����ַ
   	Start();                          //��ʼ�ź�
   	SendByte(EEPROM_SlaveAddress+1);         //�����豸��ַ
   	REG_data=RecvByte();              //����һ�ֽ�����
   	SendACK(1);   					 			 	 //����Ӧ��λ
   	Stop();                      		  //�����ź�
   	return REG_data; 
}


/**************************************
��IIC���߷���һ�ֽ�����
**************************************/
void EX_SendByte(unsigned char dat)
{
    uchar i;
    for (i=0; i<8; i++)         //8λ������
    {
        dat <<= 1;              //�Ƴ��������λ
        SDA = CY;               //�����ݿ�
        SCL = 1;                //����ʱ����
        Delay5us();             //��ʱ
        SCL = 0;                //����ʱ����
        Delay5us();             //��ʱ
    }
    RecvACK();
}

//******�ⲿ�жϵ��ֽ�д��*******************************************
void EX_EEPROM_Single_Write(unsigned char REG_Address,unsigned char REG_data)
{
    Start();                  //��ʼ�ź�
    EX_SendByte(EEPROM_SlaveAddress);   //�����豸��ַ
    EX_SendByte(REG_Address);    //�ڲ��Ĵ�����ַ 
    EX_SendByte(REG_data);       //�ڲ����ݵ�ַ
    Stop();                   //�����ź�
}

