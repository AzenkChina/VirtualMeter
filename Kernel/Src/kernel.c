/**
 * @brief		
 * @details		
 * @date		azenk@2016-11-15
 **/

/* Includes ------------------------------------------------------------------*/
#include "cpu.h"
#include "jiffy.h"
#include "power.h"
#include "kernel.h"
#include "tasks_sched.h"
#include "trace.h"

#if defined ( _WIN32 ) || defined ( _WIN64 )
#include <windows.h>
#include <conio.h>
#include "stdio.h"
#elif defined ( __linux )
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static enum __klevel klevel = SYSTEM_BOOT;
static uint32_t begin = 0;
static uint32_t consumption = 0;
static uint16_t calcu_loop = 0;
static uint16_t cpu_load = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief
  */
static enum __klevel get_state(void)
{
    enum __klevel level_choice;
    enum __power_status status;
    
	begin = jiffy.value();
    status = power.status();
    
    switch(status)
    {
        case SUPPLY_AC:
        case SUPPLY_DC:
        case SUPPLY_AUX:
        {
            level_choice = SYSTEM_RUN;
            break;
        }
        case POWER_WAKEUP:
        {
            level_choice = SYSTEM_WAKEUP;
            break;
        }
        case POWER_REBOOT:
        {
            level_choice = SYSTEM_REBOOT;
            break;
        }
        case POWER_RESET:
        {
            level_choice = SYSTEM_RESET;
            break;
        }
        case SUPPLY_BATTERY:
        default:
        {
            level_choice = SYSTEM_SLEEP;
            break;
        }
    }
    
    return(level_choice);
}


/**
  * @brief
  */
static void tasks_sched(enum __klevel level)
{
    static enum __klevel level_before = SYSTEM_BOOT;
    static uint32_t timing = 0;
	uint16_t relative = 0;
    uint8_t flush = 0;
    
    if(level != level_before)
    {
    	TRACE(TRACE_INFO, "System run level changed.");
        switch(level)
        {
        	case SYSTEM_BOOT:
            case SYSTEM_RUN:
            case SYSTEM_SLEEP:
            case SYSTEM_WAKEUP:
            {
                if(level_before != SYSTEM_BOOT)
                {
                	enum __klevel level_back = level;
                    
                	level = level_before;
                    tasks.exit();
                    level = level_back;
                }
                
                if((level == SYSTEM_RUN) || (level == SYSTEM_WAKEUP))
                {
                    cpu.core.init(CPU_NORMAL);
					if(level == SYSTEM_RUN)
					{
						TRACE(TRACE_INFO, "System entered normal mode.");
					}
					else
					{
						TRACE(TRACE_INFO, "System entered wakeup mode.");
					}
                }
                else
                {
                    cpu.core.init(CPU_POWERSAVE);
					TRACE(TRACE_INFO, "System entered sleep mode.");
                }
                
                tasks.init();
                
                break;
            }
            case SYSTEM_REBOOT:
            {
                tasks.exit();
                TRACE(TRACE_INFO, "Rebooting.");
                cpu.core.reset();
                while(1);
                break;
            }
            case SYSTEM_RESET:
            {
            	TRACE(TRACE_INFO, "System reseting.");
                tasks.reset();
                TRACE(TRACE_INFO, "System reset done, now reboot.");
                cpu.core.reset();
                while(1);
                break;
            }
        }
        
        level_before = level;
        flush = 0xff;
        
        cpu_load = 0;
        consumption = 0;
        calcu_loop = 0;
    }
    
    //start loops
    if((jiffy.after(timing) >= KERNEL_PERIOD) || (flush))
    {
        timing = jiffy.value();
        
        switch(level)
        {
        	case SYSTEM_BOOT:
            case SYSTEM_RUN:
            case SYSTEM_SLEEP:
            case SYSTEM_WAKEUP:
            {
                tasks.loop();
                break;
            }
            default:
            	break;
        }
		
		relative = jiffy.after(begin);
        
        consumption += relative;
        calcu_loop += 1;
        if(calcu_loop >= KERNEL_LOOP_FREQ)
        {
            cpu_load = (consumption * 100) / (calcu_loop * KERNEL_PERIOD);
            consumption = 0;
            calcu_loop = 0;
        }
		
        if((level == SYSTEM_WAKEUP) && (relative < PERIOD_RUNNING))
		{
			if(relative < PERIOD_RUNNING)
			{
				//cpu idle
				cpu.core.idle((PERIOD_RUNNING - relative - 1));
			}
		}
    }
    
    //system sleep
    if(level == SYSTEM_SLEEP)
    {
    	cpu.watchdog.feed();
        //cpu sleep
        cpu.core.sleep();
    }
}

/**
  * @brief
  */
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
int main(int argc, char *argv[])
#else
int main(void)
#endif
{
#if defined ( _WIN32 ) || defined ( _WIN64 )
    HANDLE hMutex = CreateMutexW(NULL, TRUE, L"VirtualMeter::Meter");
    DWORD Ret = GetLastError();
    
    if(hMutex)
    {
        if(Ret == ERROR_ALREADY_EXISTS)
        {
            printf("Multi-instance is not allowed!\n");
            CloseHandle(hMutex);
#if defined ( BUILD_DAEMON )
            exit(0);
#else
			_getch();
            exit(0);
#endif
        }
    }
    else
    {
        printf("Mutex create failed!\n");
        CloseHandle(hMutex);
#if defined ( BUILD_DAEMON )
        exit(0);
#else
		_getch();
        exit(0);
#endif
    }
    
    if((argc > 1) && (strcmp(argv[1], "reboot") == 0))
    {
		printf("\nEmulater reboot done.\n");
	}
	else
	{
#if defined ( BUILD_DAEMON )
        ShowWindow(GetConsoleWindow(), SW_HIDE);
        printf("Emulater started.\n");
#else
        printf("Press any key to start emulater.\n");
        _getch();
        printf("Emulater started.\n");
#endif
	}
	
    proc_self = argv[0];
#elif defined ( __linux )
    const unsigned char *lock = "/tmp/virtual_meter.pid";
    int fd;
    struct flock fl;
    char mypid[16];
    
#if defined ( BUILD_DAEMON )
	pid_t pid;
    
	pid = fork();
    
	if(pid == -1)
	{
		printf("Daemon start faild.\n");
		exit(1);
	}
	else if(pid)
	{
		exit(0);
	}
    
	if(setsid() == -1)
	{
		printf("Daemon start faild.\n");
		exit(1);
	}
    
	pid = fork();
    
	if(pid == -1)
	{
		printf("Daemon start faild.\n");
		exit(1);
	}
	else if(pid)
	{
		exit(0);
	}
	
	umask(0);
#endif
    
    fd = open(lock, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if(fd < 0)
    {
        printf("Lock create failed!\n");
        exit(1);
    }
    
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    if(fcntl(fd, F_SETLK, &fl) != 0)
    {
        printf("Multi-instance is not allowed!\n");
        close(fd);
        exit(0);
    }
    
    ftruncate(fd, 0);
    sprintf(mypid, "%ld", (long)getpid());
    write(fd, mypid, strlen(mypid) + 1);
    
    if((argc > 1) && (strcmp(argv[1], "reboot") == 0))
    {
		printf("\nEmulater reboot done.\n");
	}
	else
	{
#if defined ( BUILD_DAEMON )
        printf("Emulater started.\n");
#else
	    printf("Press enter to start emulater.\n");
		getchar();
        printf("Emulater started.\n");
#endif
	}
	
    proc_self = argv[0];
#endif
    
	TRACE(TRACE_INFO, "System started.");
    power.init();
    
    while(1)
    {
        klevel = get_state();
        tasks_sched(klevel);
        
#if defined ( _WIN32 ) || defined ( _WIN64 )
        Sleep(5);
#elif defined ( __linux )
        usleep(5*1000);
#endif
    }
    
#if defined ( _WIN32 ) || defined ( _WIN64 )
	CloseHandle(hMutex);
	exit(0);
#elif defined ( __linux )
    close(fd);
    exit(0);
#endif
}


/**
  * @brief
  */
enum __klevel system_status(void)
{
    return(klevel);
}

/**
  * @brief
  */
uint16_t system_usage(void)
{
    return(cpu_load);
}

