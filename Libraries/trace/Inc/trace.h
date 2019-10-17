/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TRACE_H__
#define __TRACE_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  跟踪等级
  */
enum __trace_level
{
    TRACE_INFO = 1,
    TRACE_WARN,
    TRACE_ERR,
};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/**
  * @brief  使用 __USE_TRACE 和 __USE_ASSERT 两个宏来配置是否启用日志功能
  */
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
#define __USE_TRACE             //是否使用TRACE功能
#define __USE_ASSERT            //是否使用ASSERT功能
#else

//在这里编写是否启用这两个宏

#endif

/* Exported function prototypes ----------------------------------------------*/
#if defined ( __USE_TRACE )
extern void TRACE(enum __trace_level level, const char *str, ...);
#else
    #define TRACE(l, s, ...)			;
#endif /* __USE_TRACE */

#if defined ( __USE_ASSERT )
    #define ASSERT(condition)           if(condition) while(1)
#else
#define ASSERT(condition)               if(condition) TRACE(TRACE_ERR, "Assert triggered at %d in %s", __LINE__, __FILE__)
#endif /* __USE_ASSERT */

#endif /* __TRACE_H__ */
