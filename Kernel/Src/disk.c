/**
 * @brief		
 * @details		
 * @date		2018-08-30
 **/

/* Includes ------------------------------------------------------------------*/
#include "kernel.h"
#include "allocator.h"
#include "allocator_ctrl.h"
#include "tasks.h"
#include "string.h"

#include "cpu.h"
#include "eeprom.h"
#include "flash.h"

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief  
  */
struct __file_entry
{
    char						*name;
    uint32_t					size;
    enum __disk_aria			aria;
};

/* Private define ------------------------------------------------------------*/
#define EEP_PAGES_DIV_RATIO ((uint8_t)2)//取值0~8，代表安全区占用的页的比例
#define EEP_PAGES_SAFE(n)   ((uint32_t)((n)/8*EEP_PAGES_DIV_RATIO))
#define EEP_PAGES_FREQ(n)   ((uint32_t)((n)/8*(8-EEP_PAGES_DIV_RATIO)))

/* Private macro -------------------------------------------------------------*/
#define AMOUNT_FILE			((uint16_t)(sizeof(file_entry)/sizeof(struct __file_entry)))

/* Private variables ---------------------------------------------------------*/
/**
  * @brief  系统文件表
  */
static const struct __file_entry file_entry[] = 
{
    /* 文件名  文件大小  文件所在分区 */
	{"Display", 2*1024, DISK_FREQ},
};


static uint8_t lock = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

static void disk_ctrl_start(void)
{
    if(system_status() == SYSTEM_RUN)
    {
        cpu.watchdog.feed();
        eeprom.control.init(DEVICE_NORMAL);
        
        cpu.watchdog.feed();
        flash.control.init(DEVICE_NORMAL);
    }
    else
    {
        cpu.watchdog.feed();
        eeprom.control.init(DEVICE_LOWPOWER);
        
        cpu.watchdog.feed();
        flash.control.init(DEVICE_LOWPOWER);
    }
}

/**
  * @brief  
  */
static void disk_unlock(void)
{
    lock = 0x5a;
}

/**
  * @brief  
  */
static void disk_lock(void)
{
    lock = 0;
}

/**
  * @brief  
  */
static void disk_ctrl_idle(void)
{
    cpu.watchdog.feed();
    eeprom.control.suspend();
    
    cpu.watchdog.feed();
	flash.control.suspend();
}

/**
  * @brief  
  */
static void disk_ctrl_format(void)
{
    cpu.watchdog.feed();
    eeprom.erase();
    
    cpu.watchdog.feed();
	flash.erase();
}

/**
  * @brief  
  */
const struct __disk_ctrl disk_ctrl = 
{
    .start              = disk_ctrl_start,
    .lock               = disk_lock,
    .unlock             = disk_unlock,
    .idle               = disk_ctrl_idle,
    .format             = disk_ctrl_format,
};




/**
  * @brief  
  */
static uint32_t disk_copy(const char *dst, const char *src, uint32_t count)
{
    
}

/**
  * @brief  
  */
static uint32_t disk_read(const char *name, uint32_t offset, uint32_t count, void *buff)
{
	uint16_t loop;
	uint16_t index = 0xffff;
	uint32_t address = 0;
	
	if(!name)
	{
		return(0);
	}
	
	for(loop=0; loop<AMOUNT_FILE; loop++)
	{
		if(strcmp(file_entry[loop].name, name) == 0)
		{
			index = loop;
			break;
		}
	}
	
	if(index == 0xffff)
	{
		return(0);
	}
	
	for(loop=0; loop<index; loop++)
	{
		if(file_entry[loop].aria == file_entry[index].aria)
		{
			address += file_entry[loop].size;
		}
	}
	
	return(eeprom.random.read(address + offset, count, buff));
}

/**
  * @brief  
  */
static uint32_t disk_write(const char *name, uint32_t offset, uint32_t count, const void *buff)
{
	uint16_t loop;
	uint16_t index = 0xffff;
	uint32_t address = 0;
	
	if(!name)
	{
		return(0);
	}
    
    if(lock != 0x5a)
    {
        return(0);
    }
	
	for(loop=0; loop<AMOUNT_FILE; loop++)
	{
		if(strcmp(file_entry[loop].name, name) == 0)
		{
			index = loop;
			break;
		}
	}
	
	if(index == 0xffff)
	{
		return(0);
	}
	
	for(loop=0; loop<index; loop++)
	{
		if(file_entry[loop].aria == file_entry[index].aria)
		{
			address += file_entry[loop].size;
		}
	}
	
	return(eeprom.random.write(address + offset, count, buff));
}

/**
  * @brief  
  */
static uint32_t disk_set(const char *name, uint8_t ch, uint32_t count)
{
    
}


/**
  * @brief  
  */
struct __disk disk = 
{
	.read				= disk_read,
	.write				= disk_write,
	.copy				= disk_copy,
	.set				= disk_set,
};
