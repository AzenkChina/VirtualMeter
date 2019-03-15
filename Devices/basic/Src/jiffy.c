/**
 * @brief		
 * @details		
 * @date		2016-11-15
 **/

/* Includes ------------------------------------------------------------------*/
#include "jiffy.h"
#include "cpu.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint32_t __jiffy = 0;
static void (*intr_hook)(uint16_t val) = (void *)0;
static uint32_t hook_check = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  获取当前 jiffy 的值
  */
static uint32_t jiff_get_jiffies(void)
{
    return(__jiffy);
}

/**
  * @brief  计算 jiffy 历史值 val 与 当前 __jiffy 的时间差，单位毫秒
  */
static uint32_t jiff_time_after(uint32_t val)
{
    if(__jiffy >= val)
    {
        return(__jiffy - val);
    }
    else
    {
        return(val - __jiffy);
    }
}

/**
  * @brief  添加中断钩子函数
  */
static uint32_t jiff_hook_intr(void (*hook)(uint16_t val))
{
    hook_check = 0;
    enum __interrupt_status intrs = cpu.interrupt.status();

    if(intrs == INTR_ENABLED)
    {
        cpu.interrupt.disable();
    }
    
    intr_hook = hook;
    
    if(intrs == INTR_ENABLED)
    {
        cpu.interrupt.enable();
    }
    
    if(intr_hook)
    {

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#endif

	hook_check = ((uint32_t)(~(uint32_t)(intr_hook)));

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

    }
    
    return(hook_check);
}


/**
  * @brief jiffies 接口
  */
const struct __jiffy jiffy = 
{
    .value              = jiff_get_jiffies,
    .after              = jiff_time_after,
    .hook_intr          = jiff_hook_intr,
};


/**
  * @brief  
  * 
  */
void jitter_update(uint16_t val)
{
    __jiffy += val;
#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"
#endif

    if(intr_hook && (hook_check = ((uint32_t)(~(uint32_t)(intr_hook)))))
    {
        intr_hook(val);
    }

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif
}
