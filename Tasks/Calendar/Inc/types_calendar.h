/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TYPES_CALENDAR_H__
#define __TYPES_CALENDAR_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/**
 *  12个 月/季 表（12mon*1week byte）
 *  12个 周 表（16week*7day byte）
 *  28个 天 表（28day*(12*(2time+1rate)) byte）
 *
 *  节假日表
 *  特殊时段表
 **/
#define CALENDER_MAX_MONTH                  (12)        //月/季 表 个数（1~12）
#define CALENDER_MAX_WEEK                   (12)        //周 表 个数（1~24）
#define CALENDER_MAX_DAY                    (28)        //天 表 个数（1~31）
#define CALENDER_MAX_DAY_POINT              (12)         //天 表 每天最大费率时段数（1~24）
#define CALENDER_MAX_FESTIVAL               (100)        //最大假日数（1~100）
#define CALENDER_MAX_INTERVAL               (50)        //最大特殊时段数（1~24）

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
 * @brief		节假日 表 数据结构
 **/
struct __calender_festival_table
{
    uint8_t activated;
    struct __festival_entry table[CALENDER_MAX_FESTIVAL];
};


/**
 * @brief		月/季 表 数据元素
 **/
struct __month_entry
{
    uint8_t month;
    uint8_t day;
    uint8_t selection;
};

/**
 * @brief		月/季 表 数据结构
 **/
struct __calender_month_table
{
    uint8_t activated;
    struct __month_entry table[CALENDER_MAX_MONTH];
};


/**
 * @brief		周 表 数据元素
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
 * @brief		周 表 数据结构
 **/
struct __calender_week_table
{
    uint8_t activated;
    struct __week_entry table[CALENDER_MAX_WEEK];
};


/**
 * @brief		天 表 数据元素1
 **/
struct __day_entry_point
{
    uint8_t hour;
    uint8_t min;
    uint8_t selection;
};

/**
 * @brief		天 表 数据元素2
 **/
struct __day_entry
{
    struct __day_entry_point point[CALENDER_MAX_DAY_POINT];
};

/**
 * @brief		天 表 数据结构
 **/
struct __calender_day_table
{
    uint8_t activated;
    struct __day_entry table[CALENDER_MAX_DAY];
};





/**
 * @brief		日历表 表 数据结构
 **/
struct __calender_table
{
    struct __calender_month_table       month;
    struct __calender_week_table        week;
    struct __calender_day_table         day;
    struct __calender_festival_table    festival;
};






/**
  * @brief  日历表 操作结果
  */
enum __calender_res
{
    CALENDER_SUCCESS = 0,
    CALENDER_ERR_INDEX, //索引错误
    CALENDER_ERR_GROUP, //组错误
    CALENDER_ERR_RELATION, //前后数据顺序错误
    CALENDER_ERR_DATE, //日期错误
    CALENDER_ERR_TIME, //时间错误
    
    CALENDER_ERR_NODEF = 0xff, //未知错误
    
};

/**
 * @brief		日历表 操作接口
 **/
struct __calender_interface
{
    struct
    {
        struct
        {
            uint8_t                 (*get_amount)(void); //获取有效个数
            uint8_t                 (*set_amount)(uint8_t amount); //设置有效个数
            enum __calender_res     (*get_entry)(uint8_t index, struct __festival_entry *val); //获取数据
            enum __calender_res     (*set_entry)(uint8_t index, const struct __festival_entry *val); //获取数据
            enum __calender_res     (*clear)(void); //清除全部数据项
            
        }                           special;
        struct
        {
            uint8_t                 (*get_amount)(void); //获取有效个数
            uint8_t                 (*set_amount)(uint8_t amount); //设置有效个数
            enum __calender_res     (*get_entry)(uint8_t index, struct __month_entry *val); //获取数据
            enum __calender_res     (*set_entry)(uint8_t index, const struct __month_entry *val); //设置一个数据项
            enum __calender_res     (*clear)(void); //清除全部数据项
            
        }                           month;
        struct
        {
            uint8_t                 (*get_amount)(void); //获取有效个数
            uint8_t                 (*set_amount)(uint8_t amount); //设置有效个数
            enum __calender_res     (*get_entry)(uint8_t index, struct __week_entry *val); //获取数据
            enum __calender_res     (*set_entry)(uint8_t index, const struct __week_entry *val); //设置一个数据项
            enum __calender_res     (*clear)(void); //清除全部数据项
            
        }                           week;
        struct
        {
            uint8_t                 (*get_amount)(void); //获取有效个数
            uint8_t                 (*set_amount)(uint8_t amount); //设置有效个数
            enum __calender_res     (*get_entry)(uint8_t index, struct __day_entry *val); //获取数据
            enum __calender_res     (*set_entry)(uint8_t index, const struct __day_entry *val); //设置一个数据项
            enum __calender_res     (*clear)(void); //清除全部数据项
            
        }                           day;
        
    }                               passive;
    struct
    {
        struct
        {
            uint8_t                 (*get_amount)(void); //获取有效个数
            enum __calender_res     (*get_entry)(uint8_t index, struct __festival_entry *val); //获取数据
            
        }                           special;
        struct
        {
            uint8_t                 (*get_amount)(void); //获取有效个数
            enum __calender_res     (*get_entry)(uint8_t index, struct __month_entry *val); //获取数据
            
        }                           month;
        struct
        {
            uint8_t                 (*get_amount)(void); //获取有效个数
            enum __calender_res     (*get_entry)(uint8_t index, struct __week_entry *val); //获取数据
            
        }                           week;
        struct
        {
            uint8_t                 (*get_amount)(void); //获取有效个数
            enum __calender_res     (*get_entry)(uint8_t index, struct __day_entry *val); //获取数据
            
        }                           day;
        
        uint8_t                     (*rate)(void); //当前费率
        
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
