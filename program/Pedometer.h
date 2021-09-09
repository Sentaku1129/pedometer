/*file pedometer.h*/

#ifndef __PEDOMETER_H_
#define __PEDOMETER_H_

#define FILTER_CNT 4

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define SAMPLE_SIZE 50

#define ABS(a) (0 - (a)) > 0 ? (-(a)) : (a)
#define DYNAMIC_PRECISION 80 /*动态精度*/

#define MOST_ACTIVE_NULL 0	/*未找到最活跃轴*/
#define MOST_ACTIVE_X 1		/*最活跃轴X*/
#define MOST_ACTIVE_Y 2		/*最活跃轴Y*/
#define MOST_ACTIVE_Z 3		/*最活跃轴Z*/
#define ACTIVE_PRECISION 100 /*活跃轴最小变化值*/

//三轴数据
typedef struct
{
	short x;
	short y;
	short z;
} axis_info_t;

//平均值结构
typedef struct filter_avg
{
	axis_info_t info[FILTER_CNT];
	unsigned char count;
} filter_avg_t;

//新旧三轴最值数据结构
typedef struct
{
	/*
        typedef struct {
        short x;
        short y;
        short z;
        }axis_info_t;
    */
	axis_info_t newmax;
	axis_info_t newmin;
	axis_info_t oldmax;
	axis_info_t oldmin;
} peak_value_t;

/*一个线性移位寄存器，用于过滤高频噪声*/
typedef struct slid_reg
{
	axis_info_t new_sample;
	axis_info_t old_sample;
} slid_reg_t;



//读取xyz数据存入均值滤波器，存满进行计算，滤波后样本存入sample（FILTER_CNT个数据的平均值）
static void filter_calculate(filter_avg_t *filter, axis_info_t *sample) ;
//初始化采样结构体的值
static void peak_value_init(peak_value_t *peak);
//在动态阈值结构体初始化时，一定要将max的值都赋值为最小值，min赋值为最大值，这样才有利于动态更新。
static void peak_update(peak_value_t *peak, axis_info_t *cur_sample);
//更新数据
static char slid_update(slid_reg_t *slid, axis_info_t *cur_sample);
//判断当前最活跃轴
static char is_most_active(peak_value_t *peak);
//判断是否走步
static unsigned int detect_step(peak_value_t *peak, slid_reg_t *slid);
//得到步数
void get_step();

#endif