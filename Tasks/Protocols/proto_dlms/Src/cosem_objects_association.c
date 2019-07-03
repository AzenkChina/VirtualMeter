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
static ObjectErrs AssociationLNSetSecret(ObjectPara *P)
{
    
}

/**	
  * @brief 
  */
static ObjectErrs AssociationLNGetApplicationContextName(ObjectPara *P)
{
    
}

/**	
  * @brief 
  */
static ObjectErrs AssociationLNGetxDLMSContextInfo(ObjectPara *P)
{
    
}

/**	
  * @brief 
  */
static ObjectErrs AssociationLNGetAuthenticationMechanismName(ObjectPara *P)
{
    
}

/**	
  * @brief 
  */
static ObjectErrs AssociationLNGetAssociationStatus(ObjectPara *P)
{
    
}

/**	
  * @brief 
  */
static ObjectErrs AssociationLNActionreplyToHLSAuthentication(ObjectPara *P)
{
    
}

/**	
  * @brief 
  */
static ObjectErrs AssociationLNGetObjectList(ObjectPara *P)
{
    
}

/**	
  * @brief 
  */
static ObjectErrs AssociationLNGetAssociatedPartnersID(ObjectPara *P)
{
    
}

/**	
  * @brief 
  */
static ObjectErrs AssociationLNSecuritySetupReference(ObjectPara *P)
{
    
}

/**	
  * @brief 
  */
static ObjectErrs AssociationLNUserList(ObjectPara *P)
{
    
}

/**	
  * @brief 
  */
static ObjectErrs AssociationLNCurrentUser(ObjectPara *P)
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
const ObjectCluster CosemObjectsAssociation = 
{
    .Name = "Association",
    .Amount = sizeof(ObjectList) / sizeof(TypeObject),
    .Func = ObjectList,
};
