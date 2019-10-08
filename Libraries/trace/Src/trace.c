/**
 * @brief		
 * @details		
 * @date		2016-08-15
 **/

/* Includes ------------------------------------------------------------------*/
#include "trace.h"

#if defined ( _WIN32 ) || defined ( _WIN64 )
#include <direct.h>
#include <io.h>
#include <time.h>
#include <stdarg.h>
#include "stdio.h"
#elif defined ( __linux )
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <stdarg.h>
#include "stdio.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if defined ( _WIN32 ) || defined ( _WIN64 )
#define FIL_PATH    "./log/trace.log"
#define DIR_PATH    "./log"
#elif defined ( __linux )
#if defined ( BUILD_DAEMON )
#define FIL_PATH    "/var/log/virtual_meter.log"
#define DIR_PATH    "/var/log"
#else
#define FIL_PATH    "./log/trace.log"
#define DIR_PATH    "./log"
#endif
#endif

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#if defined ( __USE_TRACE )
/**
  * @brief  
  */
void TRACE(enum __trace_level level, const char *str, ...)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    FILE *fp;
    va_list ap;
    struct tm *ptm;
    time_t stamp;

#if defined ( __linux )
    if(access(FIL_PATH, 0) != 0)
    {
        mkdir(DIR_PATH, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
#else
    if(_access(FIL_PATH, 0) != 0)
    {
        _mkdir(DIR_PATH);
    }
#endif
    
    fp = fopen(FIL_PATH,"a+");
    
    if(!fp)
    {
        return;
    }
    
    time(&stamp);
    ptm = localtime(&stamp);
    fprintf(fp, \
            "%02d-%02d-%02d %02d:%02d:%02d",\
            (ptm->tm_year%100), ((ptm->tm_mon+1)%100), (ptm->tm_mday%100),\
            (ptm->tm_hour%100), (ptm->tm_min%100), (ptm->tm_sec%100));
    
    switch(level)
    {
        case TRACE_INFO:
        {
            fprintf(fp, "  INFO  ");
            break;
        }
        case TRACE_WARN:
        {
            fprintf(fp, "  WARN  ");
            break;
        }
        case TRACE_ERR:
        {
            fprintf(fp, "  ERROR ");
            break;
        }
    }
	
	va_start(ap, str);
	vfprintf(fp, str, ap);
	va_end(ap);
	
    fprintf(fp, "\n");
    fflush(fp);
    fclose(fp);
#else

#endif /* #if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux ) */
}

#endif /* __USE_TRACE */
