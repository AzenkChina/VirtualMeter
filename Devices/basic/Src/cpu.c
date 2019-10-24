/**
 * @brief
 * @details
 * @date        2016-11-15
 **/

/* Includes ------------------------------------------------------------------*/
#include "string.h"
#include "kernel.h"
#include "cpu.h"
#include "jiffy.h"
#include "trace.h"

#if defined ( _WIN32 ) || defined ( _WIN64 )
#include <windows.h>
#elif defined ( __linux )
#include <pthread.h>
#include <unistd.h>
#include "stdlib.h"
#else
#if defined (DEMO_STM32F091)
#include "stm32f0xx.h"
#endif
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
/* 虚拟运行下仅支持注册一个中断：系统滴答中断 中断号为 0 */
#define INTERRUPT_AMOUNT    ((uint8_t)1)
#else

#if defined (DEMO_STM32F091)
/* STM32 Demo 支持注册三个中断：系统滴答中断，SVC中断，PendSV中断，中断号分别为 0 1 2 */
#define INTERRUPT_AMOUNT    ((uint8_t)3)
#endif

#endif

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
static volatile uint8_t counter = 0;
static volatile uint8_t in_sleep = 0;
char *proc_self = (char *)0;
#endif

static const union {uint32_t i; uint8_t c;} endian = {.i = 0x55FFFFAA,};
static enum __interrupt_status intr_status = INTR_ENABLED;
static enum __cpu_level cpu_level = CPU_NORMAL;

void(*hooks[INTERRUPT_AMOUNT])(void);
void(*hooks_redundance[INTERRUPT_AMOUNT])(void);

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
                
                if((hooks[0] != 0) && (hooks[0] == hooks_redundance[0]))
                {
                    hooks[0]();
                }
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
			cpu.core.reset();
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

#if defined (DEMO_STM32F091)
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
    if(endian.c == 0x55)
    {
        return(CPU_BIG_ENDIAN);
    }
    else if(endian.c == 0xAA)
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

#if defined (DEMO_STM32F091)
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

#if defined (DEMO_STM32F091)
    /* Allow access to RTC */
    PWR_BackupAccessCmd(ENABLE);
    /* Clear flags */
    RTC_ClearFlag(RTC_FLAG_TAMP2F| \
                    RTC_FLAG_TAMP1F| \
                    RTC_FLAG_TSOVF| \
                    RTC_FLAG_TSF| \
                    RTC_FLAG_WUTF| \
                    RTC_FLAG_ALRAF| \
                    RTC_FLAG_RSF);
    /* Disable access to RTC */
    PWR_BackupAccessCmd(DISABLE);
    
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
#endif

#endif
}

/**
  * @brief  内核空闲
  */
static void cpu_core_idle(uint16_t tick)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	if(cpu_level == CPU_NORMAL)
	{
#if defined ( __linux )
		usleep(tick*1000);
#else
		Sleep(tick);
#endif
	}
#else
	
#if defined (DEMO_STM32F091)
	uint32_t timing = jiffy.value();
	
	if(cpu_level == CPU_NORMAL)
	{
		while(1)
		{
			PWR_EnterSleepMode(PWR_SLEEPEntry_WFI);
			if(jiffy.after(timing) > tick)
			{
				return;
			}
		}
	}
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

#if defined (DEMO_STM32F091)
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
#if defined (DEMO_STM32F091)
    __disable_irq();
#endif
}

/**
  * @brief  使能全局中断
  */
static void cpu_entr_enable(void)
{
    intr_status = INTR_ENABLED;
#if defined (DEMO_STM32F091)
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
  * @brief  注册中断函数
  */
static bool cpu_entr_request(uint8_t n, void(*hook)(void))
{
    enum __interrupt_status intrs = cpu_entr_status();
    
    if(hook == 0)
    {
        return(false);
    }
    
    if(n >= INTERRUPT_AMOUNT)
    {
        return(false);
    }
    
    if((hooks[n] != 0) && (hooks[n] == hooks_redundance[n]))
    {
        return(false);
    }
    
    if(intrs == INTR_ENABLED)
    {
        cpu_entr_disable();
    }
    
    hooks[n] = hook;
    hooks_redundance[n] = hook;
    
    if(intrs == INTR_ENABLED)
    {
        cpu_entr_enable();
    }
    
    return(true);
}

/**
  * @brief  清理中断函数
  */
static bool cpu_entr_release(uint8_t n)
{
    enum __interrupt_status intrs = cpu_entr_status();
    
    if(n >= INTERRUPT_AMOUNT)
    {
        return(false);
    }
    
    if(intrs == INTR_ENABLED)
    {
        cpu_entr_disable();
    }
    
    hooks[n] = 0;
    hooks_redundance[n] = 0;
    
    if(intrs == INTR_ENABLED)
    {
        cpu_entr_enable();
    }
    
    return(true);
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
    
    enum __interrupt_status intrs = cpu_entr_status();
    
    if(intrs == INTR_ENABLED)
    {
        cpu_entr_disable();
    }
	
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
    
    memset(hooks, 0, sizeof(hooks));
    memset(hooks_redundance, 0, sizeof(hooks_redundance));

    if(intrs == INTR_ENABLED)
    {
        cpu_entr_enable();
    }
#else

#if defined (DEMO_STM32F091)
    GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    RTC_InitTypeDef RTC_InitStruct;
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
    
    enum __interrupt_status intrs = cpu_entr_status();
    
    if(intrs == INTR_ENABLED)
    {
        cpu_entr_disable();
    }
    
    /* Set HSION bit */
    RCC->CR |= (uint32_t)0x00000001;
    /* Select HSI as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSI;
	/* Wait till HSI is used as system clock source */
	while((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)RCC_CFGR_SWS_HSI);
    /* Disable Prefetch Buffer and Flash Latency */
    FLASH->ACR = 0x00000000;
    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
    /* PCLK = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE_DIV1;
	/* Disable the all NVIC interrupts */
    NVIC->ICER[0] = 0xffffffff;
	/* Clear the all NVIC Pendings */
    NVIC->ICPR[0] = 0xffffffff;
    /* Disable SysTick */
    SysTick->CTRL = 0x00000000;
    SysTick->VAL  = 0x00000000;
    
    /* Disable the RTC */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    RTC_DeInit();
    RCC_LSICmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET);
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
    RCC_RTCCLKCmd(DISABLE);
    PWR_BackupAccessCmd(DISABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, DISABLE);
    
    /* Reset All Peripheral */
    RCC_DeInit();
	PWR_DeInit();
    EXTI_DeInit();
    GPIO_DeInit(GPIOA);
    GPIO_DeInit(GPIOB);
    GPIO_DeInit(GPIOC);
    GPIO_DeInit(GPIOD);
    GPIO_DeInit(GPIOE);
    GPIO_DeInit(GPIOF);
    
    /* Set All GPIO to Low Power Mode */
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
    
	/* Enable WDG */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_256);
    IWDG_SetReload(40000 / 32);
    IWDG_ReloadCounter();
    IWDG_Enable();
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Disable);
	
    if(level == CPU_NORMAL)
    {
        /* Enable DBG */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_DBGMCU, ENABLE);
        DBGMCU_APB1PeriphConfig(DBGMCU_IWDG_STOP, ENABLE);
        
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
            while((RCC->CR & RCC_CR_PLLRDY) == 0);

            /* Select PLL as system clock source */
            RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
            RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

            /* Wait till PLL is used as system clock source */
            while((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)RCC_CFGR_SWS_PLL);
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
            while((RCC->CR & RCC_CR_PLLRDY) == 0);

            /* Select PLL as system clock source */
            RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
            RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

            /* Wait till PLL is used as system clock source */
            while((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)RCC_CFGR_SWS_PLL);
        }
        
        
        /* Enable the SysTick */
        /* Set SysTick Interrupt in every Millisecond */
        SysTick->CTRL = 0x00000000;
        SysTick->LOAD = ((24000000 / 1000) - 1);
        SysTick->VAL  = 0x00000000;
        NVIC_SetPriority(SysTick_IRQn, 3);
        SysTick->CTRL = 0x00000007;
        
        cpu_level = CPU_NORMAL;
    }
    else
    {
        /* Disable DBG */
        DBGMCU_APB1PeriphConfig(DBGMCU_IWDG_STOP, DISABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_DBGMCU, DISABLE);
        
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
        GPIO_InitStruct.GPIO_Pin = (GPIO_Pin_13 | GPIO_Pin_14);
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
        GPIO_Init(GPIOA, &GPIO_InitStruct);
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, DISABLE);
        
        /* Enable the PWR clock */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
        /* Allow access to RTC */
        PWR_BackupAccessCmd(ENABLE);
        /* Clock Configuration */
        RTC_InitStruct.RTC_AsynchPrediv = (40 - 1); /* (40KHz / 10) = 1KHz*/
        RTC_InitStruct.RTC_SynchPrediv = (KERNEL_LOOP_SLEEPED - 1); /* Wake up every (KERNEL_LOOP_SLEEPED/1000) second */
        RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24;
        RTC_Init(&RTC_InitStruct);
        /* Enable the RTC Clock */
        RCC_RTCCLKCmd(ENABLE);
        /* Wait for RTC APB registers synchronisation */
        RTC_WaitForSynchro();
        /* Configure the RTC WakeUp Clock source: CK_SPRE (1Hz) */
        RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
        RTC_SetWakeUpCounter(0);
        /* Enable the RTC Wakeup Interrupt */
        RTC_ITConfig(RTC_IT_WUT, ENABLE);
        /* Enable Wakeup Counter */
        RTC_WakeUpCmd(ENABLE);
        /* Clear flags */
        RTC_ClearFlag(RTC_FLAG_TAMP2F| \
                        RTC_FLAG_TAMP1F| \
                        RTC_FLAG_TSOVF| \
                        RTC_FLAG_TSF| \
                        RTC_FLAG_WUTF| \
                        RTC_FLAG_ALRAF| \
                        RTC_FLAG_RSF);
        /* Disable access to RTC */
        PWR_BackupAccessCmd(DISABLE);
        
        /* EXTI configuration */
        EXTI_ClearITPendingBit(EXTI_Line20);
        EXTI_InitStruct.EXTI_Line = EXTI_Line20;
        EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
        EXTI_InitStruct.EXTI_LineCmd = ENABLE;
        EXTI_Init(&EXTI_InitStruct);
        
        /* Enable the RTC Wakeup Interrupt */
        NVIC_SetPriority(RTC_IRQn, 3);
        NVIC_EnableIRQ(RTC_IRQn);
        
        cpu_level = CPU_POWERSAVE;
    }
    
    memset(hooks, 0, sizeof(hooks));
    memset(hooks_redundance, 0, sizeof(hooks_redundance));

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
		.idle			= cpu_core_idle,
    },
    .interrupt          =
    {
        .disable        = cpu_entr_disable,
        .enable         = cpu_entr_enable,
        .status         = cpu_entr_status,
        .request        = cpu_entr_request,
        .release        = cpu_entr_release,
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
#elif defined (DEMO_STM32F091)
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
#elif defined (DEMO_STM32F091)
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
#elif defined (DEMO_STM32F091)
        .base           = PERIPH_BASE,
        .size           = 0xBFFFFFFF,
#endif
        .read           = cpu_reg_get,
        .write          = cpu_reg_set,
    },
};
