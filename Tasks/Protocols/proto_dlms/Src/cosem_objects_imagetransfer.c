/**
 * @brief		
 * @details		
 * @date		azenk@2019-01-10
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "string.h"
#include "axdr.h"
#include "dlms_application.h"
#include "cosem_objects_imagetransfer.h"
#include "dlms_lexicon.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * 
  *
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
  * 
  *
  */
static ObjectErrs SetLogicalName(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**
  * 
  *
  */
static ObjectErrs GetBlockSize(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**
  * 
  *
  */
static ObjectErrs SetBlockSize(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**
  * 
  *
  */
static ObjectErrs GetTransferredBlocksStatus(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**
  * 
  *
  */
static ObjectErrs SetTransferredBlocksStatus(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**
  * 
  *
  */
static ObjectErrs GetFirstNotTransferredBlockNumber(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**
  * 
  *
  */
static ObjectErrs SetFirstNotTransferredBlockNumber(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**
  * 
  *
  */
static ObjectErrs GetTransferEnabled(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**
  * 
  *
  */
static ObjectErrs SetTransferEnabled(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**
  * 
  *
  */
static ObjectErrs GetTransferStatus(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**
  * 
  *
  */
static ObjectErrs SetTransferStatus(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**
  * 
  *
  */
static ObjectErrs GetActivateInfo(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**
  * 
  *
  */
static ObjectErrs SetActivateInfo(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**
  * 
  *
  */
static ObjectErrs Initiate(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**
  * 
  *
  */
static ObjectErrs BlockTransfer(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**
  * 
  *
  */
static ObjectErrs Verify(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**
  * 
  *
  */
static ObjectErrs Activate(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**	
  * @brief 对象列表
  */
const ClassImageTransfer ImageTransfer = 
{
	.GetLogicalName							= GetLogicalName,
    .SetLogicalName							= SetLogicalName,
    .GetBlockSize							= GetBlockSize,
    .SetBlockSize							= SetBlockSize,
    .GetTransferredBlocksStatus				= GetTransferredBlocksStatus,
    .SetTransferredBlocksStatus				= SetTransferredBlocksStatus,
    .GetFirstNotTransferredBlockNumber		= GetFirstNotTransferredBlockNumber,
    .SetFirstNotTransferredBlockNumber		= SetFirstNotTransferredBlockNumber,
    .GetTransferEnabled						= GetTransferEnabled,
    .SetTransferEnabled						= SetTransferEnabled,
    .GetTransferStatus						= GetTransferStatus,
    .SetTransferStatus						= SetTransferStatus,
    .GetActivateInfo						= GetActivateInfo,
    .SetActivateInfo						= SetActivateInfo,

    .Initiate								= Initiate,
    .BlockTransfer							= BlockTransfer,
    .Verify									= Verify,
    .Activate								= Activate,
};
