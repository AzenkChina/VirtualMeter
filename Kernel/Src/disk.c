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
#include "trace.h"

#include "cpu.h"
#include "eeprom.h"
#include "flash.h"

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief  �ļ���д����
  */
enum __attr
{
    FILE_EEPROM = 0x0f,//EEPROM(���ֽڲ�д�洢����)
    FILE_FLASH = 0xf0,//FLASH(���������)
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
/* Private macro -------------------------------------------------------------*/
#define AMOUNT_FILE			    ((uint16_t)(sizeof(file_entry)/sizeof(struct __file_entry)))

/* Private variables ---------------------------------------------------------*/
/**
  * @brief  ϵͳ�ļ���
  */
static const struct __file_entry file_entry[] = 
{
    /* ע��������� */
    /* �ļ���              �ļ���С        �ļ����ڷ��� */
    {"calibration",	        256,            FILE_EEPROM}, //���У׼��Ϣ
    {"information",	        512,            FILE_EEPROM}, //��������Ϣ
    {"measurements",        8*1024,         FILE_EEPROM}, //����������
    {"dlms",                2*1024,         FILE_EEPROM}, //DLMSЭ�����
    {"lexicon",             64*1024,        FILE_FLASH}, //���������ʵ�
    {"disconnect",          512,            FILE_EEPROM}, //�̵�������
    {"display",             4*1024,         FILE_EEPROM}, //��ʾ����
    {"firmware",            512*1024,       FILE_FLASH}, //�̼�����
};


static uint8_t lock = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void disk_ctrl_start(void)
{
	uint16_t loop;
    uint32_t eeprom_size = 0;
	uint32_t flash_size = 0;
	
	if((system_status() != SYSTEM_RUN) && (system_status() != SYSTEM_WAKEUP))
	{
		return;
	}
    
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
    
    for(loop=0; loop<AMOUNT_FILE; loop++)
    {
        if(file_entry[loop].attr == FILE_EEPROM)
        {
            eeprom_size += file_entry[loop].size;
        }
        else if(file_entry[loop].attr == FILE_FLASH)
        {
            flash_size += file_entry[loop].size;
        }
    }
    
    ASSERT(eeprom_size > eeprom.info.chipsize());
    ASSERT(flash_size > flash.info.chipsize());
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
	flash.control.suspend();
    
    cpu.watchdog.feed();
    eeprom.control.suspend();
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
    
    uint32_t pagesize = 0;
    uint32_t page = 0;
    uint32_t header = 0;
    uint32_t middle = 0;
    uint32_t tail = 0;
    uint8_t *buffer = (uint8_t *)buff;
	
	if(!name || !count || !buff)
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
	
    if((offset >= file_entry[index].size) || (count > file_entry[index].size))
    {
        return(0);
    }
	
    if(((offset + count) > file_entry[index].size))
    {
		count = file_entry[index].size - offset;
    }
    
    if(file_entry[index].attr == FILE_EEPROM)
    {
        pagesize = eeprom.info.pagesize();
        
        if(!pagesize)
        {
            return(0);
        }
        
        for(loop=0; loop<index; loop++)
        {
            if(file_entry[loop].attr == FILE_EEPROM)
            {
                address += file_entry[loop].size;
            }
        }
        
        address += offset;//������ʼ��ַ
        page = address / pagesize;//������ʼҳ
        header = address % pagesize;//������ʼҳ����ʼ��ַ
        
        if(page == ((address + count - 1) / pagesize))
        {
            //����ҳ
            if(eeprom.page.read(page, header, count, buffer) != count)
            {
                return(0);
            }
            
            return(count);
        }
        else
        {
            //��ҳ
            middle = (count - ((pagesize - header) % pagesize)) / pagesize;//������ҳҳ��
            tail = (address + count) % pagesize;//�������һҳд���ֽ���
            
            if(header)
            {
                if(eeprom.page.read(page, header, (pagesize - header), buffer) != (pagesize - header))
                {
                    return(0);
                }
                
                buffer += (pagesize - header);
                page += 1;
            }
            
            if(middle)
            {
                for(loop=0; loop<middle; loop++)
                {
                    if(eeprom.page.read(page, 0, pagesize, buffer) != pagesize)
                    {
                        return(0);
                    }
                    
                    buffer += pagesize;
                    page += 1;
                }
            }
            
            if(tail)
            {
                if(eeprom.page.read(page, 0, tail, buffer) != tail)
                {
                    return(0);
                }
            }
            
            return(count);
        }
    }
    else if(file_entry[index].attr == FILE_FLASH)
    {
        pagesize = flash.info.blocksize();
        
        if(!pagesize)
        {
            return(0);
        }
        
        for(loop=0; loop<index; loop++)
        {
            if(file_entry[loop].attr == FILE_FLASH)
            {
                if(file_entry[loop].size % pagesize)
                {
                    address += ((file_entry[loop].size / pagesize) + 1) * pagesize;
                }
                else
                {
                    address += file_entry[loop].size;
                }
            }
        }
        
        address += offset;//������ʼ��ַ
        page = address / pagesize;//������ʼҳ
        header = address % pagesize;//������ʼҳ����ʼ��ַ
        
        if(page == ((address + count - 1) / pagesize))
        {
            //����ҳ
            if(flash.block.read(page, header, count, buffer) != count)
            {
                return(0);
            }
            
            return(count);
        }
        else
        {
            //��ҳ
            middle = (count - ((pagesize - header) % pagesize)) / pagesize;//������ҳҳ��
            tail = (address + count) % pagesize;//�������һҳд���ֽ���
            
            if(header)
            {
                if(flash.block.read(page, header, (pagesize - header), buffer) != (pagesize - header))
                {
                    return(0);
                }
                
                buffer += (pagesize - header);
                page += 1;
            }
            
            if(middle)
            {
                for(loop=0; loop<middle; loop++)
                {
                    if(flash.block.read(page, 0, pagesize, buffer) != pagesize)
                    {
                        return(0);
                    }
                    
                    buffer += pagesize;
                    page += 1;
                }
            }
            
            if(tail)
            {
                if(flash.block.read(page, 0, tail, buffer) != tail)
                {
                    return(0);
                }
            }
            
            return(count);
        }
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
    
    uint32_t pagesize = 0;
    uint32_t page = 0;
    uint32_t header = 0;
    uint32_t middle = 0;
    uint32_t tail = 0;
    uint8_t *buffer = (uint8_t *)buff;
	
	if(!name || !count || !buff)
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
    
    if((offset >= file_entry[index].size) || (count > file_entry[index].size))
    {
        return(0);
    }
	
    if(((offset + count) > file_entry[index].size))
    {
		count = file_entry[index].size - offset;
    }
	
    if(file_entry[index].attr == FILE_EEPROM)
    {
        pagesize = eeprom.info.pagesize();
        
        if(!pagesize)
        {
            return(0);
        }
        
        for(loop=0; loop<index; loop++)
        {
            if(file_entry[loop].attr == FILE_EEPROM)
            {
                address += file_entry[loop].size;
            }
        }
        
        address += offset;//������ʼ��ַ
        page = address / pagesize;//������ʼҳ
        header = address % pagesize;//������ʼҳ����ʼ��ַ
        
        if(page == ((address + count - 1) / pagesize))
        {
            //����ҳ
            if(eeprom.page.write(page, header, count, buffer) != count)
            {
                return(0);
            }
            
            return(count);
        }
        else
        {
            //��ҳ
            middle = (count - ((pagesize - header) % pagesize)) / pagesize;//������ҳҳ��
            tail = (address + count) % pagesize;//�������һҳд���ֽ���
            
            if(header)
            {
                if(eeprom.page.write(page, header, (pagesize - header), buffer) != (pagesize - header))
                {
                    return(0);
                }
                
                buffer += (pagesize - header);
                page += 1;
            }
            
            if(middle)
            {
                for(loop=0; loop<middle; loop++)
                {
                    if(eeprom.page.write(page, 0, pagesize, buffer) != pagesize)
                    {
                        return(0);
                    }
                    
                    buffer += pagesize;
                    page += 1;
                }
            }
            
            if(tail)
            {
                if(eeprom.page.write(page, 0, tail, buffer) != tail)
                {
                    return(0);
                }
            }
            
            return(count);
        }
    }
    else if(file_entry[index].attr == FILE_FLASH)
    {
        pagesize = flash.info.blocksize();
        
        if(!pagesize)
        {
            return(0);
        }
        
        for(loop=0; loop<index; loop++)
        {
            if(file_entry[loop].attr == FILE_FLASH)
            {
                if(file_entry[loop].size % pagesize)
                {
                    address += ((file_entry[loop].size / pagesize) + 1) * pagesize;
                }
                else
                {
                    address += file_entry[loop].size;
                }
            }
        }
        
        address += offset;//������ʼ��ַ
        page = address / pagesize;//������ʼҳ
        header = address % pagesize;//������ʼҳ����ʼ��ַ
        
        if(page == ((address + count - 1) / pagesize))
        {
            //����ҳ
            if(!header)
            {
                if(flash.block.erase(page) != pagesize)
                {
                    return(0);
                }
            }
            if(flash.block.write(page, header, count, buffer) != count)
            {
                return(0);
            }
            
            return(count);
        }
        else
        {
            //��ҳ
            middle = (count - ((pagesize - header) % pagesize)) / pagesize;//������ҳҳ��
            tail = (address + count) % pagesize;//�������һҳд���ֽ���
            
            if(header)
            {
                if(flash.block.write(page, header, (pagesize - header), buffer) != (pagesize - header))
                {
                    return(0);
                }
                
                buffer += (pagesize - header);
                page += 1;
            }
            
            if(middle)
            {
                for(loop=0; loop<middle; loop++)
                {
                    if(flash.block.erase(page) != pagesize)
                    {
                        return(0);
                    }
                    if(flash.block.write(page, 0, pagesize, buffer) != pagesize)
                    {
                        return(0);
                    }
                    
                    buffer += pagesize;
                    page += 1;
                }
            }
            
            if(tail)
            {
                if(flash.block.erase(page) != pagesize)
                {
                    return(0);
                }
                if(flash.block.write(page, 0, tail, buffer) != tail)
                {
                    return(0);
                }
            }
            
            return(count);
        }
    }
    else
    {
        return(0);
    }
}

/**
  * @brief  
  */
static uint32_t disk_size(const char *name)
{
	uint16_t loop;
	
	if(!name)
	{
		return(0);
	}
	
	for(loop=0; loop<AMOUNT_FILE; loop++)
	{
		if(strcmp(file_entry[loop].name, name) == 0)
		{
			return(file_entry[loop].size);
		}
	}
    
    return(0);
}

/**
  * @brief  
  */
static uint32_t disk_cluster(const char *name)
{
	uint16_t loop;
	
	if(!name)
	{
		return(0);
	}
	
	for(loop=0; loop<AMOUNT_FILE; loop++)
	{
		if(strcmp(file_entry[loop].name, name) == 0)
		{
            if(file_entry[loop].attr == FILE_EEPROM)
            {
                return(eeprom.info.pagesize());
            }
            else if(file_entry[loop].attr == FILE_FLASH)
            {
                return(flash.info.blocksize());
            }
            
            break;
		}
	}
    
    return(0);
}

/**
  * @brief  
  */
struct __file file = 
{
	.read				= disk_read,
	.write				= disk_write,
    .size               = disk_size,
    .cluster            = disk_cluster,
};
