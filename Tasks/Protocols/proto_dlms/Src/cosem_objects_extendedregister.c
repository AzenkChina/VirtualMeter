/**
 * @brief		
 * @details		
 * @date		azenk@2019-01-10
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "axdr.h"
#include "cosem_objects_extendedregister.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**	
  * @brief ��ȡ�߼���
  */
static ObjectErrs GetLogicalName(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**	
  * @brief �����߼���
  */
static ObjectErrs SetLogicalName(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**	
  * @brief ��ȡ��ֵ
  */
static ObjectErrs GetValue(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**	
  * @brief ������ֵ
  */
static ObjectErrs SetValue(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**	
  * @brief ��ȡ��λ������
  */
static ObjectErrs GetScalerUnit(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**	
  * @brief ��ȡ��λ������
  */
static ObjectErrs SetScalerUnit(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**	
  * @brief ��ȡ״̬
  */
static ObjectErrs GetStatus(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**	
  * @brief ����״̬
  */
static ObjectErrs SetStatus(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**	
  * @brief ��ȡ����ʱ��
  */
static ObjectErrs GetCaptureTime(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**	
  * @brief ���ò���ʱ��
  */
static ObjectErrs SetCaptureTime(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**	
  * @brief ��λ
  */
static ObjectErrs Reset(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}


const ClassExtRegister ExtRegister = 
{
    .GetLogicalName     = GetLogicalName,
    .SetLogicalName     = SetLogicalName,
    .GetValue           = GetValue,
    .SetValue           = SetValue,
    .GetScalerUnit      = GetScalerUnit,
    .SetScalerUnit      = SetScalerUnit,
    .GetStatus          = GetStatus,
    .SetStatus          = SetStatus,
    .GetCaptureTime     = GetCaptureTime,
    .SetCaptureTime     = SetCaptureTime,
    
    .Reset              = Reset,
};