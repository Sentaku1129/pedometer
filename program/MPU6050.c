#include <reg51.h>
#include "MPU6050.h"
#include "IIC.h"
#include "DELAY.h"

#include "UART.h"

//******���ֽ�д��*******************************************
void Single_Write(uchar REG_Address,uchar REG_data)
{
    Start();                  //��ʼ�ź�
    SendByte(SlaveAddress);   //�����豸��ַ
    SendByte(REG_Address);    //�ڲ��Ĵ�����ַ 
    SendByte(REG_data);       //�ڲ����ݵ�ַ
    Stop();                   //�����ź�
}

//********���ֽڶ�ȡ*****************************************
uchar Single_Read(uchar REG_Address)
{  
		uchar REG_data;
   	Start();                          //��ʼ�ź�
  	SendByte(SlaveAddress);           //�����豸��ַ
   	SendByte(REG_Address);            //���ͼĴ�����ַ
   	Start();                          //��ʼ�ź�
   	SendByte(SlaveAddress+1);         //�����豸��ַ
   	REG_data=RecvByte();              //����һ�ֽ�����
   	SendACK(1);   					 			 	 //����Ӧ��λ
   	Stop();                      		  //�����ź�
   	return REG_data; 
}

//��ʼ��MPU6050***********************
void Init_MPU6050()
{
	uchar res;
	Single_Write(MPU_PWR_MGMT1_REG,0X80);	//��λMPU6050
    Delayms(100);
	Single_Write(MPU_PWR_MGMT1_REG,0X00);	//����MPU6050 
	MPU_Set_Gyro_Fsr(3);					//�����Ǵ�����,��2000dps
	MPU_Set_Accel_Fsr(2);					//���ٶȴ�����,��8g
	MPU_Set_Rate(50);						//���ò�����50Hz
	Single_Write(MPU_INT_EN_REG,0X00);	//�ر������ж�
	Single_Write(MPU_USER_CTRL_REG,0X00);	//I2C��ģʽ�ر�
	Single_Write(MPU_FIFO_EN_REG,0X00);	//�ر�FIFO
	Single_Write(MPU_INTBP_CFG_REG,0X80);	//INT���ŵ͵�ƽ��Ч
	
	res=Single_Read(MPU_DEVICE_ID_REG);
	if(res==0x68)//����ID��ȷ
	{
		Single_Write(MPU_PWR_MGMT1_REG,0X01);	//����CLKSEL,PLL X��Ϊ�ο�
		Single_Write(MPU_PWR_MGMT2_REG,0X00);	//���ٶ��������Ƕ�����
		MPU_Set_Rate(50);						//���ò�����Ϊ50Hz
 	}
}

//����MPU6050�����Ǵ����������̷�Χ
//fsr:0,��250dps;1,��500dps;2,��1000dps;3,��2000dps
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
void MPU_Set_Gyro_Fsr(uchar fsr)
{
	Single_Write(MPU_GYRO_CFG_REG,fsr<<3);//���������������̷�Χ  
}

//����MPU6050���ٶȴ����������̷�Χ
//fsr:0,��2g;1,��4g;2,��8g;3,��16g
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
void MPU_Set_Accel_Fsr(uchar fsr)
{
	Single_Write(MPU_ACCEL_CFG_REG,fsr<<3);//���ü��ٶȴ����������̷�Χ  
}

//����MPU6050�����ֵ�ͨ�˲���
//lpf:���ֵ�ͨ�˲�Ƶ��(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
void MPU_Set_LPF(uint lpf)
{
	uchar temp_data=0;
	if(lpf>=188)
		temp_data=1;
	else if(lpf>=98)
		temp_data=2;
	else if(lpf>=42)
		temp_data=3;
	else if(lpf>=20)
		temp_data=4;
	else if(lpf>=10)
		temp_data=5;
	else
		temp_data=6; 
	Single_Write(MPU_CFG_REG,temp_data);//�������ֵ�ͨ�˲���  
}
//����MPU6050�Ĳ�����(�ٶ�Fs=1KHz)
//rate:4~1000(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
void MPU_Set_Rate(uint rate)
{
	uchar temp_data;
	if(rate>1000)
		rate=1000;
	if(rate<4)
		rate=4;
	temp_data=1000/rate-1;
	Single_Write(MPU_SAMPLE_RATE_REG,temp_data);	//�������ֵ�ͨ�˲���
 	MPU_Set_LPF(rate/2);	//�Զ�����LPFΪ�����ʵ�һ��
}


//************�����������Ĵ�������
void Multiple_read(uchar REG_Address,uchar *BUF)
{
    uchar i;
    Start();                          //��ʼ
    SendByte(SlaveAddress);           //�豸��ַ
    SendByte(REG_Address);            //�洢��Ԫ��ַ
    Start();                          //��ʼ�ź�
    SendByte(SlaveAddress+1);         //�豸��ַ�Ӷ��ź�
    for (i=0; i<6; i++)               //��������������ַ����  ����BUF
    {
        BUF[i] = RecvByte();          //
        if (i == 5)
        {
          SendACK(1);                //���һ������
        }
        else
        {
          SendACK(0);                //��ӦACK
        }
    }
    Stop();                          //����
    //Delay5ms();
}

void GET_AXIS(uint *axis_x,uint *axis_y,uint *axis_z)
{
	uchar BUF[6];
	Multiple_read(0x3B,BUF);
	*axis_x=((uint)BUF[0]<<8)|BUF[1];
	*axis_y=((uint)BUF[2]<<8)|BUF[3];
	*axis_z=((uint)BUF[4]<<8)|BUF[5];
}