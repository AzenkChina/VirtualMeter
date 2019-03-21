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
static TypeObjectErrs MeteringReadSecondaryInstant(TypeObjectParam *P)
{
    enum __axdr_type type;
    uint32_t param = 0;
    struct __metering_identifier id;
    struct __metering *api_metering;
    int64_t val = 0;
    uint16_t Length;
    
    //�����������������������
    type = axdr.type.decode(OBJ_IN_ADDR(P));
    
    //�ж����������Ƿ�ƥ�䣨��ȡ��������ʹ�õ����ݱ�ʶ��һ��uint16_t���ͣ�
	if(type != AXDR_DOUBLE_LONG_UNSIGNED)
	{
		return(OBJECT_ERR_TYPE);
	}
    
    //��������������е�����
    axdr.decode(OBJ_IN_ADDR(P), 0, (void *)&param);
	
    M_UINT2ID(param, id);
    
    api_metering = (struct __metering *)api.query("task_metering");
    if(!api_metering)
    {
        return(OBJECT_ERR_LOWLEVEL);
    }
    
    //ʹ��id��ȡ��������
    if(api_metering->instant(id, &val) == M_NULL)
    {
        api.release();
        return(OBJECT_ERR_LOWLEVEL);
    }
    
    api.release();
    
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
const TypeObjectPackage CosemObjectsRegister = 
{
    .Name = "Register",
    .Amount = sizeof(ObjectList) / sizeof(TypeObject),
    .Func = ObjectList,
};
