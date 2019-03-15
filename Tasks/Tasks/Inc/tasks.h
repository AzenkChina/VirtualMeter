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
  * @brief  任务调度当前状态
  */
enum  __sched_status
{
    SCHED_NODEF = 0,//应用未定义
    SCHED_RUNNING = 0x0f,//正常调度（loop正常轮询）
    SCHED_HANGED = 0xf0,//调度被挂起（loop不轮询）
};

/**
  * @brief  任务调度控制接口
  */
struct __task_ctrl
{
    enum  __sched_status            (*hang)(const char *name);
	enum  __sched_status            (*reset)(const char *name);
    enum  __sched_status            (*run)(const char *name);
    enum  __sched_status            (*status)(const char *name);
};

/**
  * @brief  任务信息接口
  */
struct __task_trace
{
    uint8_t                         (*amount)(void);
    const struct __task_sched       *(*current)(void);
    const struct __task_sched       *(*belong)(void);
    uint8_t                         (*exist)(const char *name);
    const char                      *(*info)(uint8_t index);
};

/* Exported constants --------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern const struct __task_ctrl task_ctrl;
extern const struct __task_trace task_trace;

#endif /* __TASKS_H__ */
