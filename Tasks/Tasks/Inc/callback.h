/**
 * @brief		
 * @details		
 * @date		azenk@2019-01-03
 **/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CALLBACK_H__
#define __CALLBACK_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported macro ------------------------------------------------------------*/
#if defined ( __GNUC__ )        //GCC下的 weak 关键字
#define PORT_WEAK               __attribute__((weak))
#elif defined ( __ICCARM__ )    //IAR下的 weak 关键字
#define PORT_WEAK               __weak
#elif defined ( __CC_ARM )      //Keil MDK下的 weak 关键字
#define PORT_WEAK               __weak
#endif

#if defined ( _MSC_VER )      //Visual Studio下

//定义一个回调
#define CALLBACK_DEF(name, func)			__declspec(selectany) void (*const callback_##name)(void *args) = func
//声明一个回调
#define CALLBACK_DECL(name)					__declspec(selectany) extern void (*const callback_##name)(void *args) = 0

#else

//定义一个回调
#define CALLBACK_DEF(name, func)			void (*const callback_##name)(void *args) = func
//声明一个回调
#define CALLBACK_DECL(name)					PORT_WEAK void (*const callback_##name)(void *args) = 0; \
                                            extern void (*const callback_##name)(void *args)

#endif



//执行一个回调
#define CALLBACK_EXEC(name, args)			if(callback_##name) callback_##name((void *)args)

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif /* __CALLBACK_H__ */
