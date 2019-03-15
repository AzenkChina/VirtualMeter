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
    uint32_t					start;
    uint32_t					end;
    uint32_t					scatter_start;
    uint32_t					scatter_end;
    enum __disk_aria			aria;
};

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define AMOUNT_FILE			((uint16_t)(sizeof(files) / sizeof(struct __file_entry)))

/* Private variables ---------------------------------------------------------*/
static uint8_t lock = 0;

static const struct __file_entry files[] = 
{
	{"Display", 1, 32, 0xffffffff, 0xffffffff, DISK_FREQ},
};

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

static void disk_ctrl_idle(void)
{
    cpu.watchdog.feed();
    eeprom.control.suspend();
    
    cpu.watchdog.feed();
	flash.control.suspend();
}

static void disk_ctrl_format(void)
{
    cpu.watchdog.feed();
    eeprom.erase();
    
    cpu.watchdog.feed();
	flash.erase();
}


const struct __disk_ctrl disk_ctrl = 
{
    .start              = disk_ctrl_start,
    .lock               = disk_lock,
    .unlock             = disk_unlock,
    .idle               = disk_ctrl_idle,
    .format             = disk_ctrl_format,
};





static uint32_t disk_alloc(enum __disk_aria aria, const char *name, uint32_t size)
{
    
}

static void disk_free(const char *name)
{
    
}

static uint32_t disk_copy(const char *dst, const char *src, uint32_t count)
{
    
}

static uint32_t disk_seek(const char *name)
{
   uint16_t  loop;
   
   for(loop=0; loop< AMOUNT_FILE; loop++)
   {
		if(strcmp(files[loop].name, name) == 0)
		{
			return(files[loop].start);
		}
   }
   
   return(0);
}

static uint32_t disk_read(const char *name, uint32_t offset, uint32_t count, void *buff)
{
   uint16_t  loop;
   uint32_t  start = 0;
   uint32_t  stop = 0;
   
   for(loop=0; loop< AMOUNT_FILE;loop++)
   {
		if(strcmp(files[loop].name, name) == 0)
		{
			start = files[loop].start;
			stop = files[loop].end;
		}
   }
   
   if(!start)
   {
   		return(0);
   }
   
   start = start * eeprom.info.pagesize();
   start += offset;
   
   eeprom.random.read(start, count, buff);
   
   return(0);
}

static uint32_t disk_write(const char *name, uint32_t offset, uint32_t count, const void *buff)
{
   uint16_t  loop;
   uint32_t  start = 0;
   uint32_t  stop = 0;
   
   for(loop=0; loop< AMOUNT_FILE;loop++)
   {
		if(strcmp(files[loop].name, name) == 0)
		{
			start = files[loop].start;
			stop = files[loop].end;
		}
   }
   
   if(!start)
   {
   		return(0);
   }
   
   start = start * eeprom.info.pagesize();
   start += offset;
   
   eeprom.random.write(start, count, buff);
   
   return(0);
}

static uint32_t disk_set(const char *name, uint8_t ch, uint32_t count)
{
    
}



struct __disk disk = 
{
	.alloc				= disk_alloc,
	.free				= disk_free,
	.seek				= disk_seek,
	.read				= disk_read,
	.write				= disk_write,
	.copy				= disk_copy,
	.set				= disk_set,
};
