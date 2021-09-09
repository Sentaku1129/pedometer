/*file pedometer.h*/

#ifndef __PEDOMETER_H_
#define __PEDOMETER_H_

#define FILTER_CNT 4

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define SAMPLE_SIZE 50

#define ABS(a) (0 - (a)) > 0 ? (-(a)) : (a)
#define DYNAMIC_PRECISION 80 /*��̬����*/

#define MOST_ACTIVE_NULL 0	/*δ�ҵ����Ծ��*/
#define MOST_ACTIVE_X 1		/*���Ծ��X*/
#define MOST_ACTIVE_Y 2		/*���Ծ��Y*/
#define MOST_ACTIVE_Z 3		/*���Ծ��Z*/
#define ACTIVE_PRECISION 100 /*��Ծ����С�仯ֵ*/

//��������
typedef struct
{
	short x;
	short y;
	short z;
} axis_info_t;

//ƽ��ֵ�ṹ
typedef struct filter_avg
{
	axis_info_t info[FILTER_CNT];
	unsigned char count;
} filter_avg_t;

//�¾�������ֵ���ݽṹ
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

/*һ��������λ�Ĵ��������ڹ��˸�Ƶ����*/
typedef struct slid_reg
{
	axis_info_t new_sample;
	axis_info_t old_sample;
} slid_reg_t;



//��ȡxyz���ݴ����ֵ�˲������������м��㣬�˲�����������sample��FILTER_CNT�����ݵ�ƽ��ֵ��
static void filter_calculate(filter_avg_t *filter, axis_info_t *sample) ;
//��ʼ�������ṹ���ֵ
static void peak_value_init(peak_value_t *peak);
//�ڶ�̬��ֵ�ṹ���ʼ��ʱ��һ��Ҫ��max��ֵ����ֵΪ��Сֵ��min��ֵΪ���ֵ�������������ڶ�̬���¡�
static void peak_update(peak_value_t *peak, axis_info_t *cur_sample);
//��������
static char slid_update(slid_reg_t *slid, axis_info_t *cur_sample);
//�жϵ�ǰ���Ծ��
static char is_most_active(peak_value_t *peak);
//�ж��Ƿ��߲�
static unsigned int detect_step(peak_value_t *peak, slid_reg_t *slid);
//�õ�����
void get_step();

#endif