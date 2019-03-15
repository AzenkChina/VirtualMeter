/**
 * @brief		
 * @details		
 * @date		2016-11-15
 **/

/* Includes ------------------------------------------------------------------*/
#include "eeprom.h"
#include "eeprom_1.h"
#include "eeprom_2.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
//Page size
#define EEP_PAGE_SIZE          ((uint32_t)(eeprom_1.info.pagesize()))

//Page amount
#define EEP_PAGE_AMOUNT        ((uint32_t)(eeprom_1.info.pageamount() + eeprom_2.info.pageamount()))

//Chip size
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
	eeprom_1.control.init(state);
	eeprom_2.control.init(state);
    status = DEVICE_INIT;
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

static uint32_t eep_page_read(uint32_t page, uint8_t * buffer)
{
	if(page < eeprom_1.info.pageamount())
	{
		return(eeprom_1.page.read(page, buffer));
	}
	else
	{
		return(eeprom_2.page.read((page - eeprom_1.info.pageamount()), buffer));
	}
}

static uint32_t eep_page_write(uint32_t page, const uint8_t *buffer)
{
	if(page < eeprom_1.info.pageamount())
	{
		return(eeprom_1.page.write(page, buffer));
	}
	else
	{
		return(eeprom_2.page.write((page - eeprom_1.info.pageamount()), buffer));
	}
}


static uint32_t eep_random_read(uint32_t addr, uint32_t count, uint8_t * buffer)
{
    uint32_t ret;
    
    if((addr < eeprom_1.info.chipsize()) && ((addr + count) >= eeprom_1.info.chipsize()))
    {
        //分芯片
        ret = eeprom_1.random.read(addr, (eeprom_1.info.chipsize() - addr), buffer);
        ret += eeprom_2.random.read(0, (count - (eeprom_1.info.chipsize() - addr)), (buffer + (eeprom_1.info.chipsize() - addr)));
        
        return(ret);
    }
    else if(addr < eeprom_1.info.chipsize())
    {
        //第一片
        return(eeprom_1.random.read(addr, count, buffer));
    }
    else
    {
        //第二片
        return(eeprom_2.random.read((addr - eeprom_1.info.chipsize()), count, buffer));
    }
}

static uint32_t eep_random_write(uint32_t addr, uint32_t count, const uint8_t *buffer)
{
    uint32_t ret;
    
    if((addr < eeprom_1.info.chipsize()) && ((addr + count) >= eeprom_1.info.chipsize()))
    {
        //分芯片
        ret = eeprom_1.random.write(addr, (eeprom_1.info.chipsize() - addr), buffer);
        ret += eeprom_2.random.write(0, (count - (eeprom_1.info.chipsize() - addr)), (buffer + (eeprom_1.info.chipsize() - addr)));
        
        return(ret);
    }
    else if(addr < eeprom_1.info.chipsize())
    {
        //第一片
        return(eeprom_1.random.write(addr, count, buffer));
    }
    else
    {
        //第二片
        return(eeprom_2.random.write((addr - eeprom_1.info.chipsize()), count, buffer));
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
    
    .random          = 
    {
        .read       = eep_random_read,
        .write      = eep_random_write,
    },
    
    .info           = 
    {
        .pagesize   = eep_pagesize,
        .pageamount = eep_pageamount,
        .chipsize   = eep_chipsize,
    },
    
    .erase          = eep_erase,
};
