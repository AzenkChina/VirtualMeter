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
#include "stm32f0xx.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static enum __dev_status status = DEVICE_NOTINIT;
enum __switch_status relay_state = SWITCH_CLOSE;

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
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux )
	status = DEVICE_INIT;
#else
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
#endif
}

/**
  * @brief  
  */
static void relay_suspend(void)
{
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux )
	status = DEVICE_SUSPENDED;
#else
	if (sock != INVALID_SOCKET)
	{
		receiver.close(sock);
		sock = INVALID_SOCKET;
	}
    
    status = DEVICE_SUSPENDED;
#endif
}

static void relay_runner(uint16_t msecond)
{
    
}

static enum __switch_status relay_get(void)
{
	return(relay_state);
}

static uint8_t relay_set(enum __switch_status status)
{
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux )
    return((uint8_t)relay_state);
#else
	relay_state = status;
	return((uint8_t)relay_state);
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

