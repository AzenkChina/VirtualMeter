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
#include "mbedtls/ecdsa.h"
#include "mbedtls/sha256.h"
#include "mbedtls/bignum.h"
#include "dlms_association.h"
#include "dlms_application.h"
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
static int rng(void *p, unsigned char *buf, size_t blen)
{
	dlms_asso_random(blen, buf);
	
	return(0);
}

/**	
  * @brief 
  */
static ObjectErrs ReplytoHLSAuthentication(ObjectPara *P)
{
	struct __object_identifier identifier;
    uint8_t message[8+8+64+64];
    uint8_t len_message;
    int ret = 0;
    
	memset(&identifier, 0, sizeof(identifier));
	dlms_asso_mechname((uint8_t *)&identifier);
	
	switch(identifier.id)
	{
		case 5:
		{
			mbedtls_gcm_context ctx;
			uint8_t ekey[32];
			uint8_t len_ekey;
			uint8_t iv[32];
			uint8_t len_iv;
			uint8_t tag[12];
			
			if(OBJ_IN_SIZE(P) != 19)
			{
				return(OBJECT_ERR_DATA);
			}
			
			len_ekey = dlms_asso_ekey(ekey);
			
			message[0] = OBJ_IN_ADDR(P)[2];
			len_message = 1;
			len_message += dlms_asso_akey(&message[1]);
			len_message += dlms_asso_stoc(&message[len_message]);
			
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
											message,
											len_message,
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
			
			message[0] = 0x10;
			len_message = 1;
			len_message += dlms_asso_akey(&message[1]);
			len_message += dlms_asso_ctos(&message[len_message]);
			
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
											message,
											len_message,
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
			break;
		}
		case 7:
		{
			mbedtls_ecdsa_context ctx;
			mbedtls_mpi r, s;
			uint8_t hash[32];
			uint8_t ssprikey[48];
			uint8_t len_ssprikey;
			uint8_t cspubkey[96];
			uint8_t len_cspubkey;
			uint8_t z[48];
			
			len_ssprikey = dlms_asso_ssprikey(ssprikey);
			len_cspubkey = dlms_asso_cspubkey(cspubkey);
			
			if(((len_ssprikey != 32) && (len_ssprikey != 48)) || ((len_cspubkey != 64) && (len_cspubkey != 96)))
			{
				return(OBJECT_ERR_LOWLEVEL);
			}
			
			if(len_ssprikey != (len_cspubkey / 2))
			{
				return(OBJECT_ERR_LOWLEVEL);
			}
			
			if(OBJ_IN_ADDR(P)[1] != len_cspubkey)
			{
				return(OBJECT_ERR_LOWLEVEL);
			}
			
			memset(z, 0, sizeof(z));
			z[len_ssprikey - 1] = 1;
			
			mbedtls_mpi_init( &r );
			mbedtls_mpi_init( &s );
			mbedtls_ecp_keypair_init( &ctx );
			mbedtls_ecp_group_load( &ctx.grp, MBEDTLS_ECP_DP_SECP256R1 );
			
			if((ret = mbedtls_mpi_read_binary(&ctx.d, ssprikey, len_ssprikey)) != 0)
			{
				goto cleanup;
			}
			if((ret = mbedtls_mpi_read_binary( &ctx.Q.X, &cspubkey[0], len_ssprikey)) != 0)
			{
				goto cleanup;
			}
			if((ret = mbedtls_mpi_read_binary( &ctx.Q.Y, &cspubkey[len_cspubkey/2], len_ssprikey)) != 0)
			{
				goto cleanup;
			}
			if((ret = mbedtls_mpi_read_binary( &ctx.Q.Z, z, len_ssprikey)) != 0)
			{
				goto cleanup;
			}
			
			if((ret = mbedtls_mpi_read_binary(&r, &OBJ_IN_ADDR(P)[2+0], len_ssprikey)) != 0)
			{
				goto cleanup;
			}
			if((ret = mbedtls_mpi_read_binary(&s, &OBJ_IN_ADDR(P)[2+len_ssprikey], len_ssprikey)) != 0)
			{
				goto cleanup;
			}
			
			len_message = dlms_asso_callingtitle(&message[0]);
			len_message += dlms_asso_localtitle(&message[8]);
			len_message += dlms_asso_stoc(&message[16]);
			len_message += dlms_asso_ctos(&message[16 + len_message]);
			
			if( ( ret = mbedtls_sha256_ret( message, len_message, hash, 0 ) ) != 0 )
			{
			    goto cleanup;
			}
			
		    if( ( ret = mbedtls_ecdsa_verify( &ctx.grp, hash, sizeof( hash ), \
												 &ctx.Q, &r, &s) ) != 0 )
			{
		        goto cleanup;
			}
			else
			{
			    mbedtls_mpi_free( &r );
			    mbedtls_mpi_free( &s );
				mbedtls_mpi_init( &r );
				mbedtls_mpi_init( &s );
				
				len_message = dlms_asso_localtitle(&message[0]);
				len_message += dlms_asso_callingtitle(&message[8]);
				len_message += dlms_asso_ctos(&message[16]);
				len_message += dlms_asso_stoc(&message[16 + len_message]);
				
				if( ( ret = mbedtls_sha256_ret( message, len_message, hash, 0 ) ) != 0 )
				{
				    goto cleanup;
				}
				
				if( ( ret = mbedtls_ecdsa_sign( &ctx.grp, &r, &s, &ctx.d, hash, sizeof( hash ), \
												   rng, NULL) ) != 0 )
			    {
			        goto cleanup;
			    }
				else
				{
					axdr.type.encode(AXDR_OCTET_STRING, &OBJ_OUT_ADDR(P)[0]);
					axdr.length.encode(len_cspubkey, &OBJ_OUT_ADDR(P)[1]);
					
					if( ( ret = mbedtls_mpi_write_binary( &r, &OBJ_OUT_ADDR(P)[2 + 0], len_ssprikey ) ) != 0 )
					{
						goto cleanup;
					}
					if( ( ret = mbedtls_mpi_write_binary( &s, &OBJ_OUT_ADDR(P)[2 + len_ssprikey], len_ssprikey ) ) != 0 )
					{
						goto cleanup;
					}
					
					OBJ_PUSH_LENGTH(P, (2 + len_cspubkey));
					
					dlms_asso_accept_fctos();
				}
cleanup:
			    mbedtls_mpi_free( &r );
			    mbedtls_mpi_free( &s );
				mbedtls_ecdsa_free( &ctx );
				if(ret != 0)
				{
					return(OBJECT_ERR_NODEF);
				}
			}
			
			break;
		}
	}
    
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
