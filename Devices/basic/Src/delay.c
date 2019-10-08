/**
 * @brief		
 * @details		
 * @date		2016-11-15
 **/

/* Includes ------------------------------------------------------------------*/
#include "delay.h"
#include "cpu.h"

#if defined ( _WIN32 ) || defined ( _WIN64 )
#include <windows.h>
#elif defined ( __linux )
#include <unistd.h>
#else
#if defined (DEMO_STM32F091)
#include "stm32f0xx.h"
#endif
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Œ¢√Î—” ±∫Ø ˝
  */
void udelay(uint16_t count)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    ;
#else

#if defined (DEMO_STM32F091)
	uint16_t loop;
    
	if(cpu.core.status() == CPU_NORMAL)
	{
        for(loop=0; loop<count; loop++)
        {
            __NOP();
            __NOP();
            __NOP();
            __NOP();
            __NOP();
        }
	}
	else
	{
        for(loop=0; loop<count; loop++)
        {
            __NOP();
            __NOP();
        }
	}
#endif

#endif
}

/**
  * @brief  ∫¡√Î—” ±∫Ø ˝
  */
void mdelay(uint16_t count)
{
#if defined ( _WIN32 ) || defined ( _WIN64 )
	Sleep(count);
#elif defined ( __linux )
	usleep(count*1000);
#else

#if defined (DEMO_STM32F091)
	uint32_t cnt;
	
	if(cpu.core.status() == CPU_NORMAL)
	{
		while(count --)
		{
			for(cnt=0; cnt<2894; cnt++);
		}
	}
	else
	{
		while(count --)
		{
			for(cnt=0; cnt<3*2894; cnt++);
		}
	}
#endif

#endif
}

