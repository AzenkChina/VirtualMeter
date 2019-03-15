/**
 * @brief		
 * @details		
 * @date		2016-08-14
 **/

/* Includes ------------------------------------------------------------------*/
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )

#if defined ( __linux )
#include <unistd.h>
#include <pthread.h>
#include "stdio.h"
#include "console.h"
#else
#include <windows.h>
#include "stdio.h"
#include "console.h"
#endif


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static enum __dev_status status = DEVICE_NOTINIT;
static void(*command_received)(const char *) = (void(*)(const char *))0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#if defined ( __linux )
static void *ThreadConsole(void *arg)
#else
static DWORD CALLBACK ThreadConsole(PVOID pvoid)
#endif
{
    void(*callback)(const char *);
    char buff[256];
    
    printf("Command: ");
    
	while(1)
	{
        fgets(buff, sizeof(buff), stdin);
        
        callback = command_received;
        
        if(callback)
        {
            callback((const char *)buff);
        }
	}
	
	return(0);
}

/**
  * @brief  
  */
static enum __dev_status console_status(void)
{
    return(status);
}

/**
  * @brief  
  */
static void console_init(enum __dev_state state)
{
    if(status == DEVICE_NOTINIT)
    {
#if defined ( __linux )
        pthread_t thread;
        pthread_attr_t thread_attr;
        
        pthread_attr_init(&thread_attr);
        pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&thread, &thread_attr, ThreadConsole, NULL);
        pthread_attr_destroy(&thread_attr);
#else
        HANDLE hThread;
        hThread = CreateThread(NULL, 0, ThreadConsole, 0, 0, NULL);
        CloseHandle(hThread);
#endif
    }
    
    command_received = (void(*)(const char *))0;
    
    status = DEVICE_INIT;
}

/**
  * @brief  
  */
static void console_suspend(void)
{
}


/**
  * @brief  
  */
static void console_handler_filling(void(*callback)(const char *))
{
    command_received = callback;
}

/**
  * @brief  
  */
static void console_handler_clear(void)
{
    command_received = (void(*)(const char *))0;
}







const struct __console console = 
{
    .control            = 
    {
        .name           = "console",
        .status         = console_status,
        .init           = console_init,
        .suspend        = console_suspend,
    },
    
    .handler            = 
    {
        .filling        = console_handler_filling,
        .remove			= console_handler_clear,
    },
};

#endif
