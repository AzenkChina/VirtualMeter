/**
 * @brief		
 * @details		
 * @date		2017-01-09
 **/

/* Includes ------------------------------------------------------------------*/
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
#include "system.h"
#include "task_console.h"
#include "types_console.h"
#include "config_console.h"

#if defined ( _WIN32 ) || defined ( _WIN64 )
#include <windows.h>
#include <conio.h>
#elif defined ( __linux )
#include <unistd.h>
#include <pthread.h>
#include "string.h"
#endif

#include "console.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "stdio.h"
#include "stdlib.h"
#include "info.h"
#include "power.h"
#include "cpu.h"


/** 在这里添加所有vm的头文件 */
#include "vm_calendar.h"
#include "vm_comm.h"
#include "vm_disconnect.h"
#include "vm_display.h"
#include "vm_keyboard.h"
#include "vm_logger.h"
#include "vm_metering.h"
#include "vm_protocol.h"
#include "vm_timed.h"
#include "vm_xmodem.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define TREE_LIST_SIZE	((uint16_t)(sizeof(tree_list) / sizeof(const luaL_Reg *)))

/* Private variables ---------------------------------------------------------*/
/** 在这里添加所有vm的luaL_Reg实例 */
static const luaL_Reg *const tree_list[] = 
{
	&vm_calendar,
	&vm_comm,
	&vm_disconnect,
	&vm_display,
	&vm_keyboard,
	&vm_logger,
	&vm_metering,
	&vm_protocol, 
	&vm_timed,
	&vm_xmodem,
};




static volatile uint8_t blocked = 0;
static char script_path[256] = {0};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void luaL_opentrees(lua_State *L)
{
	int cnt;
	
	for(cnt=0; cnt<TREE_LIST_SIZE; cnt++)
	{
		lua_pushcfunction(L, tree_list[cnt]->func);
		lua_pushstring(L, tree_list[cnt]->name);
		lua_call(L, 1, 0);
	}
}

#if defined ( __linux )
static void *ThreadLuaVM(void *arg)
#else
static DWORD CALLBACK ThreadLuaVM(PVOID pvoid)
#endif
{
	static int Err;
	lua_State *L;

	Err = -1;

	if(strlen(script_path) == 0)
	{
#if defined ( __linux )
		return((void *)&Err);
#else
		return(Err);
#endif
	}
	
	L = luaL_newstate(); /* 建立Lua运行环境 */
    
	if(!L)
	{
		memset(script_path, 0, sizeof(script_path));
#if defined ( __linux )
		return((void *)&Err);
#else
		return(Err);
#endif
	}
    
	luaL_openlibs(L);
	luaL_opentrees(L);
	Err = luaL_dofile(L, script_path); /* 运行Lua脚本 */
	lua_close(L);
    
#if defined ( __linux )
	sleep(1);
#else
	Sleep(1000);
#endif
    
	memset(script_path, 0, sizeof(script_path));
    
#if defined ( __linux )
	return((void *)&Err);
#else
	return(Err);
#endif
}


static void command_received(const char *str)
{
	char buff[64];
    char *p;
    
    strncpy(buff, str, sizeof(buff));
    
    p = strtok(buff, " ");
    
    if(p)
    {
    	char trace_info[256] = "Received command: ";
    	
	    if(memcmp(p, "exit", 4) == 0)
	    {
	    	strcat(trace_info, "exit");
			strcat(trace_info, ".");
			TRACE(TRACE_INFO, trace_info);
			exit(0);
	    }
	    else if(memcmp(p, "list", 4) == 0)
	    {
            uint8_t cnt;
            
	    	strcat(trace_info, "list.");
	    	TRACE(TRACE_INFO, trace_info);
	    	
	    	printf("cpu usage: %d%%\n",system_usage());
            printf("id:    name:         status:  prio:                        time:\n");
	        printf("                               init   exit  reset   loop    init    exit    loop     max\n");
	        for(cnt=0; cnt<task_ctrl.amount(); cnt++)
	        {
	            printf("%s", task_ctrl.info(cnt));
	        }
	    }
	    else if(memcmp(p, "reset", 5) == 0)
	    {
	    	strcat(trace_info, "reset.");
	    	TRACE(TRACE_INFO, trace_info);
	    	power.alter(POWER_RESET);
	    }
	    else if(memcmp(p, "block", 5) == 0)
	    {
			p = strtok(NULL, " ");
			
        	int num = atoi(p);
        	
        	if(num <= 0)
        	{
        		num = 0;
        	}
			else if(num >= 30)
        	{
        		num = 30;
        	}
        	
        	blocked = num;
        	
			strcat(trace_info, " blocked.");
			TRACE(TRACE_INFO, trace_info);
	    }
	    else if(memcmp(p, "sleep", 5) == 0)
	    {
			p = strtok(NULL, " ");
			
        	int num = atoi(p);
        	int cnt;
        	
        	if((num <= 0) || (num >= 86400))
        	{
        		num = 1;
        	}
        	
			strcat(trace_info, " sleep.");
			TRACE(TRACE_INFO, trace_info);
			
			for(cnt=0; cnt<num; cnt++)
			{
				if((cnt%10) == 0)
				{
					printf("Sleep %d second, %d second remain.\n", num, (num-cnt));
				}
#if defined ( __linux )
                sleep(1);
#else
                Sleep(1000);
#endif
			}
	    }
	    else if(memcmp(p, "power", 5) == 0)
	    {
	    	strcat(trace_info, "power");
	        
        	printf("Power status: ");
        	enum __power_status status = power.status();
        	switch(status)
        	{
			    case SUPPLY_AC:
			    case SUPPLY_DC:
			    {
			    	printf("Net");
		    		break;
				}
			    case SUPPLY_AUX:
			    {
			    	printf("Auxiliary");
		    		break;
				}
			    case SUPPLY_BATTERY:
			    {
			    	printf("Battery");
		    		break;
				}
			    case POWER_WAKEUP:
			    {
			    	printf("Wake up");
		    		break;
				}
			    case POWER_REBOOT:
			    {
			    	printf("Rebooting");
		    		break;
				}
			    case POWER_RESET:
			    {
			    	printf("Reseting");
		    		break;
				}
			}
			
			printf("\n");
	        
            strcat(trace_info, ".");
	        TRACE(TRACE_INFO, trace_info);
	    }
	    else if(memcmp(p, "script", 6) == 0)
	    {
	    	p = strtok(NULL, " ");
	    	
    		if(!p)
    		{
			    printf("No file specified.\n");
			    goto CMD_EXIT;
			}
			
    		if(strlen(p) >= sizeof(script_path))
    		{
			    printf("File name is too long.\n");
			    goto CMD_EXIT;
			}
	    	
	    	if(strlen(script_path) == 0)
	    	{
	    		if(!strstr(p,"lua"))
	    		{
				    printf("Invalid file.\n");
				    goto CMD_EXIT;
				}
				else
				{
					*(strstr(p,"lua") + 3) = '\0';
				}
	    		
#if defined ( __linux )
				if(access(p, 0) != 0)
				{
				    printf("Invalid file.\n");
				    goto CMD_EXIT;
				}
#else
				if(_access(p, 0) != 0)
				{
				   	printf("Invalid file.\n");
				    goto CMD_EXIT;
				}
#endif

			    strcat(trace_info, "scrpit execute ");
			    strcat(trace_info, p);
			    strcat(trace_info, ".");
			    TRACE(TRACE_INFO, trace_info);
				
#if defined ( __linux )
			    pthread_t thread;
			    pthread_attr_t thread_attr;
			    sleep(1);
			    strcpy(script_path, p);
			    pthread_attr_init(&thread_attr);
			    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
			    pthread_create(&thread, &thread_attr, ThreadLuaVM, NULL);
			    pthread_attr_destroy(&thread_attr);
#else
			    HANDLE hThread;
			    Sleep(1000);
			    strcpy(script_path, p);
			    hThread = CreateThread(NULL, 0, ThreadLuaVM, 0, 0, NULL);
			    CloseHandle(hThread);
#endif
			}
			else
			{
			    printf("VM is already in running.\n");
			    goto CMD_EXIT;
			}
	    }
	    else if(memcmp(p, "info", 4) == 0)
	    {
	    	strcat(trace_info, "info.");
	    	TRACE(TRACE_INFO, trace_info);
	    	
	    	get_software_version(VERSION_FULL, 128, (uint8_t *)buff);
	    	printf("Software version:  %s\n", buff);
	    	
	    	get_hardware_version(VERSION_FULL, 128, (uint8_t *)buff);
	    	printf("Hardware version:  %s\n", buff);
	    	
	    	get_tasks_version(VERSION_FULL, 128, (uint8_t *)buff);
	    	printf("Tasks version:     %s\n", buff);
	    	
	    	get_compile_time(128, (uint8_t *)buff);
	    	printf("Compile time:      %s\n", (uint8_t *)buff);
	    }
		else if(memcmp(p, "help", 4) == 0)
	    {
	        printf(" exit:       Emulate a core hardfault.\n");
	        printf(" list:       Print the tasks and status in scheduler.\n");
	        printf(" reset:      Reset the program to factory state.\n");
	        printf(" block {n}:  Block the scheduler for {n} seconds.\n");
	        printf(" sleep {n}:  Sleep the console for {n} seconds.\n");
	        printf(" power:      Print the current power status.\n");
	        printf(" script {f}: Run the {f} script.\n");
	        printf(" info:       Print the program informations.\n");
	        printf(" help:       Print this message.\n");
	        
	    	strcat(trace_info, "help.");
	    	TRACE(TRACE_INFO, trace_info);
	    }
    }
    
CMD_EXIT:
    printf("Command: ");
}

/**
  * @brief  
  */
static void console_init(void)
{
    console.control.init(DEVICE_NORMAL);
    console.handler.filling(command_received);
    TRACE(TRACE_INFO, "Task console initialized.");
}

/**
  * @brief  
  */
static void console_loop(void)
{
	while(blocked)
	{
		blocked -= 1;
#if defined ( __linux )
        sleep(1);
#else
        Sleep(1000);
#endif
	}
}

/**
  * @brief  
  */
static void console_exit(void)
{
    console.handler.remove();
    console.control.suspend();
    
    TRACE(TRACE_INFO, "Task console exited.");
}

/**
  * @brief  
  */
static void console_reset(void)
{
    console.handler.remove();
    console.control.suspend();
    
    TRACE(TRACE_INFO, "Task console reset.");
}

/**
  * @brief  
  */
static enum __task_status console_status(void)
{
    return(TASK_INIT);
}


/**
  * @brief  
  */
const struct __task_sched task_console = 
{
    .name               = NAME_CONSOLE,
    .init               = console_init,
    .loop               = console_loop,
    .exit               = console_exit,
    .reset              = console_reset,
    .status             = console_status,
    .api                = (void *)0,
};

#endif
