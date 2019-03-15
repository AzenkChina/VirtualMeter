/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __OBJECT_TEMPLATE_H__
#define __OBJECT_TEMPLATE_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/**
  * ģ�庯������ֵ
  *
  */
typedef enum __object_errors
{
	OBJECT_NOERR = 0,
	OBJECT_ERR_TYPE,//���ʹ���
	OBJECT_ERR_DATA,//���ݴ���
	OBJECT_ERR_MEM,//�ڴ����
	OBJECT_ERR_ENCODE,//�������
	OBJECT_ERR_DECODE,//�������
	OBJECT_ERR_LOWLEVEL,//�ײ���ô���
	
	OBJECT_ERR_NODEF = 0xff,//δ֪����
	
} TypeObjectErrs;

/**
  * ģ�庯���β�
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
  * ģ�庯��
  *
  */
typedef TypeObjectErrs (*TypeObject)(TypeObjectParam *P);


/**
  * ������ע��ṹ
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
