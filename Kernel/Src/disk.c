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
  * @brief  �ļ���д����
  */
enum __attr
{
    FILE_SAFE = 0x03,//�����౸�ݵİ�ȫ����(eeprom�ȿɵ��ֽڲ�д�洢����)
    FILE_FREQ = 0x06,//��Ƶ���޸�����(eeprom�ȿɵ��ֽڲ�д�洢����)
    FILE_RARE = 0x0c,//��Ƶ���޸�����(flash�ȿ�������� ռ��˫���洢�ռ䲢��д��ʱ����ʱ��ϳ�)
    FILE_LOOP = 0x18,//�ļ�ѭ��д��(flash�ȿ��������)
    FILE_ONCE = 0x30,//�ļ�һ����д��(flash�ȿ��������)
};

/**
  * @brief  
  */
struct __file_entry
{
    char						*name;
    uint32_t					size;
    enum __attr                 attr;
};

/* Private define ------------------------------------------------------------*/
#define EEP_PAGES_DIV_RATIO     ((uint8_t)2)//ȡֵ0~8���������౸����ռ�õ�ҳ�ı���
#define EEP_PAGES_SAFE(n)       ((uint32_t)((n)*EEP_PAGES_DIV_RATIO/8))//���౸����ռ��ҳ��
#define EEP_PAGES_FREQ(n)       ((uint32_t)((n)*(8-EEP_PAGES_DIV_RATIO)/8))//Ƶ���޸���ռ��ҳ��

/* Private macro -------------------------------------------------------------*/
#define AMOUNT_FILE			    ((uint16_t)(sizeof(file_entry)/sizeof(struct __file_entry)))

/* Private variables ---------------------------------------------------------*/
/**
  * @brief  ϵͳ�ļ���
  */
static const struct __file_entry file_entry[] = 
{
    /* �ļ���  �ļ���С  �ļ����ڷ��� */
	{"display", 4*1024, FILE_FREQ},
    {"firmware", 384*1024, FILE_ONCE},
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
static void disk_ctrl_unlock(void)
{
    lock = 0x5a;
}

/**
  * @brief  
  */
static void disk_ctrl_lock(void)
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
    .lock               = disk_ctrl_lock,
    .unlock             = disk_ctrl_unlock,
    .idle               = disk_ctrl_idle,
    .format             = disk_ctrl_format,
};


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
    
    if(file_entry[index].attr == FILE_SAFE)
    {
        //...
        return(0);
    }
    else if(file_entry[index].attr == FILE_FREQ)
    {
        //...
        return(0);
    }
    else if(file_entry[index].attr == FILE_RARE)
    {
        //...
        return(0);
    }
    else if(file_entry[index].attr == FILE_LOOP)
    {
        //...
        return(0);
    }
    else if(file_entry[index].attr == FILE_ONCE)
    {
        //...
        return(0);
    }
    else
    {
        return(0);
    }
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
	
    if(file_entry[index].attr == FILE_SAFE)
    {
        //...
        return(0);
    }
    else if(file_entry[index].attr == FILE_FREQ)
    {
        //...
        return(0);
    }
    else if(file_entry[index].attr == FILE_RARE)
    {
        //...
        return(0);
    }
    else if(file_entry[index].attr == FILE_LOOP)
    {
        //...
        return(0);
    }
    else if(file_entry[index].attr == FILE_ONCE)
    {
        //...
        return(0);
    }
    else
    {
        return(0);
    }
}


/**
  * @brief  
  */
struct __file file = 
{
	.read				= disk_read,
	.write				= disk_write,
};
