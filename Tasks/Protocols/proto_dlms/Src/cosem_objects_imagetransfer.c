/**
 * @brief		
 * @details		
 * @date		azenk@2019-01-10
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "axdr.h"
#include "cosem_objects_imagetransfer.h"

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
    return(OBJECT_ERR_LOWLEVEL);
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
