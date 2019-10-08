/**
 * @brief		
 * @details		
 * @date		2016-08-16
 **/

/* Includes ------------------------------------------------------------------*/
#include "battery.h"

#if defined ( _WIN32 ) || defined ( _WIN64 )
#include "comm_socket.h"
#include <windows.h>
#include "trace.h"
#elif defined ( __linux )
#include <unistd.h>
#include <pthread.h>
#include "comm_socket.h"
#include "trace.h"
#else

#if defined (DEMO_STM32F091)
#include "stm32f0xx.h"
#endif

#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static enum __dev_status status_rtc = DEVICE_NOTINIT;
static enum __dev_status status_backup = DEVICE_NOTINIT;

#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
static enum __battery_status battery_status_rtc = BAT_FULL;
static enum __battery_status battery_status_backup = BAT_FULL;
static SOCKET sock = INVALID_SOCKET;
static struct __battery_data
{
	enum
	{
	    BATTERY_TYPE_RTC = 0,
	    BATTERY_TYPE_BACKUP,
	} type;
	
    enum __battery_status status;
    
} BatteryData;
#endif

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
#if defined ( __linux )
static void *ThreadRecvMail(void *arg)
#else
static DWORD CALLBACK ThreadRecvMail(PVOID pvoid)
#endif
{
	int32_t recv_size = 0;
    
    while(1)
    {
#if defined ( __linux )
		usleep(200*1000);
#else
		Sleep(200);
#endif
		if (sock == INVALID_SOCKET)
		{
			continue;
		}
	    
		recv_size = receiver.read(sock, (uint8_t *)&BatteryData, sizeof(BatteryData));

		if (recv_size != sizeof(BatteryData))
		{
			continue;
		}

		if (BatteryData.type == BATTERY_TYPE_RTC)
		{
			battery_status_rtc = BatteryData.status;
		}
		else if (BatteryData.type == BATTERY_TYPE_BACKUP)
		{
			battery_status_backup = BatteryData.status;
		}
    }
    
	return(0);
}
#endif
/**
  * @brief  
  */
static enum __dev_status bat_rtc_status(void)
{
    return(status_rtc);
}

/**
  * @brief  
  */
static void bat_rtc_init(enum __dev_state state)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	sock = receiver.open(50005);

	if (sock == INVALID_SOCKET)
	{
		TRACE(TRACE_INFO, "Create receiver for battery failed.");
	}
    else
    {
	    if(status_rtc == DEVICE_NOTINIT)
	    {
#if defined ( __linux )
			pthread_t thread;
			pthread_attr_t thread_attr;
			
			pthread_attr_init(&thread_attr);
			pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
			pthread_create(&thread, &thread_attr, ThreadRecvMail, NULL);
			pthread_attr_destroy(&thread_attr);
#else
			HANDLE hThread;
			hThread = CreateThread(NULL, 0, ThreadRecvMail, 0, 0, NULL);
			CloseHandle(hThread);
#endif
	    }
	    
    	status_rtc = DEVICE_INIT;
    }
#else

#if defined (DEMO_STM32F091)
    if(state == DEVICE_NORMAL)
    {
        status_rtc = DEVICE_INIT;
    }
#endif

#endif
}

/**
  * @brief  
  */
static void bat_rtc_suspend(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	if (sock != INVALID_SOCKET)
	{
		receiver.close(sock);
		sock = INVALID_SOCKET;
	}
    
    status_rtc = DEVICE_SUSPENDED;
#else

#if defined (DEMO_STM32F091)
    status_rtc = DEVICE_SUSPENDED;
#endif

#endif
}

/**
  * @brief  额定电池容量mA·H
  */
static uint32_t battery_rtc_rated_capacity(void)
{
    return(600);
}

/**
  * @brief  额定电池电压
  */
static uint32_t battery_rtc_rated_voltage(void)
{
    return(3000);
}

/**
  * @brief  电池电压
  */
static uint32_t battery_rtc_voltage(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    return(3000);
#else

#if defined (DEMO_STM32F091)
    GPIO_InitTypeDef GPIO_InitStruct;
    ADC_InitTypeDef ADC_InitStruct;
    int64_t Voltage;
    int64_t Ref;
    uint16_t *Cali = (uint16_t *)0x1FFFF7BA;
    
    if(status_rtc == DEVICE_INIT)
    {
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOC, &GPIO_InitStruct);
        
        /* ADC1 Periph clock enable */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
        
        /* ADCs DeInit */
        ADC_DeInit(ADC1);
        
        /* Select ADC clock source */
        ADC_ClockModeConfig(ADC1, ADC_ClockMode_SynClkDiv2);
        
        /* Initialize ADC structure */
        ADC_StructInit(&ADC_InitStruct);
        ADC_Init(ADC1, &ADC_InitStruct);
        ADC_VrefintCmd(ENABLE);
        ADC_ChannelConfig(ADC1, ADC_Channel_Vrefint , ADC_SampleTime_71_5Cycles);
        /* ADC Calibration */
        ADC_GetCalibrationFactor(ADC1);
        /* Enable the ADC peripheral */
        ADC_Cmd(ADC1, ENABLE);
        
        /* ADC1 regular Software Start Conv */ 
        ADC_StartOfConversion(ADC1);
        while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
        /* Get ADC1 converted data */
        Voltage = ADC_GetConversionValue(ADC1);
        if(Voltage)
        {
            Ref = ((int64_t)*Cali) * 3300 / Voltage;
            ADC_VrefintCmd(DISABLE);
        }
        else
        {
            ADC_VrefintCmd(DISABLE);
            ADC_DeInit(ADC1);
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
            return(0);
        }
        
        ADC1->CHSELR = 0;
        ADC_ChannelConfig(ADC1, ADC_Channel_10, ADC_SampleTime_41_5Cycles);
        ADC_StartOfConversion(ADC1);
        while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
        Voltage = ADC_GetConversionValue(ADC1);
        Voltage = Voltage * Ref / 4096;
        Voltage *= 2;
        
        ADC_DeInit(ADC1);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
        
        return((uint32_t)Voltage);
    }
    else
    {
        return(0);
    }
#endif

#endif
}

/**
  * @brief  
  */
static enum __battery_status battery_rtc_status(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    return(battery_status_rtc);
#else

#if defined (DEMO_STM32F091)
    uint32_t voltage = battery_rtc_voltage();
    
    if(voltage > 3600)
    {
        return(BAT_FULL);
    }
    else if(voltage > 3400)
    {
        return(BAT_LOW);
    }
    else
    {
        return(BAT_EMPTY);
    }
#endif

#endif
}












/**
  * @brief  
  */
static enum __dev_status bat_bkp_status(void)
{
    return(status_backup);
}

/**
  * @brief  
  */
static void bat_bkp_init(enum __dev_state state)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	status_backup = DEVICE_INIT;
#else
    
#if defined (DEMO_STM32F091)
    if(state == DEVICE_NORMAL)
    {
        status_backup = DEVICE_INIT;
    }
#endif
    
#endif
}

/**
  * @brief  
  */
static void bat_bkp_suspend(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	status_backup = DEVICE_SUSPENDED;
#else
    
#if defined (DEMO_STM32F091)
    status_backup = DEVICE_SUSPENDED;
#endif
    
#endif
}

/**
  * @brief  额定电池容量mA·H
  */
static uint32_t battery_bkp_rated_capacity(void)
{
    return(1800);
}

/**
  * @brief  额定电池电压
  */
static uint32_t battery_bkp_rated_voltage(void)
{
    return(6300);
}

/**
  * @brief  电池电压
  */
static uint32_t battery_bkp_voltage(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    return(6300);
#else

#if defined (DEMO_STM32F091)
    GPIO_InitTypeDef GPIO_InitStruct;
    ADC_InitTypeDef ADC_InitStruct;
    int64_t Voltage;
    int64_t Ref;
    uint16_t *Cali = (uint16_t *)0x1FFFF7BA;
    
    if(status_backup == DEVICE_INIT)
    {
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOC, &GPIO_InitStruct);
        
        /* ADC1 Periph clock enable */
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
        
        /* ADCs DeInit */  
        ADC_DeInit(ADC1);
        
        /* Select ADC clock source */
        ADC_ClockModeConfig(ADC1, ADC_ClockMode_SynClkDiv2);
        
        /* Initialize ADC structure */
        ADC_StructInit(&ADC_InitStruct);
        ADC_Init(ADC1, &ADC_InitStruct);
        ADC_VrefintCmd(ENABLE);
        ADC_ChannelConfig(ADC1, ADC_Channel_Vrefint, ADC_SampleTime_71_5Cycles);
        /* ADC Calibration */
        ADC_GetCalibrationFactor(ADC1);
        /* Enable the ADC peripheral */
        ADC_Cmd(ADC1, ENABLE);
        
        /* ADC1 regular Software Start Conv */ 
        ADC_StartOfConversion(ADC1);
        while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
        /* Get ADC1 converted data */
        Voltage = ADC_GetConversionValue(ADC1);
        if(Voltage)
        {
            Ref = ((int64_t)*Cali) * 3300 / Voltage;
            ADC_VrefintCmd(DISABLE);
        }
        else
        {
            ADC_VrefintCmd(DISABLE);
            ADC_DeInit(ADC1);
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
            return(0);
        }
        
        ADC1->CHSELR = 0;
        ADC_ChannelConfig(ADC1, ADC_Channel_13 , ADC_SampleTime_41_5Cycles);
        ADC_StartOfConversion(ADC1);
        while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
        Voltage = ADC_GetConversionValue(ADC1);
        Voltage = Voltage * Ref / 4096;
        Voltage *= 3;
        
        ADC_DeInit(ADC1);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
        
        return((uint32_t)Voltage);
    }
    else
    {
        return(0);
    }
#endif

#endif
}

/**
  * @brief  
  */
static enum __battery_status battery_bkp_status(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    return(battery_status_backup);
#else

#if defined (DEMO_STM32F091)
    uint32_t voltage = battery_bkp_voltage();
    
    if(voltage > 6200)
    {
        return(BAT_FULL);
    }
    else if(voltage > 5800)
    {
        return(BAT_LOW);
    }
    else
    {
        return(BAT_EMPTY);
    }
#endif

#endif
}

const struct __battery battery[BAT_AMOUNT] = 
{
	{
		.control        = 
		{
			.name       = "battery rtc",
			.status     = bat_rtc_status,
			.init       = bat_rtc_init,
			.suspend    = bat_rtc_suspend,
		},
		
        .rated          =
        {
            .capacity   = battery_rtc_rated_capacity,
            .voltage    = battery_rtc_rated_voltage,
        },
        
        .voltage        = battery_rtc_voltage,
		.status         = battery_rtc_status,
	},
	{
		.control        = 
		{
			.name       = "battery bkp",
			.status     = bat_bkp_status,
			.init       = bat_bkp_init,
			.suspend    = bat_bkp_suspend,
		},
        
        .rated          =
        {
            .capacity   = battery_bkp_rated_capacity,
            .voltage    = battery_bkp_rated_voltage,
        },
		
        .voltage        = battery_bkp_voltage,
		.status         = battery_bkp_status,
	},
};

