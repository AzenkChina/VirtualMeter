/**
 * @brief		
 * @details		
 * @date		azenk@2019-01-10
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "axdr.h"
#include "cosem_objects_association.h"


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
static TypeObjectErrs AssociationLNSetSecret(TypeObjectParam *P)
{
    
}

/**	
  * @brief 
  */
static TypeObjectErrs AssociationLNGetApplicationContextName(TypeObjectParam *P)
{
    
}

/**	
  * @brief 
  */
static TypeObjectErrs AssociationLNGetxDLMSContextInfo(TypeObjectParam *P)
{
    
}

/**	
  * @brief 
  */
static TypeObjectErrs AssociationLNGetAuthenticationMechanismName(TypeObjectParam *P)
{
    
}

/**	
  * @brief 
  */
static TypeObjectErrs AssociationLNGetAssociationStatus(TypeObjectParam *P)
{
    
}

/**	
  * @brief 
  */
static TypeObjectErrs AssociationLNActionreplyToHLSAuthentication(TypeObjectParam *P)
{
    
}

/**	
  * @brief 
  */
static TypeObjectErrs AssociationLNGetObjectList(TypeObjectParam *P)
{
    
}

/**	
  * @brief 
  */
static TypeObjectErrs AssociationLNGetAssociatedPartnersID(TypeObjectParam *P)
{
    
}

/**	
  * @brief 
  */
static TypeObjectErrs AssociationLNSecuritySetupReference(TypeObjectParam *P)
{
    
}

/**	
  * @brief 
  */
static TypeObjectErrs AssociationLNUserList(TypeObjectParam *P)
{
    
}

/**	
  * @brief 
  */
static TypeObjectErrs AssociationLNCurrentUser(TypeObjectParam *P)
{
    
}

/**	
  * @brief 对象列表
  */
static const TypeObject ObjectList[] = 
{
    AssociationLNSetSecret,
    AssociationLNGetApplicationContextName,
    AssociationLNGetxDLMSContextInfo,
    AssociationLNGetAuthenticationMechanismName,
    AssociationLNGetAssociationStatus,
    AssociationLNActionreplyToHLSAuthentication,
    AssociationLNGetObjectList,
    AssociationLNGetAssociatedPartnersID,
    AssociationLNSecuritySetupReference,
    AssociationLNUserList,
    AssociationLNCurrentUser,
};




/**	
  * @brief 对象包，用于注册
  */
const TypeObjectPackage CosemObjectsAssociation = 
{
    .Name = "Association",
    .Amount = sizeof(ObjectList) / sizeof(TypeObject),
    .Func = ObjectList,
};
