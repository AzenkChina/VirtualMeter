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
  * 数据分片流程
  *
  */
typedef enum
{
    IO_NO_SPLIT = 0,//不分片
    IO_SPLITTING = 0x03,//正在分片传输中
    IO_END_SPLIT = 0x0c,//分片结束
    
} IOSplit;

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
        IOSplit     Split; //分片标记
        uint32_t    Count; //当前分片数
        
    }               Input;
    
	struct
    {
        uint8_t     *Buffer; //输出缓冲
        uint16_t    Size; //输出缓冲长度
        uint16_t    Filled; //输出数据长度
        IOSplit     Split; //分片标记
        uint32_t    Count; //当前分片数
        
    }               Output;
    
	struct
    {
        uint32_t    Begin; //迭代器起始条目
        uint32_t    End; //迭代器结束条目
        
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
#define OBJ_IN_ADDR(P)          ((const uint8_t *)(P->Input.Buffer))
#define OBJ_IN_SIZE(P)          ((const uint16_t)(P->Input.Size))
#define OBJ_OUT_ADDR(P)         (P->Output.Buffer)
#define OBJ_OUT_SIZE(P)         (P->Output.Size)
#define OBJ_PUSH_LENGTH(P, n)   (P->Output.Filled = n)

/* Exported function prototypes ----------------------------------------------*/

#endif /* __OBJECT_TEMPLATE_H__ */
