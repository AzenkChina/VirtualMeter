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
  * @brief ��ȡ���β�˲ʱ��
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
    
    //ʹ��id��ȡ��������
    if(api_metering->instant(id, &val) == M_NULL)
    {
        return(OBJECT_ERR_LOWLEVEL);
    }
    
    //����ȡ���ļ������ݽ���AXDR���
    Length = axdr.encode(&val, sizeof(val), AXDR_LONG64, OBJ_OUT_ADDR(P));
    if(!Length)
	{
		return(OBJECT_ERR_ENCODE);
	}
    
    OBJ_PUSH_LENGTH(P, Length);
    
    return(OBJECT_NOERR);
}

/**	
  * @brief �����б�
  */
static const TypeObject ObjectList[] = 
{
    MeteringReadSecondaryInstant,
};




/**	
  * @brief �����������ע��
  */
const ObjectCluster CosemObjectsRegister = 
{
    .Name = "Register",
    .Amount = sizeof(ObjectList) / sizeof(TypeObject),
    .Func = ObjectList,
};
