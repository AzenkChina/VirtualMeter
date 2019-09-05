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
    uint8_t Value;
    
    if(OBJ_IN_OID(P) == 0xffffff00)
    {
        Value = 1;
        Length = axdr.encode(&Value, sizeof(Value), AXDR_ENUM, OBJ_OUT_ADDR(P));
        
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
    uint8_t Value;
    
    if(OBJ_IN_OID(P) == 0xffffff00)
    {
        Value = 1;
        Length = axdr.encode(&Value, sizeof(Value), AXDR_ENUM, OBJ_OUT_ADDR(P));
        
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
    uint8_t *Buffer = OBJ_OUT_ADDR(P);
    uint8_t Signature[16] = {0};
    uint16_t MetaLength;
    uint16_t Length;
    uint32_t Value;
    
    if(OBJ_IN_OID(P) == 0xffffff00)
    {
        Value = dlms_lex_amount(0xff);
        Value += 1;
        Value *= 96;
        
        if(OBJ_OUT_SIZE(P) < 36)
        {
            return(OBJECT_ERR_MEM);
        }
        
        if(!dlms_lex_signature(Signature))
        {
            return(OBJECT_ERR_LOWLEVEL);
        }
        
        HEAP_FORWARD(Buffer, AXDR_ARRAY);
        HEAP_FORWARD(Buffer, 1);
        HEAP_FORWARD(Buffer, AXDR_STRUCTURE);
        HEAP_FORWARD(Buffer, 3);
        Length = 4;
        
        MetaLength = axdr.encode(&Value, sizeof(Value), AXDR_DOUBLE_LONG_UNSIGNED, Buffer);
        Length += MetaLength;
        Buffer += MetaLength;
        MetaLength += axdr.encode("lexicon", 7, AXDR_OCTET_STRING, Buffer);
        Length += MetaLength;
        Buffer += MetaLength;
        MetaLength += axdr.encode(&Signature, sizeof(Signature), AXDR_OCTET_STRING, Buffer);
        Length += MetaLength;
        Buffer += MetaLength;
        
        OBJ_PUSH_LENGTH(P, Length);
        
        return(OBJECT_NOERR);
    }
    
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
    uint16_t Length;
    enum __axdr_type Type;
    union __axdr_container Value;
    
    if(OBJ_IN_OID(P) == 0xffffff00)
    {
        if(axdr.type.decode(OBJ_IN_ADDR(P)) != AXDR_STRUCTURE)
        {
            return(OBJECT_ERR_NODEF);
        }
        
        if(OBJ_IN_ADDR(P)[1] != 2)
        {
            return(OBJECT_ERR_NODEF);
        }
        
        if(axdr.type.decode(&OBJ_IN_ADDR(P)[2]) != AXDR_OCTET_STRING)
        {
            return(OBJECT_ERR_NODEF);
        }
        
        axdr.length.decode(&OBJ_IN_ADDR(P)[3], &Length);
        if((Length != 7) && (memcmp("lexicon", &OBJ_IN_ADDR(P)[4], 7) != 0))
        {
            return(OBJECT_ERR_NODEF);
        }
        
        axdr.decode(&OBJ_IN_ADDR(P)[11], &Type, &Value);
        if(Type != AXDR_DOUBLE_LONG_UNSIGNED)
        {
            return(OBJECT_ERR_NODEF);
        }
        
        if((Value.u32_t < 96) || (Value.u32_t > (file.size("lexicon"))))
        {
            return(OBJECT_ERR_NODEF);
        }
        
        if(Value.u32_t % 96)
        {
            return(OBJECT_ERR_NODEF);
        }
        
        return(OBJECT_NOERR);
    }
    
    return(OBJECT_ERR_LOWLEVEL);
}

/**
  * 
  *
  */
static ObjectErrs BlockTransfer(ObjectPara *P)
{
    uint16_t Length;
    enum __axdr_type Type;
    union __axdr_container Value;
    
    if(OBJ_IN_OID(P) == 0xffffff00)
    {
        if(OBJ_IN_SIZE(P) != 105)
        {
            return(OBJECT_ERR_NODEF);
        }
        
        if(axdr.type.decode(OBJ_IN_ADDR(P)) != AXDR_STRUCTURE)
        {
            return(OBJECT_ERR_NODEF);
        }
        
        if(OBJ_IN_ADDR(P)[1] != 2)
        {
            return(OBJECT_ERR_NODEF);
        }
        
        axdr.decode(&OBJ_IN_ADDR(P)[2], &Type, &Value);
        if(Type != AXDR_DOUBLE_LONG_UNSIGNED)
        {
            return(OBJECT_ERR_NODEF);
        }
        
        if(Value.u32_t >= (file.size("lexicon") / 96))
        {
            return(OBJECT_ERR_NODEF);
        }
        
        if(axdr.type.decode(&OBJ_IN_ADDR(P)[7]) != AXDR_OCTET_STRING)
        {
            return(OBJECT_ERR_NODEF);
        }
        
        axdr.length.decode(&OBJ_IN_ADDR(P)[8], &Length);
        if(Length != 96)
        {
            return(OBJECT_ERR_NODEF);
        }
        
        if(file.write("lexicon", (Value.u32_t * 96), 96, &OBJ_IN_ADDR(P)[9]) != 96)
        {
            return(OBJECT_ERR_NODEF);
        }
        
        if(Value.u32_t == 0)
        {
            dlms_lex_deactive();
        }
        
        return(OBJECT_NOERR);
    }
    
    return(OBJECT_ERR_LOWLEVEL);
}

/**
  * 
  *
  */
static ObjectErrs Verify(ObjectPara *P)
{
    if(OBJ_IN_OID(P) == 0xffffff00)
    {
        dlms_lex_active();
        
        if(dlms_lex_check())
        {
            return(OBJECT_NOERR);
        }
        
        return(OBJECT_ERR_NODEF);
    }
    
    return(OBJECT_ERR_LOWLEVEL);
}

/**
  * 
  *
  */
static ObjectErrs Activate(ObjectPara *P)
{
    if(OBJ_IN_OID(P) == 0xffffff00)
    {
        return(OBJECT_NOERR);
    }
    
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
