/**
 * @brief		
 * @details		
 * @date		2016-11-15
 **/

/* Includes ------------------------------------------------------------------*/
#include "meter.h"
#include "trace.h"

#if defined ( _WIN32 ) || defined ( _WIN64 )
#include "comm_socket.h"
#include <windows.h>
#elif defined ( __linux )
#include <unistd.h>
#include <pthread.h>
#include "comm_socket.h"
#else

#if defined (DEMO_STM32F091)
#include "stm32f0xx.h"
#include "vspi1.h"
#endif

#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define devspi      vspi1

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static enum __dev_status status = DEVICE_NOTINIT;
static void(*meter_callback)(void *buffer) = (void(*)(void *))0;

#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
static SOCKET sock = INVALID_SOCKET;
static volatile int32_t metering_data[42] = {0};
static volatile uint8_t updating = 0;
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
    int32_t buff[42];
	int32_t recv_size;
    uint32_t j;
    
    while(1)
    {
#if defined ( __linux )
    	usleep(8*1000);
#else
    	Sleep(8);
#endif
    	
	    if(sock == INVALID_SOCKET)
	    {
	        continue;
	    }
	    
		recv_size = receiver.read(sock, (uint8_t *)buff, sizeof(buff));
		if (recv_size < (42 * 4))
		{
			continue;
		}

		updating = 0xff;

#if defined ( __linux )
    	usleep(2*1000);
#else
    	Sleep(2);
#endif

		if (status == DEVICE_INIT)
		{
			for (j = 0; j<(int)R_ESC; j++)
			{
				metering_data[j] += buff[j];
			}
		}

		for (j = (int)R_ESC; j<(int)R_FREQ; j++)
		{
			metering_data[j] = buff[j];
		}

		updating = 0;
    }
    
	return(0);
}

/**
  * @brief  模拟程序使用当前电压来判断是否启动
  */
uint8_t is_powered(void)
{
    //只要任何一相电压大于等于1V即可
	if(metering_data[R_UARMS - 1] >= 1000 || \
        metering_data[R_UBRMS - 1] >= 1000 || \
        metering_data[R_UCRMS - 1] >= 1000)
    {
        return(0xff);
    }
    
    return(0);
}
#endif

/**
  * @brief  
  */
static enum __dev_status meter_status(void)
{
    //...获取当前设备状态
    return(status);
}

/**
  * @brief  
  */
static void meter_init(enum __dev_state state)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	sock = receiver.open(50002);

	if(sock == INVALID_SOCKET)
	{
		TRACE(TRACE_INFO, "Create receiver for metering failed.");
	}
    
    if(status == DEVICE_NOTINIT)
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
	
	meter_callback= (void(*)(void *))0;
	status = DEVICE_INIT;
#else

#if defined (DEMO_STM32F091)
    GPIO_InitTypeDef GPIO_InitStruct;
    
    if(state == DEVICE_NORMAL)
	{
		devspi.control.init(state);
		
		if(devspi.control.status() != DEVICE_INIT)
		{
			status = DEVICE_ERROR;
			return;
		}
		
		//PD12 教表参数/事件输出
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
		
		GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOD, &GPIO_InitStruct);
		
		status = DEVICE_INIT;
	}
	
	meter_callback= (void(*)(void *))0;
#endif

#endif
}

/**
  * @brief  
  */
static void meter_suspend(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	if(sock != INVALID_SOCKET)
	{
		receiver.close(sock);
		sock = INVALID_SOCKET;
	}
#else
    
#if defined (DEMO_STM32F091)
    //...IO，寄存器，时钟等关闭
    GPIO_InitTypeDef GPIO_InitStruct;
    
    devspi.control.suspend();
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStruct);
#endif
    
#endif
    
    status = DEVICE_SUSPENDED;
}

#if defined (DEMO_STM32F091)
/**
  * @brief  将 __metering_meta 转换为 702x 的命名字
  */
static uint8_t meter_cmd_translate(enum __metering_meta id)
{
    uint8_t result = 0xff;
    
    switch(id)
    {
        case R_EPA:
            result = 0x1E;
            break;
        case R_EPB:
            result = 0x1F;
            break;
        case R_EPC:
            result = 0x20;
            break;
        case R_EPT:
            result = 0x21;
            break;
        case R_EQA:
            result = 0x22;
            break;
        case R_EQB:
            result = 0x23;
            break;
        case R_EQC:
            result = 0x24;
            break;
        case R_EQT:
            result = 0x25;
            break;
        case R_ESA:
            result = 0x35;
            break;
        case R_ESB:
            result = 0x36;
            break;
        case R_ESC:
            result = 0x37;
            break;
        case R_EST:
            result = 0x38;
            break;
        case R_PA:
            result = 0x01;
            break;
        case R_PB:
            result = 0x02;
            break;
        case R_PC:
            result = 0x03;
            break;
        case R_PT:
            result = 0x04;
            break;
        case R_QA:
            result = 0x05;
            break;
        case R_QB:
            result = 0x06;
            break;
        case R_QC:
            result = 0x07;
            break;
        case R_QT:
            result = 0x08;
            break;
        case R_SA:
            result = 0x09;
            break;
        case R_SB:
            result = 0x0A;
            break;
        case R_SC:
            result = 0x0B;
            break;
        case R_ST:
            result = 0x0C;
            break;
        case R_PFA:
            result = 0x14;
            break;
        case R_PFB:
            result = 0x15;
            break;
        case R_PFC:
            result = 0x16;
            break;
        case R_PFT:
            result = 0x17;
            break;
        case R_UARMS:
            result = 0x0D;
            break;
        case R_UBRMS:
            result = 0x0E;
            break;
        case R_UCRMS:
            result = 0x0F;
            break;
        case R_IARMS:
            result = 0x10;
            break;
        case R_IBRMS:
            result = 0x11;
            break;
        case R_ICRMS:
            result = 0x12;
            break;
        case R_ITRMS:
            result = 0x13;
            break;
        case R_PGA:
            result = 0x18;
            break;
        case R_PGB:
            result = 0x19;
            break;
        case R_PGC:
            result = 0x1A;
            break;
        case R_YUAUB:
            result = 0x26;
            break;
        case R_YUAUC:
            result = 0x27;
            break;
        case R_YUBUC:
            result = 0x28;
            break;
        case R_FREQ:
            result = 0x1C;
            break;
    }
}
#endif

static void meter_runner(uint16_t msecond)
{
    
}


static uint32_t meter_config_read(uint32_t addr, uint32_t count, void *buffer)
{
	return(0);
}

static uint32_t meter_config_write(uint32_t addr, uint32_t count, const void *buffer)
{
	return(0);
}

static uint32_t meter_data_read(uint32_t addr, uint32_t count, void *buffer)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    uint16_t id = (uint16_t)addr;
    uint16_t loop;
    int32_t *result = (int32_t *)buffer;
    
    if(!result)
    {
        return(0);
    }
    
    if(((uint16_t)id < (uint16_t)R_EPT) || ((uint16_t)id > (uint16_t)R_FREQ))
    {
        return(0);
    }
    
    if(((uint16_t)id + (uint16_t)count) > (uint16_t)R_FREQ)
    {
        return(0);
    }
    
    for(loop=0; loop<((uint16_t)count); loop++)
    {
    	if(updating)
    	{
#if defined ( __linux )
            usleep(5*1000);
#else
            Sleep(5);
#endif
    	}
    	
        switch((enum __metering_meta)id)
        {
            case R_EPA:
            case R_EPB:
            case R_EPC:
            case R_EPT:
            case R_EQA:
            case R_EQB:
            case R_EQC:
            case R_EQT:
            case R_ESA:
            case R_ESB:
            case R_ESC:
            case R_EST:
                *result = metering_data[id - 1];
                metering_data[id - 1] = 0;
                break;
            case R_PA:
            case R_PB:
            case R_PC:
            case R_PT:
            case R_QA:
            case R_QB:
            case R_QC:
            case R_QT:
            case R_SA:
            case R_SB:
            case R_SC:
            case R_ST:
            case R_PFA:
            case R_PFB:
            case R_PFC:
            case R_PFT:
            case R_UARMS:
            case R_UBRMS:
            case R_UCRMS:
            case R_IARMS:
            case R_IBRMS:
            case R_ICRMS:
            case R_ITRMS:
            case R_PGA:
            case R_PGB:
            case R_PGC:
            case R_YUAUB:
            case R_YUAUC:
            case R_YUBUC:
            case R_FREQ:
                *result = metering_data[id - 1];
                break;
            default:
                *result = 0;
        }
        
        id += 1;
        result += 1;
    }
    
    return((count & 0xffff));
#else
    
#if defined (DEMO_STM32F091)
    int32_t *result = (int32_t *)buffer;
    *result = 0;
    return(1);
#endif
    
#endif
}

static uint32_t meter_data_write(uint32_t addr, uint32_t count, const void *buffer)
{
	return(0);
}

/**
  * @brief  702x 的 handler 用于向外抛出异常
  */
static void meter_handler_filling(void(*callback)(void *buffer))
{
	if(callback)
    {
        meter_callback = callback;
    }
}

static void meter_handler_remove(void)
{
    meter_callback= (void(*)(void *))0;
}










/**
  * @brief  
  */
const struct __misc meter = 
{
    .control        = 
    {
        .name       = "metering",
        .status     = meter_status,
        .init       = meter_init,
        .suspend    = meter_suspend,
    },
    
    .runner         = meter_runner,
    
    .data			=
    {
		.read		= meter_data_read,
		.write		= meter_data_write,
    },
    
    .config			=
    {
		.read		= meter_config_read,
		.write		= meter_config_write,
    },
    
    .handler		=
    {
		.filling	= meter_handler_filling,
		.remove		= meter_handler_remove,
    },
};



