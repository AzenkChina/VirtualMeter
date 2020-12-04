/**
 * @brief		
 * @details		
 * @date		2018-08-30
 **/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "stdbool.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  非易失内存分配接口
  */
struct __nvram
{
    void                        *(*address)(const char *name);
    uint32_t                    (*size)(const char *name);
};

/**
  * @brief  堆接口
  */
struct __heap
{
    void                            *(*salloc)(const char *name, uint32_t size);
    void                            *(*szalloc)(const char *name, uint32_t size);
    void                            *(*scalloc)(const char *name, uint32_t n, uint32_t size);
    void                            *(*dalloc)(uint32_t size);
    void                            *(*dzalloc)(uint32_t size);
    void                            *(*dcalloc)(uint32_t n, uint32_t size);
    void                            (*free)(void *address);
    uint32_t                        (*copy)(void *dst, const void *src, uint32_t size);
    uint32_t                        (*set)(void *address, uint8_t ch, uint32_t size);
};


/**
  * @brief  环形队列信息
  */
struct __ring_info
{
	uint16_t length; //每条记录的最大长度
	uint16_t capacity; //最大条目容量
	uint16_t amount; //当前条目数量
};

/**
  * @brief  并发缓冲信息
  */
struct __parallel_info
{
	uint16_t length; //每条记录的最大长度
	uint16_t capacity; //最大条目容量
};

/**
  * @brief  存储器接口
  */
struct __file
{
	struct
	{
		uint32_t				(*read)(const char *name, uint32_t offset, uint32_t size, void *buff);
		uint32_t				(*write)(const char *name, uint32_t offset, uint32_t size, const void *buff);
		uint32_t				(*size)(const char *name);
		
	}							parameter;
	
	struct
	{
		uint32_t				(*read)(const char *name, uint32_t index, uint32_t size, void *buff, bool reverse);
		uint32_t				(*append)(const char *name, uint32_t size, const void *buff);
		uint32_t				(*truncate)(const char *name, uint32_t amount, bool reverse);
		bool					(*info)(const char *name, struct __ring_info *ring_info);
		bool					(*reset)(const char *name);
		bool					(*init)(const char *name, uint32_t length);
		
	}							ring;
	
	struct
	{
		uint32_t				(*read)(const char *name, uint32_t index, uint32_t size, void *buff);
		uint32_t				(*write)(const char *name, uint32_t index, uint32_t size, const void *buff);
		bool					(*signature)(const char *name, uint32_t *signature);
		bool					(*status)(const char *name, uint32_t index);
		bool					(*renew)(const char *name, uint32_t index);
		bool					(*info)(const char *name, struct __parallel_info *parallel_info);
		bool					(*reset)(const char *name);
		bool					(*init)(const char *name, uint32_t length);
		
	}							parallel;
};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#if defined ( _WIN64 ) || defined ( __amd64__ ) || defined ( __x86_64 )

#define STRUCT_OFFSET(name, member)         ((uint64_t)(&((name *)0)->member))
#define STRUCT_SIZE(name, member)           ((uint64_t)(sizeof((name *)0)->member))

#else

#define STRUCT_OFFSET(name, member)         ((uint32_t)(&((name *)0)->member))
#define STRUCT_SIZE(name, member)           ((uint32_t)(sizeof((name *)0)->member))

#endif

#define HEAP_FORWARD(p, c)                  heap.set((void *)(*p), c, 1); (uint8_t *)(*p)++

/* Exported function prototypes ----------------------------------------------*/
extern struct __nvram nvram;
extern struct __heap heap;
extern struct __file file;

#endif /* __ALLOCATOR_H__ */
