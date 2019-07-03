/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __OBJECT_TEMPLATE_H__
#define __OBJECT_TEMPLATE_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/**
  * ������ý��
  *
  */
typedef enum
{
	OBJECT_NOERR = 0,
	OBJECT_ERR_TYPE,//���ʹ���
	OBJECT_ERR_DATA,//���ݴ���
	OBJECT_ERR_MEM,//�ڴ����
	OBJECT_ERR_ENCODE,//�������
	OBJECT_ERR_DECODE,//�������
	OBJECT_ERR_LOWLEVEL,//�ײ���ô���
	
	OBJECT_ERR_NODEF = 0xff,//δ֪����
	
} ObjectErrs;

/**
  * ���ݷ�Ƭ����
  *
  */
typedef enum
{
    IO_NO_SPLIT = 0,//����Ƭ
    IO_SPLITTING = 0x03,//���ڷ�Ƭ������
    IO_END_SPLIT = 0x0c,//��Ƭ����
    
} IOSplit;

/**
  * �����β�
  *
  */
typedef struct
{
	struct
    {
        uint8_t     *Buffer; //��������
        uint16_t    Size; //�������ݳ���
        IOSplit     Split; //��Ƭ���
        uint32_t    Count; //��ǰ��Ƭ��
        
    }               Input;
    
	struct
    {
        uint8_t     *Buffer; //�������
        uint16_t    Size; //������峤��
        uint16_t    Filled; //������ݳ���
        IOSplit     Split; //��Ƭ���
        uint32_t    Count; //��ǰ��Ƭ��
        
    }               Output;
    
	struct
    {
        uint32_t    Begin; //��������ʼ��Ŀ
        uint32_t    End; //������������Ŀ
        
    }               Iterator;
    
} ObjectPara;



/**
  * ����ģ��
  *
  */
typedef ObjectErrs (*TypeObject)(ObjectPara *P);


/**
  * ������ע��ṹ
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
