/**
 * @brief		
 * @details		
 * @date		azenk@2019-07-31
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "axdr.h"
#include "cosem_objects_exception.h"
#include "dlms_application.h"

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
static ObjectErrs ExceptionUndefined(ObjectPara *P)
{
    return((ObjectErrs)4);
}

/**	
  * @brief 
  */
static ObjectErrs ExceptionDenied(ObjectPara *P)
{
    return((ObjectErrs)3);
}

/**	
  * @brief 
  */
static ObjectErrs ExceptionLogicalName(ObjectPara *P)
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
  * @brief 对象列表
  */
static const TypeObject ObjectList[] = 
{
    ExceptionUndefined,
	ExceptionDenied,
    ExceptionLogicalName,
};
