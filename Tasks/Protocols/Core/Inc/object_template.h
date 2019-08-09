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
    
    OBJECT_ERR_LOWLEVEL = 128,//底层调用错误
    OBJECT_ERR_MEM = 129,//内存错误
    OBJECT_ERR_DATA = 130,//数据错误
	OBJECT_ERR_TYPE = 131,//类型错误
	OBJECT_ERR_ENCODE = 132,//编码错误
	OBJECT_ERR_DECODE = 133,//解码错误
    
	OBJECT_ERR_NODEF = 250,//未知错误
	
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
        uint32_t    From; //迭代器起始条目
        uint32_t    To; //迭代器结束条目
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

#define OBJ_ITERATE_INIT(P, f, t)       P->Iterator.From = f; \
                                        P->Iterator.To = t; \
                                        if(P->Iterator.From != P->Iterator.To) \
                                            {P->Iterator.Status = ITER_ONGOING;}//初始化迭代器
#define OBJ_ITERATE_FROM(P)             (P->Iterator.From)//迭代器起始
#define OBJ_ITERATE_TO(P)               (P->Iterator.To)//迭代器结束
#define OBJ_ITERATE_STEPPING(P)         if(P->Iterator.From < P->Iterator.To) \
                                            {P->Iterator.From += 1;} \
                                        else if(P->Iterator.From > P->Iterator.To) \
                                            {P->Iterator.From -= 1;} \
                                        if(P->Iterator.From == P->Iterator.To) \
                                            {P->Iterator.Status = ITER_FINISHED;}//迭代器步进

#define OBJ_IS_ITERATING(P)             ((P->Iterator.Status == ITER_ONGOING) && \
                                        (P->Iterator.From != P->Iterator.To))//判断是否正在迭代中

/* Exported function prototypes ----------------------------------------------*/

#endif /* __OBJECT_TEMPLATE_H__ */
