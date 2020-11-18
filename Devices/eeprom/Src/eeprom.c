/**
 * @brief		
 * @details		
 * @date		2016-11-15
 **/

/* Includes ------------------------------------------------------------------*/
#include "eeprom.h"
#include "eeprom_1.h"
#include "eeprom_2.h"
#include "trace.h"

#if defined (DEMO_STM32F091)
#include "stm32f0xx.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
//页大小，注意两片eeprom页大小是否一致
#define EEP_PAGE_SIZE          ((uint32_t)(eeprom_1.info.pagesize()))

//页数量
#define EEP_PAGE_AMOUNT        ((uint32_t)((eeprom_1.info.pageamount() + eeprom_2.info.pageamount())))

//芯片容量
#define EEP_CHIP_SIZE          ((uint32_t)(EEP_PAGE_SIZE * EEP_PAGE_AMOUNT))

/* Private variables ---------------------------------------------------------*/
static enum __dev_status status = DEVICE_NOTINIT;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  
  */
static enum __dev_status eep_status(void)
{
    return(status);
}

/**
  * @brief  
  */
static void eep_init(enum __dev_state state)
{
#if defined (DEMO_STM32F091)
    GPIO_InitTypeDef GPIO_InitStruct;
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
    
    //PD9 n power
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
    GPIO_Init(GPIOD, &GPIO_InitStruct);
    
    GPIO_ResetBits(GPIOD, GPIO_Pin_9);
#endif
    
	eeprom_1.control.init(state);
	eeprom_2.control.init(state);
    status = DEVICE_INIT;
    
    ASSERT(eeprom_1.info.pagesize() != eeprom_2.info.pagesize());
}

/**
  * @brief  
  */
static void eep_suspend(void)
{
	eeprom_1.control.suspend();
	eeprom_2.control.suspend();
    
    status = DEVICE_SUSPENDED;
}

static uint32_t eep_page_read(uint32_t page, uint16_t offset, uint16_t size, uint8_t * buffer)
{
	if(page < eeprom_1.info.pageamount())
	{
		return(eeprom_1.page.read(page, offset, size, buffer));
	}
	else
	{
		return(eeprom_2.page.read((page - eeprom_1.info.pageamount()), offset, size, buffer));
	}
}

static uint32_t eep_page_write(uint32_t page, uint16_t offset, uint16_t size, const uint8_t *buffer)
{
	if(page < eeprom_1.info.pageamount())
	{
		return(eeprom_1.page.write(page, offset, size, buffer));
	}
	else
	{
		return(eeprom_2.page.write((page - eeprom_1.info.pageamount()), offset, size, buffer));
	}
}

static uint32_t eep_erase(void)
{
	eeprom_1.erase();
	eeprom_2.erase();
	return(EEP_CHIP_SIZE);
}

static uint32_t eep_pagesize(void)
{
    return(EEP_PAGE_SIZE);
}

static uint32_t eep_pageamount(void)
{
    return(EEP_PAGE_AMOUNT);
}

static uint32_t eep_chipsize(void)
{
    return(EEP_CHIP_SIZE);
}

/**
  * @brief  
  */
const struct __eeprom eeprom = 
{
    .control        = 
    {
        .name       = "eeprom",
        .status     = eep_status,
        .init       = eep_init,
        .suspend    = eep_suspend,
    },
    
    .page           = 
    {
        .read       = eep_page_read,
        .write      = eep_page_write,
    },
    
    .info           = 
    {
        .pagesize   = eep_pagesize,
        .pageamount = eep_pageamount,
        .chipsize   = eep_chipsize,
    },
    
    .erase          = eep_erase,
};
