/**
 * @brief		
 * @details		
 * @date		azenk@2017-01-09
 **/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TASKS_H__
#define __TASKS_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported macro ------------------------------------------------------------*/
#define __TASKS_MONITOR             //决定是否启用task监测

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  任务当前状态
  */
enum  __task_status
{
    TASK_NOTINIT = 0, //应用未初始化
    TASK_INIT, //应用已初始化
    TASK_RUN, //应用正在运行
    TASK_SUSPEND, //应用已经挂起
    
    TASK_ERROR = 0x0f, //应用出错
};

/**
  * @brief  任务模型调度单元，每种应用模型中的第一个结构必须是 任务模型调度单元
  */
struct __task_sched
{
    const char                      *name;
    
    void                            (*init)(void); //应用初始化
    void                            (*loop)(void); //应用的后台轮询调用
    void                            (*exit)(void); //应用退出
    void                            (*reset)(void); //应用恢复默认值(出厂初始化)
    enum            __task_status   (*status)(void); //应用当前状态
    
    void                            *api;
};

/**
  * @brief  任务控制信息接口
  */
struct __task_ctrl
{
    uint8_t							(*amount)(void); //应用数量
    const char						*(*info)(uint8_t index); //应用信息
    uint8_t							(*index)(const char *name); //应用索引
    const struct __task_sched		*(*current)(void); //当前应用
    const struct __task_sched		*(*search)(const char *name); //根据名称查找应用
	uint8_t							(*reset)(const char *name); //根据名称复位应用
};

/* Exported constants --------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern const struct __task_ctrl task_ctrl;

#endif /* __TASKS_H__ */
