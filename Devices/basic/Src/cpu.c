/**
 * @brief
 * @details
 * @date        2016-11-15
 **/

/* Includes ------------------------------------------------------------------*/
#include "string.h"
#include "kernel.h"
#include "cpu.h"
#include "trace.h"

#if defined ( _WIN32 ) || defined ( _WIN64 )
#include <windows.h>
#include "jiffy.h"
#elif defined ( __linux )
#include <pthread.h>
#include <unistd.h>
#include "stdlib.h"
#include "jiffy.h"
#else
#if defined (STM32F091)
#include "stm32f0xx.h"
#endif
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static enum __interrupt_status intr_status = INTR_ENABLED;
static enum __cpu_level cpu_level = CPU_NORMAL;

#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
static volatile uint8_t counter = 0;
static volatile uint8_t in_sleep = 0;
char *proc_self = (char *)0;
#endif

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )

#if defined ( __linux )
static void *ThreadTick(void *arg)
#else
static DWORD CALLBACK ThreadTick(PVOID pvoid)
#endif
{
	while(1)
	{
	    if(!in_sleep)
	    {
#if defined ( __linux )
            usleep(10*1000);
#else
            Sleep(10);
#endif
			if(intr_status == INTR_ENABLED)
			{
				jitter_update(10);
			}
	    }
	    else
	    {
#if defined ( __linux )
            usleep(KERNEL_LOOP_SLEEPED*1000);
#else
            Sleep(KERNEL_LOOP_SLEEPED);
#endif
			in_sleep = 0;
			
			if(intr_status == INTR_ENABLED)
			{
				jitter_update(KERNEL_LOOP_SLEEPED);
			}
	    }
	}
}

#if defined ( __linux )
static void *ThreadDog(void *arg)
#else
static DWORD CALLBACK ThreadDog(PVOID pvoid)
#endif
{
	while(1)
	{
		if(counter > 25)
		{
			TRACE(TRACE_ERR, "Watchdog timeout.");
			exit(0);
		}
		
#if defined ( __linux )
		usleep(200*1000);
#else
		Sleep(200);
#endif
		counter ++;
	}
}

#endif

/**
  * @brief  cpu 信息
  */
static const char* cpu_core_details(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 )
    static const char details[] = "x86 windows";
#elif defined ( __linux )
    static const char details[] = "x86 linux";
#else

#if defined (STM32F091)
    static const char details[] = "stm32f091 arm cortex-m0 thumb2";
#endif

#endif
    return(details);
}

/**
  * @brief  初始化内核
  */
static enum __cpu_endian cpu_core_endian(void)
{
    union
    {
        uint32_t i;
        uint8_t c;
    } u;

    u.i = 0x55FFFFAA;

    if(u.c == 0x55)
    {
        return(CPU_BIG_ENDIAN);
    }
    else if(u.c == 0xAA)
    {
        return(CPU_LITTLE_ENDIAN);
    }

    return(CPU_UNKNOWN_ENDIAN);
}

/**
  * @brief  内核重启
  */
static void cpu_core_reset(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	if(!proc_self)
	{
		TRACE(TRACE_ERR, "Program command damaged.");
		exit(0);
	}
    
#if defined ( __linux )
    execl(proc_self, proc_self, "reboot", NULL);
#else
    ShellExecute(NULL, "open", proc_self, "reboot", NULL, SW_SHOW);
#endif
    exit(0);
#else

#if defined (STM32F091)
    NVIC_SystemReset();
#endif

#endif
	TRACE(TRACE_ERR, "Rebooting faild.");
}

/**
  * @brief  内核休眠
  */
static void cpu_core_sleep(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	in_sleep = 0xff;
	
	while(in_sleep)
	{
#if defined ( __linux )
		usleep(10*1000);
#else
		Sleep(10);
#endif
	}
#else

#if defined (STM32F091)
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
#endif

#endif
}

/**
  * @brief  重置看门狗
  */
static void watchdog_feed(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    counter = 0;
#else

#if defined (STM32F091)
    IWDG_ReloadCounter();
#endif

#endif
}

/**
  * @brief  禁用全局中断
  */
static void cpu_entr_disable(void)
{
    intr_status = INTR_DISABLED;
#if defined (STM32F091)
    __disable_irq();
#endif
}

/**
  * @brief  使能全局中断
  */
static void cpu_entr_enable(void)
{
    intr_status = INTR_ENABLED;
#if defined (STM32F091)
    __enable_irq();
#endif
}

/**
  * @brief  全局中断开关状态
  */
static enum __interrupt_status cpu_entr_status(void)
{
    return(intr_status);
}

/**
  * @brief  初始化内核
  */
static void cpu_core_init(enum __cpu_level level)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	static uint8_t flag = 0;
#if defined ( __linux )
	pthread_t thread;
	pthread_attr_t thread_attr;
#else
	HANDLE hThread;
#endif
	
    if(level == CPU_NORMAL)
    {
        cpu_level = CPU_NORMAL;
    }
    else
    {
        cpu_level = CPU_POWERSAVE;
    }
	
    if(!flag)
    {
#if defined ( __linux )
		pthread_attr_init(&thread_attr);
		pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&thread, &thread_attr, ThreadTick, NULL);
		pthread_attr_destroy(&thread_attr);
		
		pthread_attr_init(&thread_attr);
		pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&thread, &thread_attr, ThreadDog, NULL);
		pthread_attr_destroy(&thread_attr);
#else
		hThread = CreateThread(NULL, 0, ThreadTick, 0, 0, NULL);
		CloseHandle(hThread);
		
		hThread = CreateThread(NULL, 0, ThreadDog, 0, 0, NULL);
		CloseHandle(hThread);
#endif
        flag = 0xff;
    }
#else

#if defined (STM32F091)
    GPIO_InitTypeDef GPIO_InitStruct;
    RTC_InitTypeDef RTC_InitStruct;
    RTC_TimeTypeDef RTC_TimeStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    
    enum __interrupt_status intrs = cpu_entr_status();
    
    if(intrs == INTR_ENABLED)
    {
        cpu_entr_disable();
    }
    
    /* Set HSION bit */
    RCC->CR |= (uint32_t)0x00000001;
    /* Reset SW[1:0], HPRE[3:0], PPRE[2:0], ADCPRE, MCOSEL[2:0], MCOPRE[2:0] and PLLNODIV bits */
    RCC->CFGR &= (uint32_t)0x08FFB80C;
    /* Reset HSEON, CSSON and PLLON bits */
    RCC->CR &= (uint32_t)0xFEF6FFFF;
    /* Reset HSEBYP bit */
    RCC->CR &= (uint32_t)0xFFFBFFFF;
    /* Reset PLLSRC, PLLXTPRE and PLLMUL[3:0] bits */
    RCC->CFGR &= (uint32_t)0xFFC0FFFF;
    /* Reset PREDIV1[3:0] bits */
    RCC->CFGR2 &= (uint32_t)0xFFFFFFF0;
    /* Reset USARTSW[1:0], I2CSW, CECSW and ADCSW bits */
    RCC->CFGR3 &= (uint32_t)0xFFFFFEAC;
    /* Reset HSI14 bit */
    RCC->CR2 &= (uint32_t)0xFFFFFFFE;
    /* Reset HSI48 bit */
    RCC->CR2 &= (uint32_t)0xFFFEFFFF;
    /* Disable all interrupts */
    RCC->CIR = 0x00000000;
    /* Disable Prefetch Buffer and Flash Latency */
    FLASH->ACR = 0x00000000;
    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
    /* PCLK = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE_DIV1;
     //Disable SysTick
    SysTick->CTRL = 0x00000000;
    SysTick->VAL  = 0x00000000;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_DBGMCU, ENABLE);
    DBGMCU_APB1PeriphConfig(DBGMCU_IWDG_STOP, ENABLE);
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_256);
    IWDG_SetReload(40000 / 32);
    IWDG_ReloadCounter();
    IWDG_Enable();
    
    if(level == CPU_NORMAL)
    {
        /* Configure the System clock frequency, AHB/APBx prescalers and Flash settings */
        __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
        /* SYSCLK, HCLK, PCLK configuration ----------------------------------------*/
        /* Enable HSE */
        RCC->CR |= ((uint32_t)RCC_CR_HSEON);

        /* Wait till HSE is ready and if Time out is reached exit */
        do
        {
            HSEStatus = RCC->CR & RCC_CR_HSERDY;
            StartUpCounter++;
        }
        while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

        if((RCC->CR & RCC_CR_HSERDY) != RESET)
        {
            HSEStatus = (uint32_t)0x01;
        }
        else
        {
            HSEStatus = (uint32_t)0x00;
        }

        if(HSEStatus == (uint32_t)0x01)
        {
            /* PLL configuration = HSE * 3 = 24 MHz */
            RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE_PREDIV | RCC_CFGR_PLLMULL3);
            /* Enable PLL */
            RCC->CR |= RCC_CR_PLLON;

            /* Wait till PLL is ready */
            while((RCC->CR & RCC_CR_PLLRDY) == 0)
            {
            }

            /* Select PLL as system clock source */
            RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
            RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

            /* Wait till PLL is used as system clock source */
            while((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)RCC_CFGR_SWS_PLL)
            {
            }
        }
        else
        {
            /* If HSE fails to start-up, the application will have wrong clock
            configuration. User can add here some code to deal with this error */

            /* PLL configuration = HSI * 3 = 24 MHz */
            RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSI_PREDIV | RCC_CFGR_PLLMULL3);
            /* Enable PLL */
            RCC->CR |= RCC_CR_PLLON;

            /* Wait till PLL is ready */
            while((RCC->CR & RCC_CR_PLLRDY) == 0)
            {
            }

            /* Select PLL as system clock source */
            RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
            RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

            /* Wait till PLL is used as system clock source */
            while((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)RCC_CFGR_SWS_PLL)
            {
            }
        }
        
        /* Enable the PWR clock */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
        /* Allow access to RTC */
        PWR_BackupAccessCmd(DISABLE);
        RTC_DeInit();
        
        /* LSI used as RTC source clock */
        /* The RTC Clock may varies due to LSI frequency dispersion. */
        /* Disable the LSI OSC */
        RCC_LSICmd(DISABLE);
        
        /* Disable the RTC Wakeup Interrupt */
        RTC_ITConfig(RTC_IT_WUT, DISABLE);
        /* Disable Wakeup Counter */
        RTC_WakeUpCmd(DISABLE);
        
        /* EXTI configuration */
        EXTI_ClearITPendingBit(EXTI_Line20);
        EXTI_InitStruct.EXTI_Line = EXTI_Line20;
        EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
        EXTI_InitStruct.EXTI_LineCmd = DISABLE;
        EXTI_Init(&EXTI_InitStruct);
        
        /* Disable the RTC Wakeup Interrupt */
        NVIC_InitStruct.NVIC_IRQChannel = RTC_IRQn;
        NVIC_InitStruct.NVIC_IRQChannelPriority = 0;
        NVIC_InitStruct.NVIC_IRQChannelCmd = DISABLE;
        NVIC_Init(&NVIC_InitStruct);
        
        SysTick->CTRL = 0x00000000;
        SysTick->LOAD = ((24000000 / 1000) - 1); //设置 SysTick 溢出时间为1ms
        SysTick->VAL  = 0x00000000;
        NVIC_SetPriority(SysTick_IRQn, 3);
        SysTick->CTRL = 0x00000007;
        
        cpu_level = CPU_NORMAL;
    }
    else
    {
        /* Select HSI as system clock source */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSI;
        
        /* Enable the PWR clock */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
        /* Allow access to RTC */
        PWR_BackupAccessCmd(ENABLE);
        RTC_DeInit();
        
        /* LSI used as RTC source clock */
        /* The RTC Clock may varies due to LSI frequency dispersion. */
        /* Enable the LSI OSC */
        RCC_LSICmd(ENABLE);
        
        /* Wait till LSI is ready */
        while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
        {
        }
        
        /* Select the RTC Clock Source */
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
        /* Enable the RTC Clock */
        RCC_RTCCLKCmd(ENABLE);
        /* Wait for RTC APB registers synchronisation */
        RTC_WaitForSynchro();
        /* Calendar Configuration */
        RTC_InitStruct.RTC_AsynchPrediv = 99;
        RTC_InitStruct.RTC_SynchPrediv   =  399; /* (40KHz / 100) - 1 = 399*/
        RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24;
        RTC_Init(&RTC_InitStruct);
        
        /* Configure the RTC WakeUp Clock source: CK_SPRE (1Hz) */
        RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
        RTC_SetWakeUpCounter(0);
        /* Enable the RTC Wakeup Interrupt */
        RTC_ITConfig(RTC_IT_WUT, ENABLE);
        /* Enable Wakeup Counter */
        RTC_WakeUpCmd(ENABLE);
        
        /* EXTI configuration */
        EXTI_ClearITPendingBit(EXTI_Line20);
        EXTI_InitStruct.EXTI_Line = EXTI_Line20;
        EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
        EXTI_InitStruct.EXTI_LineCmd = ENABLE;
        EXTI_Init(&EXTI_InitStruct);
        
        /* Enable the RTC Wakeup Interrupt */
        NVIC_InitStruct.NVIC_IRQChannel = RTC_IRQn;
        NVIC_InitStruct.NVIC_IRQChannelPriority = 0;
        NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStruct);
        
        /* Set the time to 00h 00mn 00s AM */
        RTC_TimeStruct.RTC_H12     = RTC_H12_AM;
        RTC_TimeStruct.RTC_Hours   = 0x00;
        RTC_TimeStruct.RTC_Minutes = 0x00;
        RTC_TimeStruct.RTC_Seconds = 0x00;
        RTC_SetTime(RTC_Format_BCD, &RTC_TimeStruct);
        
        cpu_level = CPU_POWERSAVE;
    }
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
    
    GPIO_InitStruct.GPIO_Pin = ~(GPIO_Pin_13 | GPIO_Pin_14);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    GPIO_Init(GPIOD, &GPIO_InitStruct);
    GPIO_Init(GPIOE, &GPIO_InitStruct);
    GPIO_Init(GPIOF, &GPIO_InitStruct);
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, DISABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, DISABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, DISABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, DISABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, DISABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, DISABLE);
    
    SPI_I2S_DeInit(SPI1);
    SPI_I2S_DeInit(SPI2);
    ADC_DeInit(ADC1);
    PWR_DeInit();
    USART_DeInit(USART1);
    USART_DeInit(USART2);
    USART_DeInit(USART3);
    USART_DeInit(USART4);
    USART_DeInit(USART5);
    USART_DeInit(USART6);
    USART_DeInit(USART7);
    USART_DeInit(USART8);

    if(intrs == INTR_ENABLED)
    {
        cpu_entr_enable();
    }
#endif

#endif
}

/**
  * @brief  内核状态
  */
static enum __cpu_level cpu_core_status(void)
{
    return(cpu_level);
}

/**
  * @brief   获取ram数据
  */
static uint16_t cpu_ram_get(uint32_t offset, uint16_t size, uint8_t *buffer)
{
    uint32_t addr = cpu.ram.base;

    if((offset + size) > cpu.ram.size)
    {
        return(0);
    }

    addr += offset;

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#endif

	memcpy((void *)buffer, (const void *)addr, size);

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

    return(size);
}

/**
  * @brief   设置ram数据
  */
static uint16_t cpu_ram_set(uint32_t offset, uint16_t size, const uint8_t *buffer)
{
    uint32_t addr = cpu.ram.base;

    if((offset + size) > cpu.ram.size)
    {
        return(0);
    }

    addr += offset;

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#endif

	memcpy((void *)addr, (const void *)buffer, size);

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

    return(size);
}

/**
  * @brief   获取rom数据
  */
static uint16_t cpu_rom_get(uint32_t offset, uint16_t size, uint8_t *buffer)
{
    uint32_t addr = cpu.rom.base;

    if((offset + size) > cpu.ram.size)
    {
        return(0);
    }

    addr += offset;

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#endif

	memcpy((void *)buffer, (const void *)addr, size);

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

    return(size);
}

/**
  * @brief   设置rom数据
  */
static uint16_t cpu_rom_set(uint32_t offset, uint16_t size, const uint8_t *buffer)
{
    uint32_t addr = cpu.rom.base;

    if((offset + size) > cpu.ram.size)
    {
        return(0);
    }

    addr += offset;
    return(0);
}

/**
  * @brief   获取reg数据
  */
static uint16_t cpu_reg_get(uint32_t offset, uint16_t size, uint8_t *buffer)
{
    uint32_t addr = cpu.reg.base;

    if((offset + size) > cpu.ram.size)
    {
        return(0);
    }

    addr += offset;

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#endif

	memcpy((void *)buffer, (const void *)addr, size);

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

    
    return(size);
}

/**
  * @brief   设置reg数据
  */
static uint16_t cpu_reg_set(uint32_t offset, uint16_t size, const uint8_t *buffer)
{
    uint32_t addr = cpu.reg.base;

    if((offset + size) > cpu.ram.size)
    {
        return(0);
    }

    addr += offset;

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#endif

	memcpy((void *)addr, (const void *)buffer, size);

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

    return(size);
}


/**
  * @brief
  */
const struct __cpu cpu =
{
    .core               =
    {
        .details        = cpu_core_details,
        .endian         = cpu_core_endian,
        .init           = cpu_core_init,
        .status         = cpu_core_status,
        .sleep          = cpu_core_sleep,
        .reset          = cpu_core_reset,
    },
    .interrupt          =
    {
        .disable        = cpu_entr_disable,
        .enable         = cpu_entr_enable,
        .status         = cpu_entr_status,
    },
    .watchdog           =
    {
        .overflow       = 5 * 1000,
        .feed           = watchdog_feed,
    },
    .ram                =
    {
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
		.base           = 0,
		.size           = 0,
#elif defined (STM32F091)
		.base           = SRAM_BASE,
		.size           = 32 * 1024,
#endif
        .read           = cpu_ram_get,
        .write          = cpu_ram_set,
    },
    .rom                =
    {
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
		.base           = 0,
		.size           = 0,
#elif defined (STM32F091)
		.base           = FLASH_BASE,
		.size           = 256 * 1024,
#endif
        .read           = cpu_rom_get,
        .write          = cpu_rom_set,
    },
    .reg                =
    {
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
		.base           = 0,
		.size           = 0,
#elif defined (STM32F091)
        .base           = PERIPH_BASE,
        .size           = 0xBFFFFFFF,
#endif
        .read           = cpu_reg_get,
        .write          = cpu_reg_set,
    },
};
