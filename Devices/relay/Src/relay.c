/**
 * @brief		
 * @details		
 * @date		2016-08-16
 **/

/* Includes ------------------------------------------------------------------*/
#include "relay.h"

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
static enum __dev_status status = DEVICE_NOTINIT;
enum __switch_status relay_state = SWITCH_UNKNOWN;

#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
static SOCKET sock = INVALID_SOCKET;
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
    enum __switch_status state;
	int32_t recv_size = 0;
    
    while(1)
    {
#if defined ( __linux )
    	usleep(500*1000);
#else
    	Sleep(500);
#endif
    	
	    if(sock == INVALID_SOCKET)
	    {
	        continue;
	    }

		recv_size = receiver.read(sock, (uint8_t *)&state, sizeof(state));

		if (recv_size != sizeof(state))
		{
			continue;
		}

		if (status != DEVICE_INIT)
		{
			continue;
		}

		if (state == SWITCH_UNKNOWN)
		{
			relay_state = SWITCH_UNKNOWN;
		}
    }
    
	return(0);
}
#endif

/**
  * @brief  
  */
static enum __dev_status relay_status(void)
{
    return(status);
}

/**
  * @brief  
  */
static void relay_init(enum __dev_state state)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	sock = receiver.open(50004);

	if (sock == INVALID_SOCKET)
	{
		TRACE(TRACE_INFO, "Create receiver for realy failed.");
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
    
    if(state == DEVICE_NORMAL)
    {
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
        
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOC, &GPIO_InitStruct);
        GPIO_SetBits(GPIOC, GPIO_Pin_7);
        relay_state = SWITCH_OPEN;
    }
    
    status = DEVICE_INIT;
#endif
    
#endif
}

/**
  * @brief  
  */
static void relay_suspend(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	if (sock != INVALID_SOCKET)
	{
		receiver.close(sock);
		sock = INVALID_SOCKET;
	}
    
    status = DEVICE_SUSPENDED;
#else
    
#if defined (DEMO_STM32F091)
    status = DEVICE_SUSPENDED;
#endif
    
#endif
}

static void relay_runner(uint16_t msecond)
{
    
}

static enum __switch_status relay_get(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	return(relay_state);
#else
    
#if defined (DEMO_STM32F091)
    if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7) == Bit_RESET)
    {
        return(SWITCH_CLOSE);
    }
    else
    {
        return(SWITCH_OPEN);
    }
#endif
    
#endif
}

static uint8_t relay_set(enum __switch_status status)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	relay_state = status;
	return((uint8_t)relay_state);
#else
    
#if defined (DEMO_STM32F091)
    if(status == SWITCH_CLOSE)
    {
        GPIO_ResetBits(GPIOC, GPIO_Pin_7);
        return(SWITCH_CLOSE);
    }
    else
    {
        GPIO_SetBits(GPIOC, GPIO_Pin_7);
        return(SWITCH_OPEN);
    }
#endif
    
#endif
}

const struct __switch relay = 
{
	.control        = 
	{
		.name       = "relay",
		.status     = relay_status,
		.init       = relay_init,
		.suspend    = relay_suspend,
	},
    
    .runner         = relay_runner,
    .get            = relay_get,
    .set            = relay_set,
};

