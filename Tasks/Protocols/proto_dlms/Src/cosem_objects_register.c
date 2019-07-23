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
  * @brief 读取二次侧瞬时量
  */
static ObjectErrs MeteringReadSecondaryInstant(ObjectPara *P)
{
    struct __metering_identifier id;
    struct __metering *api_metering;
    int64_t val = 0;
    uint16_t Length;
	
    M_UINT2ID(OBJ_IN_ID(P), id);
    
    api_metering = (struct __metering *)api_query("task_metering");
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
  * @brief 对象列表
  */
static const TypeObject ObjectList[] = 
{
    MeteringReadSecondaryInstant,
};




/**	
  * @brief 对象包，用于注册
  */
const ObjectCluster CosemObjectsRegister = 
{
    .Name = "Register",
    .Amount = sizeof(ObjectList) / sizeof(TypeObject),
    .Func = ObjectList,
};
