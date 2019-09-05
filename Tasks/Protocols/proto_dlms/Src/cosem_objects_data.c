/**
 * @brief		
 * @details		
 * @date		azenk@2019-01-10
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "axdr.h"
#include "dlms_application.h"
#include "cosem_objects_data.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**	
  * @brief 
  */
static ObjectErrs GetLogicalName(ObjectPara *P)
{
    uint16_t Length;
    uint8_t Name[6] = {0};
    
    if(!dlms_appl_instance(Name))
    {
        return(OBJECT_ERR_DATA);
    }
    
    Length = axdr.encode(Name, sizeof(Name), AXDR_OCTET_STRING, OBJ_OUT_ADDR(P));
    
    if(!Length)
	{
		return(OBJECT_ERR_ENCODE);
	}
    
    OBJ_PUSH_LENGTH(P, Length);
    
    return(OBJECT_NOERR);
}

/**	
  * @brief 
  */
static ObjectErrs SetLogicalName(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetValue(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetValue(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

const ClassData Data = 
{
    .GetLogicalName     = GetLogicalName,
    .SetLogicalName     = SetLogicalName,
    .GetValue           = GetValue,
    .SetValue           = SetValue,
};
