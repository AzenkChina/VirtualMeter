/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TYPES_CALENDAR_H__
#define __TYPES_CALENDAR_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/**
 *  12�� ��/�� ��12mon*1week byte��
 *  12�� �� ��16week*7day byte��
 *  28�� �� ��28day*(12*(2time+1rate)) byte��
 *
 *  �ڼ��ձ�
 *  ����ʱ�α�
 **/
#define CALENDER_MAX_MONTH                  (12)        //��/�� �� ������1~12��
#define CALENDER_MAX_WEEK                   (12)        //�� �� ������1~24��
#define CALENDER_MAX_DAY                    (28)        //�� �� ������1~31��
#define CALENDER_MAX_DAY_POINT              (12)         //�� �� ÿ��������ʱ������1~24��
#define CALENDER_MAX_FESTIVAL               (100)        //����������1~100��
#define CALENDER_MAX_INTERVAL               (50)        //�������ʱ������1~24��

/**
 * @brief		
 **/
struct __festival_entry
{
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t selection;
};

/**
 * @brief		�ڼ��� �� ���ݽṹ
 **/
struct __calender_festival_table
{
    uint8_t activated;
    struct __festival_entry table[CALENDER_MAX_FESTIVAL];
};


/**
 * @brief		��/�� �� ����Ԫ��
 **/
struct __month_entry
{
    uint8_t month;
    uint8_t day;
    uint8_t selection;
};

/**
 * @brief		��/�� �� ���ݽṹ
 **/
struct __calender_month_table
{
    uint8_t activated;
    struct __month_entry table[CALENDER_MAX_MONTH];
};


/**
 * @brief		�� �� ����Ԫ��
 **/
struct __week_entry
{
    uint8_t sun;
    uint8_t mon;
    uint8_t tues;
    uint8_t wed;
    uint8_t thur;
    uint8_t fri;
    uint8_t sat;
};

/**
 * @brief		�� �� ���ݽṹ
 **/
struct __calender_week_table
{
    uint8_t activated;
    struct __week_entry table[CALENDER_MAX_WEEK];
};


/**
 * @brief		�� �� ����Ԫ��1
 **/
struct __day_entry_point
{
    uint8_t hour;
    uint8_t min;
    uint8_t selection;
};

/**
 * @brief		�� �� ����Ԫ��2
 **/
struct __day_entry
{
    struct __day_entry_point point[CALENDER_MAX_DAY_POINT];
};

/**
 * @brief		�� �� ���ݽṹ
 **/
struct __calender_day_table
{
    uint8_t activated;
    struct __day_entry table[CALENDER_MAX_DAY];
};





/**
 * @brief		������ �� ���ݽṹ
 **/
struct __calender_table
{
    struct __calender_month_table       month;
    struct __calender_week_table        week;
    struct __calender_day_table         day;
    struct __calender_festival_table    festival;
};






/**
  * @brief  ������ �������
  */
enum __calender_res
{
    CALENDER_SUCCESS = 0,
    CALENDER_ERR_INDEX, //��������
    CALENDER_ERR_GROUP, //�����
    CALENDER_ERR_RELATION, //ǰ������˳�����
    CALENDER_ERR_DATE, //���ڴ���
    CALENDER_ERR_TIME, //ʱ�����
    
    CALENDER_ERR_NODEF = 0xff, //δ֪����
    
};

/**
 * @brief		������ �����ӿ�
 **/
struct __calender_interface
{
    struct
    {
        struct
        {
            uint8_t                 (*get_amount)(void); //��ȡ��Ч����
            uint8_t                 (*set_amount)(uint8_t amount); //������Ч����
            enum __calender_res     (*get_entry)(uint8_t index, struct __festival_entry *val); //��ȡ����
            enum __calender_res     (*set_entry)(uint8_t index, const struct __festival_entry *val); //��ȡ����
            enum __calender_res     (*clear)(void); //���ȫ��������
            
        }                           special;
        struct
        {
            uint8_t                 (*get_amount)(void); //��ȡ��Ч����
            uint8_t                 (*set_amount)(uint8_t amount); //������Ч����
            enum __calender_res     (*get_entry)(uint8_t index, struct __month_entry *val); //��ȡ����
            enum __calender_res     (*set_entry)(uint8_t index, const struct __month_entry *val); //����һ��������
            enum __calender_res     (*clear)(void); //���ȫ��������
            
        }                           month;
        struct
        {
            uint8_t                 (*get_amount)(void); //��ȡ��Ч����
            uint8_t                 (*set_amount)(uint8_t amount); //������Ч����
            enum __calender_res     (*get_entry)(uint8_t index, struct __week_entry *val); //��ȡ����
            enum __calender_res     (*set_entry)(uint8_t index, const struct __week_entry *val); //����һ��������
            enum __calender_res     (*clear)(void); //���ȫ��������
            
        }                           week;
        struct
        {
            uint8_t                 (*get_amount)(void); //��ȡ��Ч����
            uint8_t                 (*set_amount)(uint8_t amount); //������Ч����
            enum __calender_res     (*get_entry)(uint8_t index, struct __day_entry *val); //��ȡ����
            enum __calender_res     (*set_entry)(uint8_t index, const struct __day_entry *val); //����һ��������
            enum __calender_res     (*clear)(void); //���ȫ��������
            
        }                           day;
        
    }                               passive;
    struct
    {
        struct
        {
            uint8_t                 (*get_amount)(void); //��ȡ��Ч����
            enum __calender_res     (*get_entry)(uint8_t index, struct __festival_entry *val); //��ȡ����
            
        }                           special;
        struct
        {
            uint8_t                 (*get_amount)(void); //��ȡ��Ч����
            enum __calender_res     (*get_entry)(uint8_t index, struct __month_entry *val); //��ȡ����
            
        }                           month;
        struct
        {
            uint8_t                 (*get_amount)(void); //��ȡ��Ч����
            enum __calender_res     (*get_entry)(uint8_t index, struct __week_entry *val); //��ȡ����
            
        }                           week;
        struct
        {
            uint8_t                 (*get_amount)(void); //��ȡ��Ч����
            enum __calender_res     (*get_entry)(uint8_t index, struct __day_entry *val); //��ȡ����
            
        }                           day;
        
        uint8_t                     (*rate)(void); //��ǰ����
        
    }                               active;
    struct
    {
        enum __calender_res         (*sunrise)(uint8_t index, struct __day_entry *val); //NG
        enum __calender_res         (*sunset)(uint8_t index, struct __day_entry *val); //NG
        enum __calender_res         (*enable_saving)(void); //
        enum __calender_res         (*disable_saving)(void); //
        
    }                               daylight;
    struct
    {
        struct
        {
            enum __calender_res     (*get)(uint8_t index, struct __day_entry *val); //NG
            enum __calender_res     (*set)(uint8_t index, struct __day_entry *val); //NG
            enum __calender_res     (*tostamp)(uint8_t index, struct __day_entry *val); //NG
            
        }                           time;
        struct
        {
            enum __calender_res     (*get)(uint8_t index, struct __week_entry *val); //NG
            enum __calender_res     (*set)(uint8_t index, struct __day_entry *val); //NG
            enum __calender_res     (*totime)(uint8_t index, struct __day_entry *val); //NG
            
        }                           stamp;
        
    }                               real;
};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif /* __TYPES_CALENDAR_H__ */
