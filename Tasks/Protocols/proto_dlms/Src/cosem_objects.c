/**
 * @brief		
 * @details		
 * @date		azenk@2019-01-10
 **/

/* Includes ------------------------------------------------------------------*/
#include "cosem_objects.h"
#include "dlms_types.h"
#include "string.h"

/** 在这里添加所有任务模块输出的基础函数列表对应头文件 */
#include "cosem_objects_exception.h"
#include "cosem_objects_association.h"
#include "cosem_objects_clock.h"
#include "cosem_objects_data.h"
#include "cosem_objects_extendedregister.h"
#include "cosem_objects_hdlc_setup.h"
#include "cosem_objects_register.h"
#include "cosem_objects_imagetransfer.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * 获取一个属性
  *
  */
TypeObject CosemLoadAttribute(uint16_t ClassID, uint8_t Index, uint8_t Motive)
{
    TypeObject *Obj = (TypeObject *)0;
    
    if(!Index)
    {
        return((TypeObject)0);
    }
    
    if((Motive != MOTIV_GET) && (Motive != MOTIV_SET))
    {
        return((TypeObject)0);
    }
    
    switch(ClassID)
    {
        case CLASS_DATA:
        {
            if(Index > 2)
            {
                return((TypeObject)0);
            }
            else
            {
                Obj = (TypeObject *)&Data;
                Index -= 1;
                Index *= 2;
                Obj += Index;
                if(Motive == MOTIV_SET)
                {
                    Obj += 1;
                }
                return(*Obj);
            }
        }
        case CLASS_REGISTER:
        {
            if(Index > 3)
            {
                return((TypeObject)0);
            }
            else
            {
                Obj = (TypeObject *)&Register;
                Index -= 1;
                Index *= 2;
                Obj += Index;
                if(Motive == MOTIV_SET)
                {
                    Obj += 1;
                }
                return(*Obj);
            }
        }
        case CLASS_EXTREGISTER:
        {
            if(Index > 5)
            {
                return((TypeObject)0);
            }
            else
            {
                Obj = (TypeObject *)&ExtRegister;
                Index -= 1;
                Index *= 2;
                Obj += Index;
                if(Motive == MOTIV_SET)
                {
                    Obj += 1;
                }
                return(*Obj);
            }
        }
        case CLASS_CLOCK:
        {
            if(Index > 9)
            {
                return((TypeObject)0);
            }
            else
            {
                Obj = (TypeObject *)&Clock;
                Index -= 1;
                Index *= 2;
                Obj += Index;
                if(Motive == MOTIV_SET)
                {
                    Obj += 1;
                }
                return(*Obj);
            }
        }
        case CLASS_ASSOCIATION_LN:
        {
            if(Index > 11)
            {
                return((TypeObject)0);
            }
            else
            {
                Obj = (TypeObject *)&AssociationLN;
                Index -= 1;
                Index *= 2;
                Obj += Index;
                if(Motive == MOTIV_SET)
                {
                    Obj += 1;
                }
                return(*Obj);
            }
        }
        case CLASS_IMAGE_TRANSFER:
        {
            if(Index > 7)
            {
                return((TypeObject)0);
            }
            else
            {
                Obj = (TypeObject *)&ImageTransfer;
                Index -= 1;
                Index *= 2;
                Obj += Index;
                if(Motive == MOTIV_SET)
                {
                    Obj += 1;
                }
                return(*Obj);
            }
        }
        case CLASS_HDLC_SETUP:
        {
            if(Index > 9)
            {
                return((TypeObject)0);
            }
            else
            {
                Obj = (TypeObject *)&ImageTransfer;
                Index -= 1;
                Index *= 2;
                Obj += Index;
                if(Motive == MOTIV_SET)
                {
                    Obj += 1;
                }
                return(*Obj);
            }
        }
        default:
        {
            return((TypeObject)0);
        }
    }
}

/**
  * 获取一个方法
  *
  */
TypeObject CosemLoadMethod(uint16_t ClassID, uint8_t Index)
{
    TypeObject *Obj = (TypeObject *)0;
    
    if(!Index)
    {
        return((TypeObject)0);
    }
    
    switch(ClassID)
    {
        case CLASS_REGISTER:
        {
            if(Index > 1)
            {
                return((TypeObject)0);
            }
            else
            {
                Obj = (TypeObject *)&Register;
                Obj += 3 * 2;
                Index -= 1;
                Obj += Index;
                return(*Obj);
            }
        }
        case CLASS_EXTREGISTER:
        {
            if(Index > 1)
            {
                return((TypeObject)0);
            }
            else
            {
                Obj = (TypeObject *)&ExtRegister;
                Obj += 5 * 2;
                Index -= 1;
                Obj += Index;
                return(*Obj);
            }
        }
        case CLASS_CLOCK:
        {
            if(Index > 6)
            {
                return((TypeObject)0);
            }
            else
            {
                Obj = (TypeObject *)&Clock;
                Obj += 9 * 2;
                Index -= 1;
                Obj += Index;
                return(*Obj);
            }
        }
        case CLASS_ASSOCIATION_LN:
        {
            if(Index > 6)
            {
                return((TypeObject)0);
            }
            else
            {
                Obj = (TypeObject *)&AssociationLN;
                Obj += 11 * 2;
                Index -= 1;
                Obj += Index;
                return(*Obj);
            }
        }
        case CLASS_IMAGE_TRANSFER:
        {
            if(Index > 4)
            {
                return((TypeObject)0);
            }
            else
            {
                Obj = (TypeObject *)&ImageTransfer;
                Obj += 7 * 2;
                Index -= 1;
                Obj += Index;
                return(*Obj);
            }
        }
        default:
        {
            return((TypeObject)0);
        }
    }
}
