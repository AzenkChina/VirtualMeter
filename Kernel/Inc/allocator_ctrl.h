/**
 * @brief		
 * @details		
 * @date		2018-08-30
 **/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ALLOCATOR_CTRL_H__
#define __ALLOCATOR_CTRL_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  堆解锁
  */
enum __heap_unlock
{
    HEAP_UNLOCK_ALLOC = 0x03,//解锁heap_alloc函数
    HEAP_UNLOCK_FREE = 0x30,//解锁heap_free函数
};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/**
  * @brief  存储器控制接口
  */
struct __disk_ctrl
{
    void                            (*start)(void);
    void                            (*lock)(void);
    void                            (*unlock)(void);
    void                            (*idle)(void);
    void                            (*format)(void);
};

/**
  * @brief  堆控制接口
  */
struct __heap_ctrl
{
    void                            (*sinit)(void);
    void                            (*dinit)(void);
    void                            (*unlock)(enum __heap_unlock fun);
    void                            (*lock)(void);
    void                            (*recycle)(void);
};


/* Exported function prototypes ----------------------------------------------*/
extern const struct __disk_ctrl disk_ctrl;
extern const struct __heap_ctrl heap_ctrl;

#endif /* __ALLOCATOR_CTRL_H__ */
