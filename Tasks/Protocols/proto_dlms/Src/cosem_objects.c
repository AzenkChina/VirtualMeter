/**
 * @brief		
 * @details		
 * @date		azenk@2019-01-10
 **/

/* Includes ------------------------------------------------------------------*/
#include "cosem_objects.h"
#include "string.h"

/** �����������������ģ������Ļ��������б��Ӧͷ�ļ� */
#include "cosem_objects_association.h"
#include "cosem_objects_clock.h"
#include "cosem_objects_hdlc_setup.h"
#include "cosem_objects_data.h"
#include "cosem_objects_register.h"
#include "cosem_objects_extendedregister.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** �����������������ģ������Ļ��������б� */
static const TypeObjectPackage *ObjectPackageTable[] = 
{
    (TypeObjectPackage *)0,
};

/* Private macro -------------------------------------------------------------*/
#define FUNC_LIST_AMOUNT        ((uint16_t)(sizeof(ObjectPackageTable) / sizeof(TypeObjectPackage *)))

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * ��ȡһ����������
  *
  */
TypeObject CosemObjectsQuery(const char *table, uint8_t index)
{
    uint16_t cnt;
    
    if(!table)
    {
        return((TypeObject)0);
    }
    
    for(cnt=0; cnt<FUNC_LIST_AMOUNT; cnt++)
    {
    	if(!ObjectPackageTable[cnt])
    	{
    		continue;
		}
		
    	if(!ObjectPackageTable[cnt]->Name)
    	{
    		continue;
		}
		
        if(strcmp(table, ObjectPackageTable[cnt]->Name) != 0)
        {
            continue;
        }
        
        if(index >= ObjectPackageTable[cnt]->Amount)
        {
            return((TypeObject)0);
        }
        
        return(ObjectPackageTable[cnt]->Func[index]);
    }
    
    return((TypeObject)0);
}

