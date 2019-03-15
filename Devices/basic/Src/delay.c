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
#include "stm32f0xx.h"
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
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux )
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
#else
	;
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
	uint32_t cnt;
	
	if(cpu.core.status() == CPU_NORMAL)
	{
		while(count --)
		{
			for(cnt=0; cnt<30000; cnt++);
		}
	}
	else
	{
		while(count --)
		{
			for(cnt=0; cnt<300000; cnt++);
		}
	}
#endif
}

