/**
 * @brief		
 * @details		
 * @date		2019-08-18
 **/

/* Includes ------------------------------------------------------------------*/
#include "mids.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

int64_t __mids_scaling(int64_t val, enum __meta_scale scale)
{
	switch((scale)&0x1f)
	{
		case M_SCALE_ZP:
			(val) = (val)/1000;break;
		case M_SCALE_N1:
			val = (val)/100;break;
		case M_SCALE_N2:
			(val) = (val)/10;break;
		case M_SCALE_N3: 
			break;
		case M_SCALE_ZN:
			(val) = (val)*1000;break;
		default:
		{
			if((((scale)&0x1f) >= M_SCALE_P1) && (((scale)&0x1f) <= M_SCALE_P8))
			{
				uint8_t i;
				(val)=(val)/1000;
				for(i=0; i<((scale)&0x1f); i++)
				{
					(val)=(val)/10;
				}
			}
			else if((((scale)&0x1f) >= M_SCALE_N8) && (((scale)&0x1f) <= M_SCALE_N4))
			{
				uint8_t i;
				for(i=3; i<((~((scale)&0x1f))&0x1f); i++)
				{
					(val)=(val)*10;
				}
			}
		}
	}
    
    return(val);
}