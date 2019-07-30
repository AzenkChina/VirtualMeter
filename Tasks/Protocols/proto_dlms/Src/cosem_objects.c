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
#include "cosem_objects_data.h"
#include "cosem_objects_extendedregister.h"
#include "cosem_objects_hdlc_setup.h"
#include "cosem_objects_register.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** �����������������ģ������Ļ��������б� */
static const ObjectCluster *ObjectClusterTable[] = 
{
    (ObjectCluster *)&CosemObjectsAssociation,
    (ObjectCluster *)&CosemObjectsRegister,
};

/* Private macro -------------------------------------------------------------*/
#define FUNC_LIST_AMOUNT        ((uint16_t)(sizeof(ObjectClusterTable) / sizeof(ObjectCluster *)))

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * ��ȡһ����������
  *
  */
TypeObject cosem_load_object(const char *table, uint8_t index)
{
    uint16_t cnt;
    
    if(!table)
    {
        return((TypeObject)0);
    }
    
    for(cnt=0; cnt<FUNC_LIST_AMOUNT; cnt++)
    {
    	if(!ObjectClusterTable[cnt])
    	{
    		continue;
		}
		
    	if(!ObjectClusterTable[cnt]->Name)
    	{
    		continue;
		}
		
        if(strcmp(table, ObjectClusterTable[cnt]->Name) != 0)
        {
            continue;
        }
        
        if(index >= ObjectClusterTable[cnt]->Amount)
        {
            return((TypeObject)0);
        }
        
        return(ObjectClusterTable[cnt]->Func[index]);
    }
    
    return((TypeObject)0);
}

