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
#include "stm32f0xx.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static enum __dev_status status_rtc = DEVICE_NOTINIT;
static enum __dev_status status_backup = DEVICE_NOTINIT;
static enum __battery_status battery_status_rtc = BAT_FULL;
static enum __battery_status battery_status_backup = BAT_FULL;

#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
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
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux )
	status_rtc = DEVICE_INIT;
#else
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
    
#endif
}

/**
  * @brief  
  */
static void bat_rtc_suspend(void)
{
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux )
	status_rtc = DEVICE_SUSPENDED;
#else
	if (sock != INVALID_SOCKET)
	{
		receiver.close(sock);
		sock = INVALID_SOCKET;
	}
    
    status_rtc = DEVICE_SUSPENDED;
#endif
}

/**
  * @brief  
  */
static enum __battery_status battery_rtc_status(void)
{
    return(battery_status_rtc);
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
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux )
	status_backup = DEVICE_INIT;
#else
	status_backup = DEVICE_INIT;
#endif
}

/**
  * @brief  
  */
static void bat_bkp_suspend(void)
{
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux )
	status_backup = DEVICE_SUSPENDED;
#else
    status_backup = DEVICE_SUSPENDED;
#endif
}

/**
  * @brief  
  */
static enum __battery_status battery_bkp_status(void)
{
    return(battery_status_backup);
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
		
		.status         = battery_bkp_status,
	},
};

