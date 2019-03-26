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
#if defined ( __GNUC__ )	//GCC�µ� weak �ؼ���
#define PORT_WEAK		__attribute__((weak))
#elif defined ( __ICCARM__ )    //IAR�µ� weak �ؼ���
#define PORT_WEAK		__weak
#elif defined ( __CC_ARM )      //Keil MDK�µ� weak �ؼ���
#define PORT_WEAK		__weak
#endif

#if defined ( _MSC_VER )      //Visual Studio��

//����һ���ص�
#define CALLBACK_CONNECT(name, func)			__declspec(selectany) void (*const callback_##name)(void *args) = func
//����һ���ص�
#define CALLBACK_SLOT(name)				__declspec(selectany) extern void (*const callback_##name)(void *args) = 0

#else

//����һ���ص�
#define CALLBACK_CONNECT(name, func)			void (*const callback_##name)(void *args) = func
//����һ���ص�
#define CALLBACK_SLOT(name)				void __callback_##name(void *args){(void)args;} \
							PORT_WEAK void (*const callback_##name)(void *args) = __callback_##name; \
							extern void (*const callback_##name)(void *args)

#endif


#define CALLBACK_TRIGGER(name, args)			callback_##name((void *)args)

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif /* __CALLBACK_H__ */
