/**
 * @brief		
 * @details		
 * @date		2016-08-16
 **/

/* Includes ------------------------------------------------------------------*/
#include "keys.h"

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
#include "delay.h"
#endif

#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static enum __dev_status status = DEVICE_NOTINIT;
static void(*key_changed)(uint16_t id, enum __key_status status);

#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
static SOCKET sock = INVALID_SOCKET;
static struct __key_data
{
    uint16_t id;
    enum __key_status status;
    
} KeyData;
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
	int32_t recv_size;
    
    while(1)
    {
#if defined ( __linux )
    	usleep(50*1000);
#else
    	Sleep(50);
#endif
    	
	    if(sock == INVALID_SOCKET)
	    {
	        continue;
	    }
	    
	    if(status != DEVICE_INIT)
	    {
	    	continue;
	    }

		recv_size = receiver.read(sock, (uint8_t *)&KeyData, sizeof(KeyData));

		if (recv_size != sizeof(KeyData))
		{
			continue;
		}
    }
    
	return(0);
}
#endif

/**
  * @brief  
  */
static enum __dev_status key_status(void)
{
    return(status);
}

/**
  * @brief  
  */
static void key_init(enum __dev_state state)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	KeyData.status = KEY_NONE;
	
	sock = receiver.open(50006);

	if (sock == INVALID_SOCKET)
	{
		TRACE(TRACE_INFO, "Create receiver for keyboard failed.");
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
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
    
    //上翻按键
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    //编程按键
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOD, &GPIO_InitStruct);
    
    //上翻按键
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    status = DEVICE_INIT;
#endif

#endif
}

/**
  * @brief  
  */
static void key_suspend(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    if(sock != INVALID_SOCKET)
    {
		receiver.close(sock);
		sock = INVALID_SOCKET;
    }
    
    status = DEVICE_SUSPENDED;
#else
	status = DEVICE_SUSPENDED;
#endif
}


static void key_runner(uint16_t msecond)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    if(key_changed && (KeyData.status != KEY_NONE))
    {
    	key_changed(KeyData.id, KeyData.status);
    	KeyData.status = KEY_NONE;
    }
#else

#if defined (DEMO_STM32F091)
    static uint16_t id_before = 0;
    uint16_t id_sample[3];
    
    id_sample[0] = 0;
    id_sample[1] = 0;
    id_sample[2] = 0;
    
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == Bit_SET)
    {
        id_sample[0] |= KEY_ID_UP;
    }
    
    if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_7) == Bit_SET)
    {
        id_sample[0] |= KEY_ID_DOWN;
    }
    
    if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_15) == Bit_SET)
    {
        id_sample[0] |= KEY_ID_PROGRAM;
    }
    
    if(id_before != id_sample[0])
    {
        mdelay(1);
        
        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == Bit_SET)
        {
            id_sample[1] |= KEY_ID_UP;
        }
        
        if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_7) == Bit_SET)
        {
            id_sample[1] |= KEY_ID_DOWN;
        }
        
        if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_15) == Bit_SET)
        {
            id_sample[1] |= KEY_ID_PROGRAM;
        }
        
        if(id_sample[0] != id_sample[1])
        {
            return;
        }
        
        mdelay(2);
        
        if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == Bit_SET)
        {
            id_sample[2] |= KEY_ID_UP;
        }
        
        if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_7) == Bit_SET)
        {
            id_sample[2] |= KEY_ID_DOWN;
        }
        
        if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_15) == Bit_SET)
        {
            id_sample[2] |= KEY_ID_PROGRAM;
        }
        
        if(id_sample[1] != id_sample[2])
        {
            return;
        }
        
        if(id_sample[0] != 0)
        {
            key_changed(id_sample[0], KEY_PRESS);
        }
        else
        {
            key_changed(id_sample[0], KEY_RELEASE);
        }
        
        id_before = id_sample[0];
    }
    
#endif

#endif
}

static uint16_t key_get(void)
{
#if defined (DEMO_STM32F091)
    uint16_t id_sample = 0;
    
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == Bit_SET)
    {
        id_sample |= KEY_ID_UP;
    }
    
    if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_7) == Bit_SET)
    {
        id_sample |= KEY_ID_DOWN;
    }
    
    if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_15) == Bit_SET)
    {
        id_sample |= KEY_ID_PROGRAM;
    }
    
    return(id_sample);
#endif
}

static void key_filling(void(*callback)(uint16_t id, enum __key_status status))
{
    key_changed = callback;
}

static void key_remove(void)
{
    key_changed = (void(*)(uint16_t, enum __key_status))0;
}

const struct __keys keys = 
{
    .control        = 
    {
        .name       = "keyboard",
        .status     = key_status,
        .init       = key_init,
        .suspend    = key_suspend,
    },
    
    .runner         = key_runner,
    .get            = key_get,
    
    .handler        =
    {
        .filling    = key_filling,
        .remove     = key_remove,
    },
};

