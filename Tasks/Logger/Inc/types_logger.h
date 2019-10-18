/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TYPES_LOGGER_H__
#define __TYPES_LOGGER_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  事件状态
  */
enum __event_status
{
    EVES_END = 0, //事件已结束
    EVES_ONGONG = 1, //事件正在发生
    EVES_PENDING = 2, //事件正在计时中（等待发生或者结束）
};

/**
  * @brief  事件标识
  */
enum __event_id
{
    EVEI_ALL = 0, //事件ID开始
    
    EVEI_POWERUP = 1, //上电事件
    EVEI_POWERDOWN = 2, //掉电事件
    
    //...添加其它事件
    
    EVEI_MAX = 512, //事件ID上限（名称不要随意修改）
};

/**
  * @brief  logger task 的对外接口
  */
struct __logger
{
    enum __event_status             (*status)(enum __event_id id); //读取事件状态
    uint32_t                        (*amount)(enum __event_id id); //日志条数
    uint32_t                        (*clear)(enum __event_id id); //日志清空
};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
extern void __log_trrgger(enum __event_id id);

/* Exported function prototypes ----------------------------------------------*/
#define LOG(e)       __log_trrgger(e)

#endif /* __API_LOGGER_H__ */
