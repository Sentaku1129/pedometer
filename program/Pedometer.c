#include <stdio.h>
#include "Pedometer.h"
#include "MPU6050.h"
#include "UART.h"
#include "LCD1602.h"

#include "DELAY.h"
#include "EEPROM.h"

static unsigned int step_cnt = 0;		//step_cnt为步数

//读取xyz数据存入均值滤波器，存满进行计算，滤波后样本存入sample（FILTER_CNT个数据的平均值）
static void filter_calculate(filter_avg_t *filter, axis_info_t *sample)
{
	unsigned int i;
	short x_sum = 0, y_sum = 0, z_sum = 0;
	for (i = 0; i < FILTER_CNT; i++)
	{
		x_sum += filter->info[i].x;
		y_sum += filter->info[i].y;
		z_sum += filter->info[i].z;
	}
	sample->x = x_sum / FILTER_CNT;
	sample->y = y_sum / FILTER_CNT;
	sample->z = z_sum / FILTER_CNT;
}

//初始化采样结构体的值
static void peak_value_init(peak_value_t *peak)
{
	peak->newmax.x=0;
	peak->newmax.y=0;
	peak->newmax.z=0;

	peak->newmin.x=2048;
	peak->newmin.y=2048;
	peak->newmin.z=2048;
}

//在动态阈值结构体初始化时，一定要将max的值都赋值为最小值，min赋值为最大值，这样才有利于动态更新。
static void peak_update(peak_value_t *peak, axis_info_t *cur_sample)
{
	static unsigned int sample_size = 0;
	sample_size++;
	if (sample_size > SAMPLE_SIZE)
	{
		/*采样达到50个，更新一次*/
		sample_size = 1;
		peak->oldmax = peak->newmax;
		peak->oldmin = peak->newmin;
		//初始化
		peak_value_init(peak);
	}
	peak->newmax.x = MAX(peak->newmax.x, cur_sample->x);
	peak->newmax.y = MAX(peak->newmax.y, cur_sample->y);
	peak->newmax.z = MAX(peak->newmax.z, cur_sample->z);

	peak->newmin.x = MIN(peak->newmin.x, cur_sample->x);
	peak->newmin.y = MIN(peak->newmin.y, cur_sample->y);
	peak->newmin.z = MIN(peak->newmin.z, cur_sample->z);
}

//更新数据
static char slid_update(slid_reg_t *slid, axis_info_t *cur_sample)
{
	char res = 0;
	if (ABS((cur_sample->x - slid->new_sample.x)) > DYNAMIC_PRECISION)
	{
		slid->old_sample.x = slid->new_sample.x;
		slid->new_sample.x = cur_sample->x;
		res = 1;
	}
	else
	{
		slid->old_sample.x = slid->new_sample.x;
	}
	if (ABS((cur_sample->y - slid->new_sample.y)) > DYNAMIC_PRECISION)
	{
		slid->old_sample.y = slid->new_sample.y;
		slid->new_sample.y = cur_sample->y;
		res = 1;
	}
	else
	{
		slid->old_sample.y = slid->new_sample.y;
	}

	if (ABS((cur_sample->z - slid->new_sample.z)) > DYNAMIC_PRECISION)
	{
		slid->old_sample.z = slid->new_sample.z;
		slid->new_sample.z = cur_sample->z;
		res = 1;
	}
	else
	{
		slid->old_sample.z = slid->new_sample.z;
	}
	return res;
}

/*判断当前最活跃轴*/
static char is_most_active(peak_value_t *peak)
{
	char res = MOST_ACTIVE_NULL;
	short x_change = ABS((peak->newmax.x - peak->newmin.x));
	short y_change = ABS((peak->newmax.y - peak->newmin.y));
	short z_change = ABS((peak->newmax.z - peak->newmin.z));

	if (x_change > y_change && x_change > z_change && x_change >= ACTIVE_PRECISION)
	{
		res = MOST_ACTIVE_X;
	}
	else if (y_change > x_change && y_change > z_change && y_change >= ACTIVE_PRECISION)
	{
		res = MOST_ACTIVE_Y;
	}
	else if (z_change > x_change && z_change > y_change && z_change >= ACTIVE_PRECISION)
	{
		res = MOST_ACTIVE_Z;
	}
	return res;
}

/*判断是否走步*/
static unsigned int detect_step(peak_value_t *peak, slid_reg_t *slid)
{
	char res = is_most_active(peak);
	switch (res)
	{
	case MOST_ACTIVE_NULL:
	{
		//fix
		break;
	}
	case MOST_ACTIVE_X:
	{
		short threshold_x = (peak->oldmax.x + peak->oldmin.x) / 2;
		if (slid->old_sample.x > threshold_x && slid->new_sample.x < threshold_x)
		{
			step_cnt++;
		}
		break;
	}
	case MOST_ACTIVE_Y:
	{
		short threshold_y = (peak->oldmax.y + peak->oldmin.y) / 2;
		if (slid->old_sample.y > threshold_y && slid->new_sample.y < threshold_y)
		{
			step_cnt++;
		}
		break;
	}
	case MOST_ACTIVE_Z:
	{
		short threshold_z = (peak->oldmax.z + peak->oldmin.z) / 2;
		if (slid->old_sample.z > threshold_z && slid->new_sample.z < threshold_z)
		{
			step_cnt++;
		}
		break;
	}
	default:
		break;
	}
	return step_cnt;
}

void get_step()
{
	uchar i;
	uint temp_step=0;
	//uint comdata[3];
	uint axis_x,axis_y,axis_z;
	axis_info_t xdata sample;
	filter_avg_t xdata filter;
	axis_info_t xdata cur_sample;
	peak_value_t xdata peak;
	slid_reg_t xdata slid;
	
	Init_MPU6050();		//MPU6050初始化
	UsartInit();		//串口初始化	
	LcdInit();			//LCD1602初始化
	init_display();	//LCD1602界面初始化
	
	//uint x,y,z;
	peak_value_init(&peak);		//初始化 peak_value_t xdata peak 结构体

	while (1)
	{
		for(i=0;i<4;i++)
		{
			step_cnt=EEPROM_Single_Read(0x00);
			GET_AXIS(&axis_x,&axis_y,&axis_z);			
			/*使x,y,z的值为三轴传感器的值
			cur_sample.x=x;
			cur_sample.y=y;
			cur_sample.z=z;
			*/
			cur_sample.x=axis_x;
			cur_sample.y=axis_y;
			cur_sample.z=axis_z;
			
			//平均值结构体数据存储
			filter.info[i].x=cur_sample.x;
			filter.info[i].y=cur_sample.y;
			filter.info[i].z=cur_sample.z;
		}
				
		filter_calculate(&filter,&sample);	//均值滤波
		peak_update(&peak,&sample);		//动态old,new的max，min值更新
		slid_update(&slid,&sample);		//数据更新

		temp_step=detect_step(&peak,&slid);
		
		Paper_aircraft(sample.x,sample.y,sample.z,temp_step);//串口数据上传
		EEPROM_Single_Write(0x00,step_cnt);
		display(temp_step);
	}
}

void INITEX0() interrupt 0
{
	int i;
	Delay5ms();
	if(k3==0)
	{
		led=~led;
	}
	EX_EEPROM_Single_Write(0x00,0);
	step_cnt=0;
	Delay5ms();
	LcdWriteCom(0x80+0x49);
	for(i=0;i<7;i++)
		LcdWriteData(0x20);	//清屏
}
