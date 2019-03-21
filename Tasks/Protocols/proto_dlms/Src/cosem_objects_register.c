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
static TypeObjectErrs MeteringReadSecondaryInstant(TypeObjectParam *P)
{
    enum __axdr_type type;
    uint32_t param = 0;
    struct __metering_identifier id;
    struct __metering *api_metering;
    int64_t val = 0;
    uint16_t Length;
    
    //解析出输入参数的数据类型
    type = axdr.type.decode(OBJ_IN_ADDR(P));
    
    //判断数据类型是否匹配（获取计量数据使用的数据标识是一个uint16_t类型）
	if(type != AXDR_DOUBLE_LONG_UNSIGNED)
	{
		return(OBJECT_ERR_TYPE);
	}
    
    //解析出输入参数中的数据
    axdr.decode(OBJ_IN_ADDR(P), 0, (void *)&param);
	
    M_UINT2ID(param, id);
    
    api_metering = (struct __metering *)api.query("task_metering");
    if(!api_metering)
    {
        return(OBJECT_ERR_LOWLEVEL);
    }
    
    //使用id获取计量数据
    if(api_metering->instant(id, &val) == M_NULL)
    {
        api.release();
        return(OBJECT_ERR_LOWLEVEL);
    }
    
    api.release();
    
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
const TypeObjectPackage CosemObjectsRegister = 
{
    .Name = "Register",
    .Amount = sizeof(ObjectList) / sizeof(TypeObject),
    .Func = ObjectList,
};
