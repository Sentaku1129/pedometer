#include <reg51.h>
#include "MPU6050.h"
#include "IIC.h"
#include "DELAY.h"

#include "UART.h"

//******单字节写入*******************************************
void Single_Write(uchar REG_Address,uchar REG_data)
{
    Start();                  //起始信号
    SendByte(SlaveAddress);   //发送设备地址
    SendByte(REG_Address);    //内部寄存器地址 
    SendByte(REG_data);       //内部数据地址
    Stop();                   //结束信号
}

//********单字节读取*****************************************
uchar Single_Read(uchar REG_Address)
{  
		uchar REG_data;
   	Start();                          //起始信号
  	SendByte(SlaveAddress);           //发送设备地址
   	SendByte(REG_Address);            //发送寄存器地址
   	Start();                          //起始信号
   	SendByte(SlaveAddress+1);         //发送设备地址
   	REG_data=RecvByte();              //接收一字节数据
   	SendACK(1);   					 			 	 //发送应答位
   	Stop();                      		  //结束信号
   	return REG_data; 
}

//初始化MPU6050***********************
void Init_MPU6050()
{
	uchar res;
	Single_Write(MPU_PWR_MGMT1_REG,0X80);	//复位MPU6050
    Delayms(100);
	Single_Write(MPU_PWR_MGMT1_REG,0X00);	//唤醒MPU6050 
	MPU_Set_Gyro_Fsr(3);					//陀螺仪传感器,±2000dps
	MPU_Set_Accel_Fsr(2);					//加速度传感器,±8g
	MPU_Set_Rate(50);						//设置采样率50Hz
	Single_Write(MPU_INT_EN_REG,0X00);	//关闭所有中断
	Single_Write(MPU_USER_CTRL_REG,0X00);	//I2C主模式关闭
	Single_Write(MPU_FIFO_EN_REG,0X00);	//关闭FIFO
	Single_Write(MPU_INTBP_CFG_REG,0X80);	//INT引脚低电平有效
	
	res=Single_Read(MPU_DEVICE_ID_REG);
	if(res==0x68)//器件ID正确
	{
		Single_Write(MPU_PWR_MGMT1_REG,0X01);	//设置CLKSEL,PLL X轴为参考
		Single_Write(MPU_PWR_MGMT2_REG,0X00);	//加速度与陀螺仪都工作
		MPU_Set_Rate(50);						//设置采样率为50Hz
 	}
}

//设置MPU6050陀螺仪传感器满量程范围
//fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
//返回值:0,设置成功
//    其他,设置失败 
void MPU_Set_Gyro_Fsr(uchar fsr)
{
	Single_Write(MPU_GYRO_CFG_REG,fsr<<3);//设置陀螺仪满量程范围  
}

//设置MPU6050加速度传感器满量程范围
//fsr:0,±2g;1,±4g;2,±8g;3,±16g
//返回值:0,设置成功
//    其他,设置失败 
void MPU_Set_Accel_Fsr(uchar fsr)
{
	Single_Write(MPU_ACCEL_CFG_REG,fsr<<3);//设置加速度传感器满量程范围  
}

//设置MPU6050的数字低通滤波器
//lpf:数字低通滤波频率(Hz)
//返回值:0,设置成功
//    其他,设置失败 
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
	Single_Write(MPU_CFG_REG,temp_data);//设置数字低通滤波器  
}
//设置MPU6050的采样率(假定Fs=1KHz)
//rate:4~1000(Hz)
//返回值:0,设置成功
//    其他,设置失败 
void MPU_Set_Rate(uint rate)
{
	uchar temp_data;
	if(rate>1000)
		rate=1000;
	if(rate<4)
		rate=4;
	temp_data=1000/rate-1;
	Single_Write(MPU_SAMPLE_RATE_REG,temp_data);	//设置数字低通滤波器
 	MPU_Set_LPF(rate/2);	//自动设置LPF为采样率的一半
}


//************连续读六个寄存器内容
void Multiple_read(uchar REG_Address,uchar *BUF)
{
    uchar i;
    Start();                          //起始
    SendByte(SlaveAddress);           //设备地址
    SendByte(REG_Address);            //存储单元地址
    Start();                          //起始信号
    SendByte(SlaveAddress+1);         //设备地址加读信号
    for (i=0; i<6; i++)               //连续接收六个地址数据  存入BUF
    {
        BUF[i] = RecvByte();          //
        if (i == 5)
        {
          SendACK(1);                //最后一个数据
        }
        else
        {
          SendACK(0);                //回应ACK
        }
    }
    Stop();                          //结束
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