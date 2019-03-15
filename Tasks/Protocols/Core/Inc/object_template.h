/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __OBJECT_TEMPLATE_H__
#define __OBJECT_TEMPLATE_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/**
  * 模板函数返回值
  *
  */
typedef enum __object_errors
{
	OBJECT_NOERR = 0,
	OBJECT_ERR_TYPE,//类型错误
	OBJECT_ERR_DATA,//数据错误
	OBJECT_ERR_MEM,//内存错误
	OBJECT_ERR_ENCODE,//编码错误
	OBJECT_ERR_DECODE,//解码错误
	OBJECT_ERR_LOWLEVEL,//底层调用错误
	
	OBJECT_ERR_NODEF = 0xff,//未知错误
	
} TypeObjectErrs;

/**
  * 模板函数形参
  *
  */
typedef struct __object_params
{
	struct
    {
        uint16_t    Size;
        uint8_t     *Buffer;
        
    }               Input;
    
	struct
    {
        uint16_t    Size;
        uint16_t    Filled;
        uint8_t     *Buffer;
        
    }               Output;
    
} TypeObjectParam;



/**
  * 模板函数
  *
  */
typedef TypeObjectErrs (*TypeObject)(TypeObjectParam *P);


/**
  * 函数表注册结构
  *
  */
typedef struct
{
    char                *Name;
    uint16_t            Amount;
    const TypeObject    *Func;
    
} TypeObjectPackage;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define OBJ_IN_ADDR(P)          (P->Input.Buffer)
#define OBJ_IN_SIZE(P)          (P->Input.Size)
#define OBJ_OUT_ADDR(P)         (P->Output.Buffer)
#define OBJ_OUT_SIZE(P)         (P->Output.Size)
#define OBJ_PUSH_LENGTH(P, n)   (P->Output.Filled = n)

/* Exported function prototypes ----------------------------------------------*/

#endif /* __OBJECT_TEMPLATE_H__ */
