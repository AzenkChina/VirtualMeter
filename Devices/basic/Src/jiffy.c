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

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  ��ȡ��ǰ jiffy ��ֵ
  */
static uint32_t jiff_get_jiffies(void)
{
    return(__jiffy);
}

/**
  * @brief  ���� jiffy ��ʷֵ val �� ��ǰ __jiffy ��ʱ����λ����
  */
static uint32_t jiff_time_after(uint32_t val)
{
    if(__jiffy >= val)
    {
        return((uint32_t)(__jiffy - val));
    }
    else
    {
        return(((uint32_t)~(val - __jiffy)));
    }
}


/**
  * @brief jiffies �ӿ�
  */
const struct __jiffy jiffy = 
{
    .value              = jiff_get_jiffies,
    .after              = jiff_time_after,
};


/**
  * @brief  
  * 
  */
void jitter_update(uint16_t val)
{
    __jiffy += val;
}
