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
  * ��������
  *
  */
typedef enum
{
    ITER_NONE = 0,//�����е���
    ITER_ONGOING = 0x03,//���ڵ���
    ITER_FINISHED = 0x0c,//��������
    
} IterStatus;

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
        uint32_t    ID; //�ڲ����ݱ�ʶ
        
    }               Input;
    
	struct
    {
        uint8_t     *Buffer; //�������
        uint16_t    Size; //������峤��
        uint16_t    Filled; //������ݳ���
        
    }               Output;
    
	struct
    {
        uint32_t    Begin; //��������ʼ��Ŀ
        uint32_t    End; //������������Ŀ
        uint8_t     Status; //��������ǰ״̬
        
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
#define OBJ_IN_ADDR(P)                  ((const uint8_t *)(P->Input.Buffer))//��ȡ���������׵�ַ
#define OBJ_IN_SIZE(P)                  ((uint16_t)(P->Input.Size))//��ȡ���������ֽڳ���
#define OBJ_IN_ID(P)                    ((uint32_t)(P->Input.ID))//��ȡ���ݱ�ʶ

#define OBJ_OUT_ADDR(P)                 (P->Output.Buffer)//��ȡ��������׵�ַ
#define OBJ_OUT_SIZE(P)                 (P->Output.Size)//��ȡ��������ֽڴ�С
#define OBJ_PUSH_LENGTH(P, n)           (P->Output.Filled = n)//������������ֽڳ���

#define OBJ_ITERATE_BEGIN(P)            (P->Iterator.Begin)//��ȡ��������ʼ
#define OBJ_ITERATE_END(P)              (P->Iterator.End)//��ȡ����������
#define OBJ_ITERATE_STATUS(P)           ((IterStatus)(P->Iterator.Status))//��ȡ������״̬
#define OBJ_ITERATE_SET_BEGIN(P, v)     (P->Iterator.Begin = v)//���õ�������ʼ
#define OBJ_ITERATE_SET_END(P, v)       (P->Iterator.End = v)//���õ���������
#define OBJ_ITERATE_SET_STATUS(P, v)    (P->Iterator.Status = (IterStatus)v)//���õ�����״̬

/* Exported function prototypes ----------------------------------------------*/

#endif /* __OBJECT_TEMPLATE_H__ */
