/**
 * @brief		
 * @details		
 * @date		azenk@2017-01-09
 **/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TYPES_TIMED_H__
#define __TYPES_TIMED_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  timed task ����
  */
enum __timd_status
{
	TIMD_SUCCESS = 0,
	TIMD_CONFLICT,
	TIMD_NOEXIST,
};

/**
  * @brief  timed task ����
  */
struct __timed_conf
{
    void                            (*callback)(void); //��ʱʱ�䵽�ص�����
    uint64_t						period; //��ʱ����
    uint16_t                        flags; //��־λ
};

/**
  * @brief  timed task �Ķ���ӿ�
  */
struct __timed
{
    enum __timd_status				(*create)(const struct __timed_conf *conf); //����һ����ʱ������
    enum __timd_status				(*query)(const struct __timed_conf *conf); //���������Ч��
    enum __timd_status				(*remove)(const struct __timed_conf *conf); //����һ����ʱ������
    enum __timd_status				(*empty)(void); //������ж�ʱ������
};

/* Exported constants --------------------------------------------------------*/
#define TIMD_FLAG_RELATIVE      ((uint16_t)(0x0000 << 0)) //ʹ�����ʱ��
#define TIMD_FLAG_ABSOLUTE      ((uint16_t)(0x0003 << 0)) //ʹ�þ���ʱ��

#define TIMD_FLAG_CHANGE        ((uint16_t)(0x0000 << 2)) //ϵͳʱ�ӱ��޸�ִ��
#define TIMD_FLAG_PERIODIC      ((uint16_t)(0x0001 << 2)) //����ִ��
#define TIMD_FLAG_ONCE          ((uint16_t)(0x0003 << 2)) //ִ��һ��

/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif /* __TYPES_TIMED_H__ */
