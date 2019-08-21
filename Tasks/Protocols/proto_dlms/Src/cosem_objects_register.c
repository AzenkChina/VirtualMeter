/**
 * @brief		
 * @details		
 * @date		azenk@2019-01-10
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "axdr.h"
#include "cosem_objects_register.h"
#include "types_metering.h"

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
static ObjectErrs RegisterGetLogicalName(ObjectPara *P)
{
    uint16_t Length;
    uint8_t Name[6] = {0};
    
    Length = axdr.encode(Name, sizeof(Name), AXDR_OCTET_STRING, OBJ_OUT_ADDR(P));
    
    if(!Length)
	{
		return(OBJECT_ERR_ENCODE);
	}
    
    OBJ_PUSH_LENGTH(P, Length);
    
    return(OBJECT_NOERR);
}

/**	
  * @brief 读取数值
  */
static ObjectErrs RegisterGetValue(ObjectPara *P)
{
    struct __meta_identifier id;
    struct __metering *api_metering;
    int64_t val = 0;
    uint16_t Length;
	
    M_U2ID(OBJ_IN_MID(P), id);
    
    api_metering = (struct __metering *)api("task_metering");
    if(!api_metering)
    {
        return(OBJECT_ERR_LOWLEVEL);
    }
    
    //使用id获取计量数据
    if(api_metering->instant(id, &val) == M_NULL)
    {
        return(OBJECT_ERR_LOWLEVEL);
    }
    
    //将获取到的计量数据进行AXDR组包
    Length = axdr.encode(&val, sizeof(val), AXDR_LONG64, OBJ_OUT_ADDR(P));
    if(!Length)
	{
		return(OBJECT_ERR_ENCODE);
	}
    
    OBJ_PUSH_LENGTH(P, Length);
    
    return(OBJECT_NOERR);
}

/**	
  * @brief 设置数值
  */
static ObjectErrs RegisterSetValue(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**	
  * @brief 读取单位和缩放
  */
static ObjectErrs RegisterGetScalerUnit(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**	
  * @brief 复位
  */
static ObjectErrs RegisterReset(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**	
  * @brief 对象列表
  */
const ClassRegister Register = 
{
    .GetLogicalName     = RegisterGetLogicalName,
    .SetLogicalName     = (TypeObject)0,
    .GetValue           = RegisterGetValue,
    .SetValue           = RegisterSetValue,
    .GetScalerUnit      = RegisterGetScalerUnit,
    .SetScalerUnit      = (TypeObject)0,
    .Reset              = RegisterReset,
};
