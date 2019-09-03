/**
 * @brief		
 * @details		
 * @date		azenk@2019-01-10
 **/

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "system.h"
#include "axdr.h"
#include "mbedtls/gcm.h"
#include "dlms_association.h"
#include "cosem_objects_association.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**	
  * @brief 读取逻辑名
  */
static ObjectErrs GetLogicalName(ObjectPara *P)
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
  * @brief 设置逻辑名
  */
static ObjectErrs SetLogicalName(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**	
  * @brief 
  */
static ObjectErrs GetObjectList(ObjectPara *P)
{
    /** 读取多条数据编程范例 */
    
    //判断迭代器是否已经在运行中
    if(!OBJ_IS_ITERATING(P))
    {
        //初始化迭代器
        OBJ_ITERATE_INIT(P, 0, 10);
    }
    
    //迭代读取数据条目
    for(; OBJ_ITERATE_FROM(P) != OBJ_ITERATE_TO(P);)
    {
        //...组包数据到输出缓冲
        
        
        
        //迭代器步进
        OBJ_ITERATE_STEPPING(P);
    }
    
    
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetObjectList(ObjectPara *P)
{
    return(OBJECT_ERR_LOWLEVEL);
}

/**	
  * @brief 
  */
static ObjectErrs GetAssociatedPartnersID(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetAssociatedPartnersID(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetApplicationContextName(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetApplicationContextName(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetxDLMSContextInfo(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetxDLMSContextInfo(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetAuthenticationMechanismName(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetAuthenticationMechanismName(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetSecret(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetSecret(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetAssociationStatus(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetAssociationStatus(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}


/**	
  * @brief 
  */
static ObjectErrs GetSecuritySetupReference(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetSecuritySetupReference(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetUserList(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetUserList(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetCurrentUser(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetCurrentUser(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs ReplytoHLSAuthentication(ObjectPara *P)
{
    uint8_t add[128];
    uint8_t len_add;
    uint8_t ekey[32];
    uint8_t len_ekey;
    uint8_t iv[32];
    uint8_t len_iv;
    uint8_t tag[12];
    mbedtls_gcm_context ctx;
    int ret = 0;
    
    if(OBJ_IN_SIZE(P) != 19)
    {
        return(OBJECT_ERR_DATA);
    }
    
    len_ekey = dlms_asso_ekey(ekey);
    
    add[0] = OBJ_IN_ADDR(P)[2];
    len_add = 1;
    len_add += dlms_asso_akey(&add[1]);
    len_add += dlms_asso_stoc(&add[len_add]);
    
    len_iv = dlms_asso_callingtitle(iv);
    len_iv += heap.copy(&iv[8], &OBJ_IN_ADDR(P)[3], 4);
    
    mbedtls_gcm_init(&ctx);
    
    ret = mbedtls_gcm_setkey(&ctx,
                             MBEDTLS_CIPHER_ID_AES,
                             ekey,
                             len_ekey*8);
    
    if(ret)
    {
        return(OBJECT_ERR_DECODE);
    }
    
    ret = mbedtls_gcm_crypt_and_tag(&ctx,
                                    MBEDTLS_GCM_ENCRYPT,
                                    0,
                                    iv,
                                    len_iv,
                                    add,
                                    len_add,
                                    (void *)0,
                                    (void *)0,
                                    sizeof(tag),
                                    tag);
    
    mbedtls_gcm_free(&ctx);
    
    if(ret)
    {
        return(OBJECT_ERR_DECODE);
    }
    
    if(memcmp(tag, &OBJ_IN_ADDR(P)[7], 12) != 0)
    {
        return((ObjectErrs)2);
    }
    
    add[0] = 0x10;
    len_add = 1;
    len_add += dlms_asso_akey(&add[1]);
    len_add += dlms_asso_ctos(&add[len_add]);
    
    len_iv = dlms_asso_localtitle(iv);
    len_iv += dlms_asso_fc(&iv[8]);
    
    mbedtls_gcm_init(&ctx);
    
    ret = mbedtls_gcm_setkey(&ctx,
                             MBEDTLS_CIPHER_ID_AES,
                             ekey,
                             len_ekey*8);
    
    if(ret)
    {
        return(OBJECT_ERR_ENCODE);
    }
    
    ret = mbedtls_gcm_crypt_and_tag(&ctx,
                                    MBEDTLS_GCM_ENCRYPT,
                                    0,
                                    iv,
                                    len_iv,
                                    add,
                                    len_add,
                                    (void *)0,
                                    (void *)0,
                                    sizeof(tag),
                                    tag);
    
    mbedtls_gcm_free(&ctx);
    
    if(ret)
    {
        return(OBJECT_ERR_ENCODE);
    }
    
    axdr.type.encode(AXDR_OCTET_STRING, &OBJ_OUT_ADDR(P)[0]);
    axdr.length.encode(17, &OBJ_OUT_ADDR(P)[1]);
    OBJ_OUT_ADDR(P)[2] = 0x10;
    heap.copy(&OBJ_OUT_ADDR(P)[3], &iv[8], 4);
    heap.copy(&OBJ_OUT_ADDR(P)[7], tag, 12);
    
    OBJ_PUSH_LENGTH(P, 19);
    
    return(OBJECT_NOERR);
}

/**	
  * @brief 
  */
static ObjectErrs ChangeHLSSecret(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs AddObject(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs RemoveObject(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs AddUser(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs RemoveUser(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}


const ClassAssociationLN AssociationLN = 
{
    .GetLogicalName                     = GetLogicalName,
    .SetLogicalName                     = SetLogicalName,
    .GetObjectList                      = GetObjectList,
    .SetObjectList                      = SetObjectList,
    .GetAssociatedPartnersID            = GetAssociatedPartnersID,
    .SetAssociatedPartnersID            = SetAssociatedPartnersID,
    .GetApplicationContextName          = GetApplicationContextName,
    .SetApplicationContextName          = SetApplicationContextName,
    .GetXDLMSContextInfo                = GetxDLMSContextInfo,
    .SetXDLMSContextInfo                = SetxDLMSContextInfo,
    .GetAuthenticationMechanismName     = GetAuthenticationMechanismName,
    .SetAuthenticationMechanismName     = SetAuthenticationMechanismName,
    .GetSecret                          = GetSecret,
    .SetSecret                          = SetSecret,
    .GetAssociationStatus               = GetAssociationStatus,
    .SetAssociationStatus               = SetAssociationStatus,
    .GetSecuritySetupReference          = GetSecuritySetupReference,
    .SetSecuritySetupReference          = SetSecuritySetupReference,
    .GetUserList                        = GetUserList,
    .SetUserList                        = SetUserList,
    .GetCurrentUser                     = GetCurrentUser,
    .SetCurrentUser                     = SetCurrentUser,
    
    .ReplytoHLSAuthentication           = ReplytoHLSAuthentication,
    .ChangeHLSSecret                    = ChangeHLSSecret,
    .AddObject                          = AddObject,
    .RemoveObject                       = RemoveObject,
    .AddUser                            = AddUser,
    .RemoveUser                         = RemoveUser,
};
