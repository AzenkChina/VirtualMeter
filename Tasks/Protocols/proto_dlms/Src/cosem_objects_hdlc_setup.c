/**
 * @brief		
 * @details		
 * @date		azenk@2019-01-10
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "axdr.h"
#include "cosem_objects_hdlc_setup.h"

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
    return(OBJECT_ERR_NODEF);
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
static ObjectErrs GetCommSpeed(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetCommSpeed(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetWindowSizeTransmit(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetWindowSizeTransmit(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetWindowSizeReceive(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetWindowSizeReceive(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetMaxInfoFieldLengthTransmit(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetMaxInfoFieldLengthTransmit(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetMaxInfoFieldLengthReceive(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetMaxInfoFieldLengthReceive(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetInterOctetTimeOut(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetInterOctetTimeOut(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetInactivityTimeOut(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetInactivityTimeOut(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetDeviceAddress(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetDeviceAddress(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}



const ClassHDLCSetup HDLCSetup = 
{
    .GetLogicalName                 = GetLogicalName,
    .SetLogicalName                 = SetLogicalName,
    .GetCommSpeed                   = GetCommSpeed,
    .SetCommSpeed                   = SetCommSpeed,
    .GetWindowSizeTransmit          = GetWindowSizeTransmit,
    .SetWindowSizeTransmit          = SetWindowSizeTransmit,
    .GetWindowSizeReceive           = GetWindowSizeReceive,
    .SetWindowSizeReceive           = SetWindowSizeReceive,
    .GetMaxInfoFieldLengthTransmit  = GetMaxInfoFieldLengthTransmit,
    .SetMaxInfoFieldLengthTransmit  = SetMaxInfoFieldLengthTransmit,
    .GetMaxInfoFieldLengthReceive   = GetMaxInfoFieldLengthReceive,
    .SetMaxInfoFieldLengthReceive   = SetMaxInfoFieldLengthReceive,
    .GetInterOctetTimeOut           = GetInterOctetTimeOut,
    .SetInterOctetTimeOut           = SetInterOctetTimeOut,
    .GetInactivityTimeOut           = GetInactivityTimeOut,
    .SetInactivityTimeOut           = SetInactivityTimeOut,
    .GetDeviceAddress               = GetDeviceAddress,
    .SetDeviceAddress               = SetDeviceAddress,
};
