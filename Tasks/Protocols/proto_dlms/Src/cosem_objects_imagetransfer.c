/**
 * @brief		
 * @details		
 * @date		azenk@2019-01-10
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "axdr.h"
#include "dlms_application.h"
#include "cosem_objects_imagetransfer.h"

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief   
  */
enum __transfer_status
{
    TRANS_NOT_INIT = 0,
    TRANS_INITED = 1,
    VERI_INITED = 2,
    VERI_SUCCESS = 3,
    VERI_FAILED = 4,
    ACTIV_INITED = 5,
    ACTIV_SUCCESS = 6,
    ACTIV_FAILED = 7,
};

/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static enum __transfer_status TransferStatus = TRANS_NOT_INIT;

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
    
    dlms_appl_logicalname(Name);
    
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
    uint16_t Length;
    uint32_t BlockSize;
    
    if(OBJ_IN_OID(P) == 0xffffff00)
    {
        BlockSize = 96;
        Length = axdr.encode(&BlockSize, sizeof(BlockSize), AXDR_DOUBLE_LONG_UNSIGNED, OBJ_OUT_ADDR(P));
        
        if(!Length)
        {
            return(OBJECT_ERR_ENCODE);
        }
        
        OBJ_PUSH_LENGTH(P, Length);
        
        return(OBJECT_NOERR);
    }
    
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
    if(OBJ_IN_OID(P) == 0xffffff00)
    {
        OBJ_OUT_ADDR(P)[0] = AXDR_BIT_STRING;
        OBJ_OUT_ADDR(P)[1] = 0;
        OBJ_PUSH_LENGTH(P, 2);
        
        return(OBJECT_NOERR);
    }
    
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
    uint16_t Length;
    uint32_t BlockSize;
    
    if(OBJ_IN_OID(P) == 0xffffff00)
    {
        BlockSize = 0;
        Length = axdr.encode(&BlockSize, sizeof(BlockSize), AXDR_DOUBLE_LONG_UNSIGNED, OBJ_OUT_ADDR(P));
        
        if(!Length)
        {
            return(OBJECT_ERR_ENCODE);
        }
        
        OBJ_PUSH_LENGTH(P, Length);
        
        return(OBJECT_NOERR);
    }
    
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
    uint16_t Length;
    uint8_t EnableStatus;
    
    if(OBJ_IN_OID(P) == 0xffffff00)
    {
        EnableStatus = 1;
        Length = axdr.encode(&EnableStatus, sizeof(EnableStatus), AXDR_ENUM, OBJ_OUT_ADDR(P));
        
        if(!Length)
        {
            return(OBJECT_ERR_ENCODE);
        }
        
        OBJ_PUSH_LENGTH(P, Length);
        
        return(OBJECT_NOERR);
    }
    
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
    uint16_t Length;
    
    if(OBJ_IN_OID(P) == 0xffffff00)
    {
        Length = axdr.encode(&TransferStatus, sizeof(TransferStatus), AXDR_ENUM, OBJ_OUT_ADDR(P));
        
        if(!Length)
        {
            return(OBJECT_ERR_ENCODE);
        }
        
        OBJ_PUSH_LENGTH(P, Length);
        
        return(OBJECT_NOERR);
    }
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
