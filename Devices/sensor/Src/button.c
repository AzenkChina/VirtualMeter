/**
 * @brief		
 * @details		
 * @date		2018-09-07
 **/

/* Includes ------------------------------------------------------------------*/
#include "button.h"

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
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
static enum __dev_status status = DEVICE_NOTINIT;
enum __switch_status status_main_cover = SWITCH_CLOSE;
enum __switch_status status_sub_cover = SWITCH_CLOSE;
enum __switch_status status_magnetic = SWITCH_OPEN;
static SOCKET sock = INVALID_SOCKET;
static struct
{
	enum
	{
	    SENSORS_TYPE_MAIN_COVER = 0,
	    SENSORS_TYPE_SUB_COVER,
	    SENSORS_TYPE_MAGNETIC,
	} type;
	
    enum __switch_status status;
    
} SensorsData;
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
    	
	    if(sock == INVALID_SOCKET)
	    {
	        continue;
	    }
	    
		recv_size = receiver.read(sock, (uint8_t *)&SensorsData, sizeof(SensorsData));
		if (recv_size != sizeof(SensorsData))
		{
			continue;
		}
    }
    
	return(0);
}

enum __switch_status mailslot_magnetic(void)
{
    return(status_magnetic);
}
#endif

/**
  * @brief  
  */
static enum __dev_status main_cover_status(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	return(status);
#else

#if defined (DEMO_STM32F091)
    return(DEVICE_INIT);
#endif

#endif
}

/**
  * @brief  
  */
static void main_cover_init(enum __dev_state state)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	sock = receiver.open(50003);
    
    if(sock == INVALID_SOCKET)
    {
        TRACE(TRACE_INFO, "Create receiver for sensors failed.");
    }
    else
    {
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
	    
    	status = DEVICE_INIT;
    }
#else

#if defined (DEMO_STM32F091)
    GPIO_InitTypeDef GPIO_InitStruct;
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOE, &GPIO_InitStruct);
#endif

#endif
}

/**
  * @brief  
  */
static void main_cover_suspend(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    if(sock != INVALID_SOCKET)
    {
		receiver.close(sock);
		sock = INVALID_SOCKET;
    }
    
    status = DEVICE_SUSPENDED;
#else

#endif
}

static enum __switch_status main_cover_get(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    return(status_main_cover);
#else

#if defined (DEMO_STM32F091)
    if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_8) == Bit_RESET)
    {
        return(SWITCH_OPEN);
    }
    else
    {
        return(SWITCH_CLOSE);
    }
#endif
	
#endif
}

static void main_cover_runner(uint16_t msecond)
{
}

static uint8_t main_cover_set(enum __switch_status status)
{
    return(0);
}



/**
  * @brief  
  */
static enum __dev_status sub_cover_status(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    return(status);
#else
	return(DEVICE_INIT);
#endif
}

/**
  * @brief  
  */
static void sub_cover_init(enum __dev_state state)
{
#if defined (DEMO_STM32F091)
    GPIO_InitTypeDef GPIO_InitStruct;
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
#endif
}

/**
  * @brief  
  */
static void sub_cover_suspend(void)
{
}

static void sub_cover_runner(uint16_t msecond)
{
}

static enum __switch_status sub_cover_get(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    return(status_sub_cover);
#else

#if defined (DEMO_STM32F091)
    if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6) == Bit_RESET)
    {
        return(SWITCH_OPEN);
    }
    else
    {
        return(SWITCH_CLOSE);
    }
#endif

#endif
}

static uint8_t sub_cover_set(enum __switch_status status)
{
    return(0);
}



const struct __switch main_cover = 
{
	.control        = 
	{
		.name       = "main cover",
		.status     = main_cover_status,
		.init       = main_cover_init,
		.suspend    = main_cover_suspend,
	},
    
    .runner         = main_cover_runner,
    .get            = main_cover_get,
    .set            = main_cover_set,
};

const struct __switch sub_cover = 
{
	.control        = 
	{
		.name       = "sub cover",
		.status     = sub_cover_status,
		.init       = sub_cover_init,
		.suspend    = sub_cover_suspend,
	},
    
    .runner         = sub_cover_runner,
    .get            = sub_cover_get,
    .set            = sub_cover_set,
};

