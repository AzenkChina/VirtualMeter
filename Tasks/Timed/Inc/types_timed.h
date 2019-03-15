/**
 * @brief		
 * @details		
 * @date		azenk@2017-01-09
 **/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TYPES_TIMED_H__
#define __TYPES_TIMED_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  timed task 配置
  */
enum __timd_status
{
	TIMD_SUCCESS = 0,
	TIMD_CONFLICT,
	TIMD_NOEXIST,
};

/**
  * @brief  timed task 配置
  */
struct __timed_conf
{
    void                            (*callback)(void); //定时时间到回调函数
    uint64_t						period; //定时周期
    uint16_t                        flags; //标志位
};

/**
  * @brief  timed task 的对外接口
  */
struct __timed
{
    enum __timd_status				(*create)(const struct __timed_conf *conf); //创建一个定时器任务
    enum __timd_status				(*query)(const struct __timed_conf *conf); //检查任务有效性
    enum __timd_status				(*remove)(const struct __timed_conf *conf); //销毁一个定时器任务
    enum __timd_status				(*empty)(void); //清空所有定时器任务
};

/* Exported constants --------------------------------------------------------*/
#define TIMD_FLAG_RELATIVE      ((uint16_t)(0x0000 << 0)) //使用相对时间
#define TIMD_FLAG_ABSOLUTE      ((uint16_t)(0x0003 << 0)) //使用绝对时间

#define TIMD_FLAG_CHANGE        ((uint16_t)(0x0000 << 2)) //系统时钟被修改执行
#define TIMD_FLAG_PERIODIC      ((uint16_t)(0x0001 << 2)) //周期执行
#define TIMD_FLAG_ONCE          ((uint16_t)(0x0003 << 2)) //执行一次

/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif /* __TYPES_TIMED_H__ */
