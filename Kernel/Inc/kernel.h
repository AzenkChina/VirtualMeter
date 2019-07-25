/**
 * @brief		
 * @details		
 * @date		azenk@2016-11-15
 **/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __KERNEL_H__
#define __KERNEL_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  内核运行等级
  */
enum __klevel
{
    SYSTEM_BOOT = 0,//系统启动时的初始状态，运行时不应该有此状态
    
    SYSTEM_SLEEP = 0x30,//系统进入掉电状态
    SYSTEM_WAKEUP = 0x3f,//掉电的子状态，由power驱动控制进入此状态
    
    SYSTEM_RUN = 0xc0,//系统正常运行状态
    SYSTEM_REBOOT = 0xcf,//正常运行的子状态，由power驱动控制进入此状态
    
    SYSTEM_RESET = 0xf0,//系统重置
};

/* Exported define ------------------------------------------------------------*/
//app轮询频率，取值空间为5~1000，尽可能能被1000整除
#define KERNEL_LOOP_FREQ                ((uint16_t)25)
//休眠唤醒间隔时间，单位毫秒
#define KERNEL_LOOP_SLEEPED             ((uint32_t)1000)
//非休眠状态下task里 loop 函数的轮询周期
#define PERIOD_RUNNING                  ((uint32_t)((KERNEL_LOOP_FREQ >= 10 && KERNEL_LOOP_FREQ <= 1000)? (1000 / KERNEL_LOOP_FREQ) : 10))

/* Exported macro -------------------------------------------------------------*/
//task里当前 loop 函数的轮询周期
//task中可以使用该宏来做简单的计时用
#define KERNEL_PERIOD                   ((system_status() == SYSTEM_SLEEP)? KERNEL_LOOP_SLEEPED:PERIOD_RUNNING)

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern enum __klevel system_status(void);
extern uint16_t system_usage(void);

#endif /* __KERNEL_H__ */
