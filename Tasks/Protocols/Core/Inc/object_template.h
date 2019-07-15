/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __OBJECT_TEMPLATE_H__
#define __OBJECT_TEMPLATE_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/**
  * 对象调用结果
  *
  */
typedef enum
{
	OBJECT_NOERR = 0,
	OBJECT_ERR_TYPE,//类型错误
	OBJECT_ERR_DATA,//数据错误
	OBJECT_ERR_MEM,//内存错误
	OBJECT_ERR_ENCODE,//编码错误
	OBJECT_ERR_DECODE,//解码错误
	OBJECT_ERR_LOWLEVEL,//底层调用错误
	
	OBJECT_ERR_NODEF = 0xff,//未知错误
	
} ObjectErrs;

/**
  * 迭代流程
  *
  */
typedef enum
{
    ITER_NONE = 0,//不进行迭代
    ITER_ONGOING = 0x03,//正在迭代
    ITER_FINISHED = 0x0c,//迭代结束
    
} IterStatus;

/**
  * 对象形参
  *
  */
typedef struct
{
	struct
    {
        uint8_t     *Buffer; //输入数据
        uint16_t    Size; //输入数据长度
        uint32_t    ID; //内部数据标识
        
    }               Input;
    
	struct
    {
        uint8_t     *Buffer; //输出缓冲
        uint16_t    Size; //输出缓冲长度
        uint16_t    Filled; //输出数据长度
        
    }               Output;
    
	struct
    {
        uint32_t    Begin; //迭代器起始条目
        uint32_t    End; //迭代器结束条目
        uint8_t     Status; //迭代器当前状态
        
    }               Iterator;
    
} ObjectPara;



/**
  * 对象模板
  *
  */
typedef ObjectErrs (*TypeObject)(ObjectPara *P);


/**
  * 对象组注册结构
  *
  */
typedef struct
{
    char                *Name;
    uint16_t            Amount;
    const TypeObject    *Func;
    
} ObjectCluster;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define OBJ_IN_ADDR(P)                  ((const uint8_t *)(P->Input.Buffer))//获取输入数据首地址
#define OBJ_IN_SIZE(P)                  ((uint16_t)(P->Input.Size))//获取输入数据字节长度
#define OBJ_IN_ID(P)                    ((uint32_t)(P->Input.ID))//获取数据标识

#define OBJ_OUT_ADDR(P)                 (P->Output.Buffer)//获取输出缓冲首地址
#define OBJ_OUT_SIZE(P)                 (P->Output.Size)//获取输出缓冲字节大小
#define OBJ_PUSH_LENGTH(P, n)           (P->Output.Filled = n)//设置输出数据字节长度

#define OBJ_ITERATE_BEGIN(P)            (P->Iterator.Begin)//获取迭代器起始
#define OBJ_ITERATE_END(P)              (P->Iterator.End)//获取迭代器结束
#define OBJ_ITERATE_STATUS(P)           ((IterStatus)(P->Iterator.Status))//获取迭代器状态
#define OBJ_ITERATE_SET_BEGIN(P, v)     (P->Iterator.Begin = v)//设置迭代器起始
#define OBJ_ITERATE_SET_END(P, v)       (P->Iterator.End = v)//设置迭代器结束
#define OBJ_ITERATE_SET_STATUS(P, v)    (P->Iterator.Status = (IterStatus)v)//设置迭代器状态

/* Exported function prototypes ----------------------------------------------*/

#endif /* __OBJECT_TEMPLATE_H__ */
