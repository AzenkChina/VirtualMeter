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
  * @brief 读取逻辑名
  */
static ObjectErrs GetLogicalName(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**	
  * @brief 设置逻辑名
  */
static ObjectErrs SetLogicalName(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**	
  * @brief 读取数值
  */
static ObjectErrs GetValue(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**	
  * @brief 设置数值
  */
static ObjectErrs SetValue(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**	
  * @brief 读取单位和缩放
  */
static ObjectErrs GetScalerUnit(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**	
  * @brief 读取单位和缩放
  */
static ObjectErrs SetScalerUnit(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**	
  * @brief 读取状态
  */
static ObjectErrs GetStatus(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**	
  * @brief 设置状态
  */
static ObjectErrs SetStatus(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**	
  * @brief 读取捕获时间
  */
static ObjectErrs GetCaptureTime(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**	
  * @brief 设置捕获时间
  */
static ObjectErrs SetCaptureTime(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**	
  * @brief 复位
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