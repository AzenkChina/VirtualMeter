/**
 * @brief		
 * @details		
 * @date		azenk@2019-01-10
 **/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __COSEM_OBJECTS_H__
#define __COSEM_OBJECTS_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "object_template.h"

/* Exported types ------------------------------------------------------------*/
/**
  * Data ¿‡
  *
  */
typedef struct
{
    TypeObject  GetLogicalName;
    TypeObject  SetLogicalName;
    
    TypeObject  GetValue;
    TypeObject  SetValue;
    
} ClassData;

/**
  * Register ¿‡
  *
  */
typedef struct
{
    TypeObject  GetLogicalName;
    TypeObject  SetLogicalName;
    
    TypeObject  GetValue;
    TypeObject  SetValue;
    
    TypeObject  GetScalerUnit;
    TypeObject  SetScalerUnit;
    
    TypeObject  Reset;
    
} ClassRegister;

/**
  * AssociationLN ¿‡
  *
  */
typedef struct
{
    TypeObject  GetLogicalName;
    TypeObject  SetLogicalName;
    
    TypeObject  GetObjectList;
    TypeObject  SetObjectList;
    
    TypeObject  GetAssociatedPartnersID;
    TypeObject  SetAssociatedPartnersID;
    
    TypeObject  GetApplicationContextName;
    TypeObject  SetApplicationContextName;
    
    TypeObject  GetXDLMSContextInfo;
    TypeObject  SetXDLMSContextInfo;
    
    TypeObject  GetAuthenticationMechanismName;
    TypeObject  SetAuthenticationMechanismName;
    
    TypeObject  GetSecret;
    TypeObject  SetSecret;
    
    TypeObject  GetAssociationStatus;
    TypeObject  SetAssociationStatus;
    
    TypeObject  GetSecuritySetupReference;
    TypeObject  SetSecuritySetupReference;
    
    TypeObject  GetUserList;
    TypeObject  SetUserList;
    
    TypeObject  GetCurrentUser;
    TypeObject  SetCurrentUser;
    
    TypeObject  ReplytoHLSAuthentication;
    TypeObject  ChangeHLSSecret;
    TypeObject  AddObject;
    TypeObject  RemoveObject;
    TypeObject  AddUser;
    TypeObject  RemoveUser;
    
} ClassAssociationLN;


/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern TypeObject CosemLoadAttrGet(uint16_t classid, uint8_t index);
extern TypeObject CosemLoadAttrSet(uint16_t classid, uint8_t index);
extern TypeObject CosemLoadMethod(uint16_t classid, uint8_t index);

#endif /* __COSEM_OBJECTS_H__ */
