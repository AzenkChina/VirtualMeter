/**
 * @brief		
 * @details		
 * @date		azenk@2019-01-10
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "axdr.h"
#include "dlms_application.h"
#include "cosem_objects_security_setup.h"

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
  * @brief 
  */
static ObjectErrs SetLogicalName(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetSecurityPolicy(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetSecurityPolicy(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetSecuritySuite(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetSecuritySuite(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetClientSystemTitle(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetClientSystemTitle(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetServerSystemTitle(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetServerSystemTitle(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetCertificates(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetCertificates(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetSecurityActivate(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetSecurityActivate(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}


/**	
  * @brief 
  */
static ObjectErrs KeyTransfer(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs KeyAgreement(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GenerateKeyPair(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GenerateCertificateRequest(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs ImportCertificate(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs ExportCertificate(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs RemoveCertificate(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}


const ClassSecuritySetup SecuritySetup = 
{
    .GetLogicalName             = GetLogicalName,
    .SetLogicalName             = SetLogicalName,
    .GetSecurityPolicy          = GetSecurityPolicy,
    .SetSecurityPolicy          = SetSecurityPolicy,
    .GetSecuritySuite           = GetSecuritySuite,
    .SetSecuritySuite           = SetSecuritySuite,
    .GetClientSystemTitle       = GetClientSystemTitle,
    .SetClientSystemTitle       = SetClientSystemTitle,
    .GetServerSystemTitle       = GetServerSystemTitle,
    .SetServerSystemTitle       = SetServerSystemTitle,
    .GetCertificates            = GetCertificates,
    .SetCertificates            = SetCertificates,
    .GetSecurityActivate        = GetSecurityActivate,
    .SetSecurityActivate        = SetSecurityActivate,
    
    .KeyTransfer                = KeyTransfer,
    .KeyAgreement               = KeyAgreement,
    .GenerateKeyPair            = GenerateKeyPair,
    .GenerateCertificateRequest = GenerateCertificateRequest,
    .ImportCertificate          = ImportCertificate,
    .ExportCertificate          = ExportCertificate,
    .RemoveCertificate          = RemoveCertificate,
};
