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
#include "lfs.h"
#include "crc.h"
#include "ecc.h"

#include "cpu.h"
#include "flash.h"

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief  ��������
  */
enum __container_type
{
    CT_NORMAL = 0x03,//�������
	CT_SECURE = 0x06,//��ȫ����
    CT_RING = 0x0c,//ѭ������
    CT_PARALLEL = 0x18,//��������
};

/**
  * @brief  ���ζ���ͷ
  */
struct __ring_queue_header
{
	uint16_t length; //ÿ����¼����󳤶�
	uint16_t capacity; //�����Ŀ����
	uint16_t amount; //��ǰ��Ŀ����
	uint16_t current; //��ǰ��Ŀ
	uint32_t check;
};

/**
  * @brief  ��������ͷ
  */
struct __parallel_buffer_header
{
	uint16_t length; //ÿ����¼����󳤶�
	uint16_t capacity; //�����Ŀ����
	uint32_t signature; //�ļ�ǩ��
	uint32_t check;
};

/**
  * @brief  �ļ�����
  */
struct __file_entry
{
    char						*name;
    uint32_t					size;
    enum __container_type		attr;
};

/* Private define ------------------------------------------------------------*/
static int lfs_low_read(const struct lfs_config *c, lfs_block_t block,
		lfs_off_t off, void *buffer, lfs_size_t size);
static int lfs_low_prog(const struct lfs_config *c, lfs_block_t block,
		lfs_off_t off, const void *buffer, lfs_size_t size);
static int lfs_low_erase(const struct lfs_config *c, lfs_block_t block);
static int lfs_low_sync(const struct lfs_config *c);

/* Private macro -------------------------------------------------------------*/
#define AMOUNT_FILE			    ((uint16_t)(sizeof(file_entry)/sizeof(struct __file_entry)))

/* Private variables ---------------------------------------------------------*/
/**
  * @brief  ϵͳ�ļ���
  */
static const struct __file_entry file_entry[] = 
{
    /* ע��������� */
    /* �ļ���              �ļ���С        �������� */
    {"calibration",	        512,            CT_SECURE},		//���У׼��Ϣ
    {"information",	        512,            CT_NORMAL},		//��������Ϣ
    {"measurements",        8*1024,         CT_SECURE},		//����������
    {"dlms",                2*1024,         CT_SECURE},		//DLMSЭ�����
    {"lexicon",             64*1024,        CT_SECURE},		//���������ʵ�
    {"disconnect",          512,            CT_SECURE},		//�̵�������
    {"display",             4*1024,         CT_SECURE},		//��ʾ����
    {"firmware",            512*1024,       CT_PARALLEL},	//�̼�����
};

static uint8_t lock = 0;

static const struct lfs_config lfs_cfg = 
{
	.read			= lfs_low_read,
	.prog			= lfs_low_prog,
	.erase			= lfs_low_erase,
	.sync			= lfs_low_sync,
	
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    .block_size = 4096,
    .block_count = 1024,
#else
#if defined (BUILD_REAL_WORLD)
    .block_size = 512,
    .block_count = 8192,
#endif
#endif
    .read_size		= 16,
    .prog_size		= 16,
    .cache_size		= 128,
    .lookahead_size	= 16,
    .block_cycles	= 300,
};
static lfs_t lfs_lfs;
static int lfs_err = -1;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

static int lfs_low_read(const struct lfs_config *c, lfs_block_t block,
		lfs_off_t off, void *buffer, lfs_size_t size)
{
	cpu.watchdog.feed();
	
	if(flash.block.read(block, off, size, buffer) != size)
	{
		return(LFS_ERR_IO);
	}
	
	return(LFS_ERR_OK);
}

static int lfs_low_prog(const struct lfs_config *c, lfs_block_t block,
		lfs_off_t off, const void *buffer, lfs_size_t size)
{
	cpu.watchdog.feed();
	
	if(flash.block.write(block, off, size, buffer) != size)
	{
		return(LFS_ERR_IO);
	}
	
	return(LFS_ERR_OK);
}

static int lfs_low_erase(const struct lfs_config *c, lfs_block_t block)
{
	cpu.watchdog.feed();
	
	if(flash.block.erase(block) != c->block_size)
	{
		return(LFS_ERR_IO);
	}
	
	return(LFS_ERR_OK);
}

static int lfs_low_sync(const struct lfs_config *c)
{
	return(LFS_ERR_OK);
}



/**
  * @brief  
  */
static void disk_ctrl_start(void)
{
	uint16_t loop;
	uint32_t flash_size = 0;
	
	if((system_status() != SYSTEM_RUN) && (system_status() != SYSTEM_WAKEUP))
	{
		return;
	}
    
    if(system_status() == SYSTEM_RUN)
    {
        cpu.watchdog.feed();
        flash.control.init(DEVICE_NORMAL);
    }
    else
    {
        cpu.watchdog.feed();
        flash.control.init(DEVICE_LOWPOWER);
    }
    
#if 1
	
    for(loop=0; loop<AMOUNT_FILE; loop++)
    {
        if(file_entry[loop].attr == CT_NORMAL)
        {
			//CT_NORMAL �����ļ����ռ�ռ��
            flash_size += file_entry[loop].size;
        }
        else if(file_entry[loop].attr == CT_SECURE)
        {
			//CT_SECURE �����ļ������ռ�+ECC��ռ��
            flash_size += (file_entry[loop].size * 3);
        }
		else if(file_entry[loop].attr == CT_RING)
        {
			//CT_RING �����ļ����ռ�+�ļ���Ϣ�ṹ��ռ��
			flash_size += (sizeof(struct __ring_queue_header) * 2);
            flash_size += file_entry[loop].size;
        }
		else if(file_entry[loop].attr == CT_PARALLEL)
        {
			//CT_PARALLEL �����ļ����ռ�+�ļ���Ϣ�ṹ��ռ��
			flash_size += (sizeof(struct __parallel_buffer_header) * 2);
            flash_size += file_entry[loop].size;
        }
    }
	
#endif
	
    ASSERT((flash_size * 7) > (flash.info.chipsize() * 8));
	ASSERT(lfs_cfg.block_size > flash.info.blocksize());
	ASSERT(lfs_cfg.block_count > flash.info.blockcount());
	
	if(lfs_err)
	{
		lfs_err = lfs_mount(&lfs_lfs, &lfs_cfg);
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
	if(!lfs_err)
	{
		cpu.watchdog.feed();
		lfs_unmount(&lfs_lfs);
		lfs_err = -1;
	}
	cpu.watchdog.feed();
	flash.control.suspend();
}

/**
  * @brief  
  */
static void disk_ctrl_format(void)
{
	if(!lfs_err)
	{
		cpu.watchdog.feed();
		lfs_unmount(&lfs_lfs);
		lfs_err = -1;
	}
	
	cpu.watchdog.feed();
	flash.erase();
	cpu.watchdog.feed();
	
	lfs_err = lfs_format(&lfs_lfs, &lfs_cfg);
	
	ASSERT(lfs_err != 0);
	
	if(!lfs_err)
	{
		cpu.watchdog.feed();
		lfs_err = lfs_mount(&lfs_lfs, &lfs_cfg);
	}
}



/**
  * @brief  ϵͳ�����߼�
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
  * @brief  ����
  */
static uint32_t disk_parameter_read(const char *name, uint32_t offset, uint32_t size, void *buff)
{
	uint16_t loop;
	lfs_file_t lfs_file;
	lfs_ssize_t readsize;
	int err;
	
	if(!name || !size || !buff || lfs_err)
	{
		return(0);
	}
	
	//��ѯ�ļ������Ƿ���Ч
    for(loop=0; loop<AMOUNT_FILE; loop++)
    {
        if((file_entry[loop].attr == CT_NORMAL) || (file_entry[loop].attr == CT_SECURE))
        {
			if(strcmp(file_entry[loop].name, name) == 0)
			{
				break;
			}
        }
    }
	
	if(loop >= AMOUNT_FILE)
	{
		return(0);
	}
	
    if((offset >= file_entry[loop].size) || (size > file_entry[loop].size))
    {
        return(0);
    }
	
    if(((offset + size) > file_entry[loop].size))
    {
		size = file_entry[loop].size - offset;
    }
	
	if(file_entry[loop].attr == CT_NORMAL)
	{
		err = lfs_file_open(&lfs_lfs, &lfs_file, file_entry[loop].name, LFS_O_RDONLY);
		if(err)
		{
			return(0);
		}
		
		if(lfs_file_size(&lfs_lfs, &lfs_file) < (offset + size))
		{
			lfs_file_close(&lfs_lfs, &lfs_file);
			return(0);
		}
		
		if(lfs_file_seek(&lfs_lfs, &lfs_file, offset, LFS_SEEK_SET) < 0)
		{
			lfs_file_close(&lfs_lfs, &lfs_file);
			return(0);
		}
		
		readsize = lfs_file_read(&lfs_lfs, &lfs_file, buff, size);
		
		lfs_file_close(&lfs_lfs, &lfs_file);
		
		if(readsize <= 0)
		{
			return(0);
		}
		
		return(readsize);
	}
	else
	{
		//TODO: CT_SECURE ��Ҫ��ȡ�����ļ����бȶ�
		err = lfs_file_open(&lfs_lfs, &lfs_file, file_entry[loop].name, LFS_O_RDONLY);
		if(err)
		{
			return(0);
		}
		
		if(lfs_file_size(&lfs_lfs, &lfs_file) < (offset + size))
		{
			lfs_file_close(&lfs_lfs, &lfs_file);
			return(0);
		}
		
		if(lfs_file_seek(&lfs_lfs, &lfs_file, offset, LFS_SEEK_SET) < 0)
		{
			lfs_file_close(&lfs_lfs, &lfs_file);
			return(0);
		}
		
		readsize = lfs_file_read(&lfs_lfs, &lfs_file, buff, size);
		
		lfs_file_close(&lfs_lfs, &lfs_file);
		
		if(readsize <= 0)
		{
			return(0);
		}
		
		return(readsize);
	}
}

/**
  * @brief  ����
  */
static uint32_t disk_parameter_write(const char *name, uint32_t offset, uint32_t size, const void *buff)
{
	uint16_t loop;
	lfs_file_t lfs_file;
	lfs_ssize_t writesize;
	int err;
	
	if(!name || !size || !buff || lfs_err)
	{
		return(0);
	}
    
    if(lock != 0x5a)
    {
        return(0);
    }
	
	//��ѯ�ļ������Ƿ���Ч
    for(loop=0; loop<AMOUNT_FILE; loop++)
    {
        if((file_entry[loop].attr == CT_NORMAL) || (file_entry[loop].attr == CT_SECURE))
        {
			if(strcmp(file_entry[loop].name, name) == 0)
			{
				break;
			}
        }
    }
	
	if(loop >= AMOUNT_FILE)
	{
		return(0);
	}
    
    if((offset >= file_entry[loop].size) || (size > file_entry[loop].size))
    {
        return(0);
    }
	
    if(((offset + size) > file_entry[loop].size))
    {
		return(0);
    }
	
	if(file_entry[loop].attr == CT_NORMAL)
	{
		err = lfs_file_open(&lfs_lfs, &lfs_file, file_entry[loop].name, LFS_O_RDWR | LFS_O_CREAT);
		if(err)
		{
			return(0);
		}
		
		if(lfs_file_seek(&lfs_lfs, &lfs_file, offset, LFS_SEEK_SET) < 0)
		{
			lfs_file_close(&lfs_lfs, &lfs_file);
			return(0);
		}
		
		writesize = lfs_file_write(&lfs_lfs, &lfs_file, buff, size);
		
		if(writesize <= 0)
		{
			lfs_file_close(&lfs_lfs, &lfs_file);
			return(0);
		}
		
		err = lfs_file_close(&lfs_lfs, &lfs_file);
		
		if(err)
		{
			return(0);
		}
		
		return(writesize);
	}
	else
	{
		//TODO: CT_SECURE ��Ҫͬʱ���±����ļ�
		err = lfs_file_open(&lfs_lfs, &lfs_file, file_entry[loop].name, LFS_O_RDWR | LFS_O_CREAT);
		if(err)
		{
			return(0);
		}
		
		if(lfs_file_seek(&lfs_lfs, &lfs_file, offset, LFS_SEEK_SET) < 0)
		{
			lfs_file_close(&lfs_lfs, &lfs_file);
			return(0);
		}
		
		writesize = lfs_file_write(&lfs_lfs, &lfs_file, buff, size);
		
		if(writesize <= 0)
		{
			lfs_file_close(&lfs_lfs, &lfs_file);
			return(0);
		}
		
		err = lfs_file_close(&lfs_lfs, &lfs_file);
		
		if(err)
		{
			return(0);
		}
		
		return(writesize);
	}
}

/**
  * @brief  ����
  */
static uint32_t disk_parameter_size(const char *name)
{
	uint16_t loop;
	
	if(!name)
	{
		return(0);
	}
	
    for(loop=0; loop<AMOUNT_FILE; loop++)
    {
        if((file_entry[loop].attr == CT_NORMAL) || (file_entry[loop].attr == CT_SECURE))
        {
			if(strcmp(file_entry[loop].name, name) == 0)
			{
				return(file_entry[loop].size);
			}
        }
    }
    
    return(0);
}



/**
  * @brief  ���ζ���
  */
static uint32_t disk_ring_read(const char *name, uint32_t index, uint32_t size, void *buff, bool reverse)
{
	uint16_t loop;
	lfs_file_t lfs_file;
	lfs_ssize_t readsize;
	struct __ring_queue_header *queue_header;
	uint32_t check, calc;
	uint32_t offset;
	char *info;
	int err;
	
	if(!name || !size || !buff || lfs_err)
	{
		return(0);
	}
	
    for(loop=0; loop<AMOUNT_FILE; loop++)
    {
        if(file_entry[loop].attr == CT_RING)
        {
			if(strcmp(file_entry[loop].name, name) == 0)
			{
				break;
			}
        }
    }
	
	if(loop >= AMOUNT_FILE)
	{
		return(0);
	}
	
	info = heap.dzalloc(strlen(file_entry[loop].name) + strlen(".r0") + 1);
	
	if(!info)
	{
		return(0);
	}
	
	strcpy(info, file_entry[loop].name);
	strcat(info, ".r0");
	
	err = lfs_file_open(&lfs_lfs, &lfs_file, info, LFS_O_RDONLY);
	if(err)
	{
		heap.free(info);
		return(0);
	}
	
	heap.free(info);
	
	if(lfs_file_rewind(&lfs_lfs, &lfs_file) < 0)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		return(0);
	}
	
	queue_header = heap.dzalloc(256);
	if(!queue_header)
	{
		return(0);
	}
	
	readsize = lfs_file_read(&lfs_lfs, &lfs_file, (void *)queue_header, sizeof(struct __ring_queue_header));
	
	lfs_file_close(&lfs_lfs, &lfs_file);
	
	if(readsize != sizeof(struct __ring_queue_header))
	{
		heap.free(queue_header);
		return(0);
	}
	
	check = queue_header->check;
	queue_header->check = 0;
	
	__nand_calculate_ecc((void *)queue_header, 256, (uint8_t *)&calc);
	
	if(calc != check)
	{
		if(__nand_correct_data((void *)queue_header, (uint8_t *)&check, (uint8_t *)&calc, 256) < 0)
		{
			heap.free(queue_header);
			return(0);
		}
	}
	
	index = index % queue_header->capacity;
	
	if(index >= queue_header->amount)
	{
		heap.free(queue_header);
		return(0);
	}
	
	if(reverse)
	{
		//�����ϵ�һ����ʼ����
		offset = ((uint32_t)queue_header->capacity + queue_header->current - queue_header->amount + index) % queue_header->capacity;
	}
	else
	{
		//�����µ�һ����ʼ����
		offset = ((uint32_t)queue_header->capacity + queue_header->current - 1 - index)  % queue_header->capacity;
	}
	
	offset = offset * (queue_header->length + 4);
	
	err = lfs_file_open(&lfs_lfs, &lfs_file, file_entry[loop].name, LFS_O_RDONLY);
	if(err)
	{
		heap.free(queue_header);
		return(0);
	}
	
	if(lfs_file_seek(&lfs_lfs, &lfs_file, offset, LFS_SEEK_SET) < 0)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		heap.free(queue_header);
		return(0);
	}
	
	//TODO: ��Ч���ݺ��4�ֽ���ECCУ�飬����֤
	readsize = lfs_file_read(&lfs_lfs, &lfs_file, buff, (size<queue_header->length?size:queue_header->length));
	
	lfs_file_close(&lfs_lfs, &lfs_file);
	heap.free(queue_header);
	return(readsize);
}

/**
  * @brief  ���ζ���
  */
static uint32_t disk_ring_append(const char *name, uint32_t size, const void *buff)
{
	uint16_t loop;
	lfs_file_t lfs_file_info, lfs_file_data;
	lfs_ssize_t operatesize;
	struct __ring_queue_header *queue_header;
	uint32_t check, calc;
	uint32_t offset;
	char *info;
	int err;
	
	if(!name || !size || !buff || lfs_err)
	{
		return(0);
	}
	
    if(lock != 0x5a)
    {
        return(0);
    }
	
    for(loop=0; loop<AMOUNT_FILE; loop++)
    {
        if(file_entry[loop].attr == CT_RING)
        {
			if(strcmp(file_entry[loop].name, name) == 0)
			{
				break;
			}
        }
    }
	
	if(loop >= AMOUNT_FILE)
	{
		return(0);
	}
	
	info = heap.dzalloc(strlen(file_entry[loop].name) + strlen(".r0") + 1);
	
	if(!info)
	{
		return(0);
	}
	
	strcpy(info, file_entry[loop].name);
	strcat(info, ".r0");
	
	err = lfs_file_open(&lfs_lfs, &lfs_file_info, info, LFS_O_RDWR);
	if(err)
	{
		heap.free(info);
		return(0);
	}
	
	heap.free(info);
	
	if(lfs_file_rewind(&lfs_lfs, &lfs_file_info) < 0)
	{
		lfs_file_close(&lfs_lfs, &lfs_file_info);
		return(0);
	}
	
	queue_header = heap.dzalloc(256);
	if(!queue_header)
	{
		lfs_file_close(&lfs_lfs, &lfs_file_info);
		return(0);
	}
	
	operatesize = lfs_file_read(&lfs_lfs, &lfs_file_info, (void *)queue_header, sizeof(struct __ring_queue_header));
	
	if(operatesize != sizeof(struct __ring_queue_header))
	{
		lfs_file_close(&lfs_lfs, &lfs_file_info);
		heap.free(queue_header);
		return(0);
	}
	
	check = queue_header->check;
	queue_header->check = 0;
	
	__nand_calculate_ecc((void *)queue_header, 256, (uint8_t *)&calc);
	
	if(calc != check)
	{
		if(__nand_correct_data((void *)queue_header, (uint8_t *)&check, (uint8_t *)&calc, 256) < 0)
		{
			lfs_file_close(&lfs_lfs, &lfs_file_info);
			heap.free(queue_header);
			return(0);
		}
	}
	
	if(queue_header->current >= queue_header->capacity)
	{
		lfs_file_close(&lfs_lfs, &lfs_file_info);
		heap.free(queue_header);
		return(0);
	}
	
	offset = (uint32_t)queue_header->current * (queue_header->length + 4);
	
	if((offset + (queue_header->length + 4)) > file_entry[loop].size)
	{
		lfs_file_close(&lfs_lfs, &lfs_file_info);
		heap.free(queue_header);
		return(0);
	}
	
	err = lfs_file_open(&lfs_lfs, &lfs_file_data, file_entry[loop].name, LFS_O_RDWR | LFS_O_CREAT);
	if(err)
	{
		lfs_file_close(&lfs_lfs, &lfs_file_info);
		heap.free(queue_header);
		return(0);
	}
	
	if(lfs_file_seek(&lfs_lfs, &lfs_file_data, offset, LFS_SEEK_SET) < 0)
	{
		lfs_file_close(&lfs_lfs, &lfs_file_data);
		lfs_file_close(&lfs_lfs, &lfs_file_info);
		heap.free(queue_header);
		return(0);
	}
	
	//TODO: ��Ч���ݺ��4�ֽ���ECCУ�飬�����
	operatesize = lfs_file_write(&lfs_lfs, &lfs_file_data, buff, (size<queue_header->length?size:queue_header->length));
	lfs_file_close(&lfs_lfs, &lfs_file_data);
	
	if(operatesize != (size<queue_header->length?size:queue_header->length))
	{
		lfs_file_close(&lfs_lfs, &lfs_file_info);
		heap.free(queue_header);
		return(0);
	}
	
	if(lfs_file_rewind(&lfs_lfs, &lfs_file_info) < 0)
	{
		lfs_file_close(&lfs_lfs, &lfs_file_info);
		heap.free(queue_header);
		return(0);
	}
	
	if(queue_header->amount < queue_header->capacity)
	{
		queue_header->amount += 1;
	}
	
	queue_header->current += 1;
	queue_header->current = queue_header->current % queue_header->capacity;
	
	queue_header->check = 0;
	__nand_calculate_ecc((void *)queue_header, 256, (uint8_t *)&calc);
	queue_header->check = calc;
	
	if(lfs_file_size(&lfs_lfs, &lfs_file_info) != sizeof(struct __ring_queue_header))
	{
		lfs_file_truncate(&lfs_lfs, &lfs_file_info, sizeof(struct __ring_queue_header));
	}
	
	if(lfs_file_write(&lfs_lfs, &lfs_file_info, (void *)queue_header, sizeof(struct __ring_queue_header)) != sizeof(struct __ring_queue_header))
	{
		lfs_file_close(&lfs_lfs, &lfs_file_info);
		heap.free(queue_header);
		return(0);
	}
	
	lfs_file_close(&lfs_lfs, &lfs_file_info);
	heap.free(queue_header);
	return(operatesize);
}

/**
  * @brief  ���ζ���
  */
static uint32_t disk_ring_truncate(const char *name, uint32_t amount, bool reverse)
{
	uint16_t loop;
	lfs_file_t lfs_file;
	lfs_ssize_t operatesize;
	struct __ring_queue_header *queue_header;
	uint32_t check, calc;
	char *info;
	int err;
	
	if(!name || !amount || lfs_err)
	{
		return(0);
	}
	
    if(lock != 0x5a)
    {
        return(0);
    }
	
    for(loop=0; loop<AMOUNT_FILE; loop++)
    {
        if(file_entry[loop].attr == CT_RING)
        {
			if(strcmp(file_entry[loop].name, name) == 0)
			{
				break;
			}
        }
    }
	
	if(loop >= AMOUNT_FILE)
	{
		return(0);
	}
	
	info = heap.dzalloc(strlen(file_entry[loop].name) + strlen(".r0") + 1);
	
	if(!info)
	{
		return(0);
	}
	
	strcpy(info, file_entry[loop].name);
	strcat(info, ".r0");
	
	err = lfs_file_open(&lfs_lfs, &lfs_file, info, LFS_O_RDWR);
	if(err)
	{
		heap.free(info);
		return(0);
	}
	
	heap.free(info);
	
	if(lfs_file_rewind(&lfs_lfs, &lfs_file) < 0)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		return(0);
	}
	
	queue_header = heap.dzalloc(256);
	if(!queue_header)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		return(0);
	}
	
	operatesize = lfs_file_read(&lfs_lfs, &lfs_file, (void *)queue_header, sizeof(struct __ring_queue_header));
	
	if(operatesize != sizeof(struct __ring_queue_header))
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		heap.free(queue_header);
		return(0);
	}
	
	check = queue_header->check;
	queue_header->check = 0;
	
	__nand_calculate_ecc((void *)queue_header, 256, (uint8_t *)&calc);
	
	if(calc != check)
	{
		if(__nand_correct_data((void *)queue_header, (uint8_t *)&check, (uint8_t *)&calc, 256) < 0)
		{
			lfs_file_close(&lfs_lfs, &lfs_file);
			heap.free(queue_header);
			return(0);
		}
	}
	
	if(amount >= queue_header->amount)
	{
		//ȫ�����
		queue_header->current = 0;
		queue_header->amount = 0;
	}
	else
	{
		queue_header->amount -= amount;
		
		if(!reverse)
		{
			//�����µ�һ����ʼ����
			queue_header->current = ((uint32_t)queue_header->capacity + queue_header->current - amount) % queue_header->capacity;
		}
	}
	
	operatesize = queue_header->amount;
	
	if(lfs_file_rewind(&lfs_lfs, &lfs_file) < 0)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		heap.free(queue_header);
		return(0);
	}
	
	queue_header->check = 0;
	__nand_calculate_ecc((void *)queue_header, 256, (uint8_t *)&calc);
	queue_header->check = calc;
	
	if(lfs_file_size(&lfs_lfs, &lfs_file) != sizeof(struct __ring_queue_header))
	{
		lfs_file_truncate(&lfs_lfs, &lfs_file, sizeof(struct __ring_queue_header));
	}
	
	if(lfs_file_write(&lfs_lfs, &lfs_file, (void *)queue_header, sizeof(struct __ring_queue_header)) != sizeof(struct __ring_queue_header))
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		heap.free(queue_header);
		return(0);
	}
	
	lfs_file_close(&lfs_lfs, &lfs_file);
	heap.free(queue_header);
	return(operatesize);
}

/**
  * @brief  ���ζ���
  */
static bool disk_ring_info(const char *name, struct __ring_info *ring_info)
{
	uint16_t loop;
	lfs_file_t lfs_file;
	lfs_ssize_t readsize;
	struct __ring_queue_header *queue_header;
	uint32_t check, calc;
	char *info;
	int err;
	
	if(!name || !ring_info || lfs_err)
	{
		return(false);
	}
	
    for(loop=0; loop<AMOUNT_FILE; loop++)
    {
        if(file_entry[loop].attr == CT_RING)
        {
			if(strcmp(file_entry[loop].name, name) == 0)
			{
				break;
			}
        }
    }
	
	if(loop >= AMOUNT_FILE)
	{
		return(false);
	}
	
	info = heap.dzalloc(strlen(file_entry[loop].name) + strlen(".r0") + 1);
	
	if(!info)
	{
		return(false);
	}
	
	strcpy(info, file_entry[loop].name);
	strcat(info, ".r0");
	
	err = lfs_file_open(&lfs_lfs, &lfs_file, info, LFS_O_RDONLY);
	if(err)
	{
		heap.free(info);
		return(false);
	}
	
	heap.free(info);
	
	if(lfs_file_rewind(&lfs_lfs, &lfs_file) < 0)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		return(false);
	}
	
	queue_header = heap.dzalloc(256);
	if(!queue_header)
	{
		return(false);
	}
	
	readsize = lfs_file_read(&lfs_lfs, &lfs_file, (void *)queue_header, sizeof(struct __ring_queue_header));
	
	lfs_file_close(&lfs_lfs, &lfs_file);
	
	if(readsize != sizeof(struct __ring_queue_header))
	{
		heap.free(queue_header);
		return(false);
	}
	
	check = queue_header->check;
	queue_header->check = 0;
	
	__nand_calculate_ecc((void *)queue_header, 256, (uint8_t *)&calc);
	
	if(calc != check)
	{
		if(__nand_correct_data((void *)queue_header, (uint8_t *)&check, (uint8_t *)&calc, 256) < 0)
		{
			heap.free(queue_header);
			return(false);
		}
	}
	
	ring_info->amount = queue_header->amount;
	ring_info->capacity = queue_header->capacity;
	ring_info->length = queue_header->capacity;
	
	heap.free(queue_header);
	return(true);
}

/**
  * @brief  ���ζ���
  */
static bool disk_ring_reset(const char *name)
{
	uint16_t loop;
	lfs_file_t lfs_file;
	struct __ring_queue_header *queue_header;
	uint32_t check, calc;
	char *info;
	int err;
	
	if(!name || lfs_err)
	{
		return(false);
	}
	
    if(lock != 0x5a)
    {
        return(false);
    }
	
    for(loop=0; loop<AMOUNT_FILE; loop++)
    {
        if(file_entry[loop].attr == CT_RING)
        {
			if(strcmp(file_entry[loop].name, name) == 0)
			{
				break;
			}
        }
    }
	
	if(loop >= AMOUNT_FILE)
	{
		return(false);
	}
	
	info = heap.dzalloc(strlen(file_entry[loop].name) + strlen(".r0") + 1);
	
	if(!info)
	{
		return(false);
	}
	
	strcpy(info, file_entry[loop].name);
	strcat(info, ".r0");
	
	err = lfs_file_open(&lfs_lfs, &lfs_file, info, LFS_O_RDWR);
	if(err)
	{
		heap.free(info);
		return(false);
	}
	
	heap.free(info);
	
	if(lfs_file_rewind(&lfs_lfs, &lfs_file) < 0)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		return(false);
	}
	
	queue_header = heap.dzalloc(256);
	if(!queue_header)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		return(false);
	}
	
	if(lfs_file_read(&lfs_lfs, &lfs_file, (void *)queue_header, sizeof(struct __ring_queue_header)) != sizeof(struct __ring_queue_header))
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		heap.free(queue_header);
		return(false);
	}
	
	check = queue_header->check;
	queue_header->check = 0;
	
	__nand_calculate_ecc((void *)queue_header, 256, (uint8_t *)&calc);
	
	if(calc != check)
	{
		if(__nand_correct_data((void *)queue_header, (uint8_t *)&check, (uint8_t *)&calc, 256) < 0)
		{
			lfs_file_close(&lfs_lfs, &lfs_file);
			heap.free(queue_header);
			return(false);
		}
	}
	
	queue_header->current = 0;
	queue_header->amount = 0;
	
	if(lfs_file_rewind(&lfs_lfs, &lfs_file) < 0)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		heap.free(queue_header);
		return(false);
	}
	
	queue_header->check = 0;
	__nand_calculate_ecc((void *)queue_header, 256, (uint8_t *)&calc);
	queue_header->check = calc;
	
	if(lfs_file_size(&lfs_lfs, &lfs_file) != sizeof(struct __ring_queue_header))
	{
		lfs_file_truncate(&lfs_lfs, &lfs_file, sizeof(struct __ring_queue_header));
	}
	
	if(lfs_file_write(&lfs_lfs, &lfs_file, (void *)queue_header, sizeof(struct __ring_queue_header)) != sizeof(struct __ring_queue_header))
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		heap.free(queue_header);
		return(false);
	}
	
	lfs_file_close(&lfs_lfs, &lfs_file);
	heap.free(queue_header);
	return(true);
}

/**
  * @brief  ���ζ���
  */
static bool disk_ring_init(const char *name, uint32_t length)
{
	uint16_t loop;
	lfs_file_t lfs_file;
	struct __ring_queue_header *queue_header;
	uint32_t check;
	char *info;
	int err;
	
	if(!name || lfs_err)
	{
		return(false);
	}
	
    if(lock != 0x5a)
    {
        return(false);
    }
	
    for(loop=0; loop<AMOUNT_FILE; loop++)
    {
        if(file_entry[loop].attr == CT_RING)
        {
			if(strcmp(file_entry[loop].name, name) == 0)
			{
				break;
			}
        }
    }
	
	if(loop >= AMOUNT_FILE)
	{
		return(false);
	}
	
	info = heap.dzalloc(strlen(file_entry[loop].name) + strlen(".r0") + 1);
	
	if(!info)
	{
		return(false);
	}
	
	strcpy(info, file_entry[loop].name);
	strcat(info, ".r0");
	
	err = lfs_file_open(&lfs_lfs, &lfs_file, info, LFS_O_RDWR | LFS_O_CREAT);
	if(err)
	{
		heap.free(info);
		return(false);
	}
	
	heap.free(info);
	
	if(lfs_file_rewind(&lfs_lfs, &lfs_file) < 0)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		return(false);
	}
	
	queue_header = heap.dzalloc(256);
	if(!queue_header)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		return(false);
	}
	
	queue_header->length = length;
	queue_header->capacity = file_entry[loop].size / (queue_header->length + 4);
	
	if(lfs_file_rewind(&lfs_lfs, &lfs_file) < 0)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		heap.free(queue_header);
		return(false);
	}
	
	__nand_calculate_ecc((void *)queue_header, 256, (uint8_t *)&check);
	queue_header->check = check;
	
	if(lfs_file_size(&lfs_lfs, &lfs_file) != sizeof(struct __ring_queue_header))
	{
		lfs_file_truncate(&lfs_lfs, &lfs_file, sizeof(struct __ring_queue_header));
	}
	
	if(lfs_file_write(&lfs_lfs, &lfs_file, (void *)queue_header, sizeof(struct __ring_queue_header)) != sizeof(struct __ring_queue_header))
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		heap.free(queue_header);
		return(false);
	}
	
	lfs_file_close(&lfs_lfs, &lfs_file);
	heap.free(queue_header);
	return(true);
}



/**
  * @brief  ���ػ���
  */
static uint32_t disk_parallel_read(const char *name, uint32_t index, uint32_t size, void *buff)
{
	uint16_t loop;
	lfs_file_t lfs_file;
	lfs_ssize_t readsize;
	struct __parallel_buffer_header *buffer_header;
	uint32_t check, calc;
	uint32_t offset;
	char *info;
	int err;
	
	if(!name || !size || !buff || lfs_err)
	{
		return(0);
	}
	
    for(loop=0; loop<AMOUNT_FILE; loop++)
    {
        if(file_entry[loop].attr == CT_PARALLEL)
        {
			if(strcmp(file_entry[loop].name, name) == 0)
			{
				break;
			}
        }
    }
	
	if(loop >= AMOUNT_FILE)
	{
		return(0);
	}
	
	info = heap.dzalloc(strlen(file_entry[loop].name) + strlen(".p0") + 1);
	
	if(!info)
	{
		return(0);
	}
	
	strcpy(info, file_entry[loop].name);
	strcat(info, ".p0");
	
	err = lfs_file_open(&lfs_lfs, &lfs_file, info, LFS_O_RDONLY);
	if(err)
	{
		heap.free(info);
		return(0);
	}
	
	heap.free(info);
	
	if(lfs_file_rewind(&lfs_lfs, &lfs_file) < 0)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		return(0);
	}
	
	buffer_header = heap.dzalloc(256);
	if(!buffer_header)
	{
		return(0);
	}
	
	readsize = lfs_file_read(&lfs_lfs, &lfs_file, (void *)buffer_header, sizeof(struct __parallel_buffer_header));
	
	lfs_file_close(&lfs_lfs, &lfs_file);
	
	if(readsize != sizeof(struct __parallel_buffer_header))
	{
		heap.free(buffer_header);
		return(0);
	}
	
	check = buffer_header->check;
	buffer_header->check = 0;
	
	__nand_calculate_ecc((void *)buffer_header, 256, (uint8_t *)&calc);
	
	if(calc != check)
	{
		if(__nand_correct_data((void *)buffer_header, (uint8_t *)&check, (uint8_t *)&calc, 256) < 0)
		{
			heap.free(buffer_header);
			return(0);
		}
	}
	
	offset = (index % buffer_header->capacity) * (buffer_header->length + 4);
	
	//TODO:��ȡmap
	
	err = lfs_file_open(&lfs_lfs, &lfs_file, file_entry[loop].name, LFS_O_RDONLY);
	if(err)
	{
		heap.free(buffer_header);
		return(0);
	}
	
	if(lfs_file_seek(&lfs_lfs, &lfs_file, offset, LFS_SEEK_SET) < 0)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		heap.free(buffer_header);
		return(0);
	}
	
	//TODO: ��Ч���ݺ��4�ֽ���ECCУ�飬����֤
	readsize = lfs_file_read(&lfs_lfs, &lfs_file, buff, (size<buffer_header->length?size:buffer_header->length));
	
	lfs_file_close(&lfs_lfs, &lfs_file);
	heap.free(buffer_header);
	return(readsize);
}

/**
  * @brief  ���ػ���
  */
static uint32_t disk_parallel_write(const char *name, uint32_t index, uint32_t size, const void *buff)
{
	uint16_t loop;
	lfs_file_t lfs_file;
	lfs_ssize_t operatesize;
	struct __parallel_buffer_header *buffer_header;
	uint32_t check, calc;
	uint32_t offset;
	char *info;
	int err;
	
	if(!name || !size || !buff || lfs_err)
	{
		return(0);
	}
	
    if(lock != 0x5a)
    {
        return(0);
    }
	
    for(loop=0; loop<AMOUNT_FILE; loop++)
    {
        if(file_entry[loop].attr == CT_PARALLEL)
        {
			if(strcmp(file_entry[loop].name, name) == 0)
			{
				break;
			}
        }
    }
	
	if(loop >= AMOUNT_FILE)
	{
		return(0);
	}
	
	info = heap.dzalloc(strlen(file_entry[loop].name) + strlen(".p0") + 1);
	
	if(!info)
	{
		return(0);
	}
	
	strcpy(info, file_entry[loop].name);
	strcat(info, ".p0");
	
	err = lfs_file_open(&lfs_lfs, &lfs_file, info, LFS_O_RDONLY);
	if(err)
	{
		heap.free(info);
		return(0);
	}
	
	heap.free(info);
	
	if(lfs_file_rewind(&lfs_lfs, &lfs_file) < 0)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		return(0);
	}
	
	buffer_header = heap.dzalloc(256);
	if(!buffer_header)
	{
		return(0);
	}
	
	operatesize = lfs_file_read(&lfs_lfs, &lfs_file, (void *)buffer_header, sizeof(struct __parallel_buffer_header));
	
	lfs_file_close(&lfs_lfs, &lfs_file);
	
	if(operatesize != sizeof(struct __parallel_buffer_header))
	{
		heap.free(buffer_header);
		return(0);
	}
	
	check = buffer_header->check;
	buffer_header->check = 0;
	
	__nand_calculate_ecc((void *)buffer_header, 256, (uint8_t *)&calc);
	
	if(calc != check)
	{
		if(__nand_correct_data((void *)buffer_header, (uint8_t *)&check, (uint8_t *)&calc, 256) < 0)
		{
			heap.free(buffer_header);
			return(0);
		}
	}
	
	offset = (index % buffer_header->capacity) * (buffer_header->length + 4);
	
	err = lfs_file_open(&lfs_lfs, &lfs_file, file_entry[loop].name, LFS_O_RDWR | LFS_O_CREAT);
	if(err)
	{
		heap.free(buffer_header);
		return(0);
	}
	
	if(lfs_file_seek(&lfs_lfs, &lfs_file, offset, LFS_SEEK_SET) < 0)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		heap.free(buffer_header);
		return(0);
	}
	
	//TODO: ��Ч���ݺ��4�ֽ���ECCУ�飬����֤
	operatesize = lfs_file_write(&lfs_lfs, &lfs_file, buff, (size<buffer_header->length?size:buffer_header->length));
	
	lfs_file_close(&lfs_lfs, &lfs_file);
	
	if(operatesize == (size<buffer_header->length?size:buffer_header->length))
	{
		//TODO:����map
		heap.free(buffer_header);
		return(operatesize);
	}
	else
	{
		heap.free(buffer_header);
		return(0);
	}
}

/**
  * @brief  ���ػ���
  */
static bool disk_parallel_signature(const char *name, uint32_t *signature)
{
	uint16_t loop;
	lfs_file_t lfs_file;
	struct __parallel_buffer_header *buffer_header;
	uint32_t check, calc;
	uint32_t index;
	char *info;
	int err;
	
	if(!name || !signature || lfs_err)
	{
		return(false);
	}
	
	*signature = 0;
	
    for(loop=0; loop<AMOUNT_FILE; loop++)
    {
        if(file_entry[loop].attr == CT_PARALLEL)
        {
			if(strcmp(file_entry[loop].name, name) == 0)
			{
				break;
			}
        }
    }
	
	if(loop >= AMOUNT_FILE)
	{
		return(false);
	}
	
	info = heap.dzalloc(strlen(file_entry[loop].name) + strlen(".p0") + 1);
	
	if(!info)
	{
		return(false);
	}
	
	strcpy(info, file_entry[loop].name);
	strcat(info, ".p0");
	
	err = lfs_file_open(&lfs_lfs, &lfs_file, info, LFS_O_RDONLY);
	if(err)
	{
		heap.free(info);
		return(false);
	}
	
	heap.free(info);
	
	if(lfs_file_rewind(&lfs_lfs, &lfs_file) < 0)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		return(false);
	}
	
	buffer_header = heap.dzalloc(256);
	if(!buffer_header)
	{
		return(false);
	}
	
	if(lfs_file_read(&lfs_lfs, &lfs_file, (void *)buffer_header, sizeof(struct __parallel_buffer_header)) != sizeof(struct __parallel_buffer_header))
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		heap.free(buffer_header);
		return(false);
	}
	
	lfs_file_close(&lfs_lfs, &lfs_file);
	
	check = buffer_header->check;
	buffer_header->check = 0;
	
	__nand_calculate_ecc((void *)buffer_header, 256, (uint8_t *)&calc);
	
	if(calc != check)
	{
		if(__nand_correct_data((void *)buffer_header, (uint8_t *)&check, (uint8_t *)&calc, 256) < 0)
		{
			heap.free(buffer_header);
			return(false);
		}
	}
	
	//TODO:��֤map�Ƿ�����
	
	err = lfs_file_open(&lfs_lfs, &lfs_file, file_entry[loop].name, LFS_O_RDONLY);
	if(err)
	{
		heap.free(buffer_header);
		return(false);
	}
	
	if(lfs_file_size(&lfs_lfs, &lfs_file) < buffer_header->capacity * (buffer_header->length + 4))
	{
		heap.free(buffer_header);
		return(false);
	}
	
	if(lfs_file_seek(&lfs_lfs, &lfs_file, 0, LFS_SEEK_SET) < 0)
	{
		heap.free(buffer_header);
		return(false);
	}
	
	info = heap.dzalloc(buffer_header->length + 4);
	
	if(!info)
	{
		heap.free(buffer_header);
		return(false);
	}
	
	for(index=0; index<buffer_header->capacity; index++)
	{
		if(lfs_file_read(&lfs_lfs, &lfs_file, info, (buffer_header->length + 4)) != (buffer_header->length + 4))
		{
			lfs_file_close(&lfs_lfs, &lfs_file);
			heap.free(buffer_header);
			heap.free(info);
			*signature = 0;
			return(false);
		}
		
		*signature = crc32(info, buffer_header->length, *signature);
	}
	
	lfs_file_close(&lfs_lfs, &lfs_file);
	heap.free(info);
	
	info = heap.dzalloc(strlen(file_entry[loop].name) + strlen(".p0") + 1);
	
	if(!info)
	{
		heap.free(buffer_header);
		return(false);
	}
	
	strcpy(info, file_entry[loop].name);
	strcat(info, ".p0");
	
	err = lfs_file_open(&lfs_lfs, &lfs_file, info, LFS_O_RDWR);
	if(err)
	{
		heap.free(buffer_header);
		heap.free(info);
		return(false);
	}
	
	heap.free(info);
	
	if(lfs_file_rewind(&lfs_lfs, &lfs_file) < 0)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		heap.free(buffer_header);
		return(false);
	}
	
	buffer_header->signature = *signature;
	buffer_header->check = 0;
	__nand_calculate_ecc((void *)buffer_header, 256, (uint8_t *)&calc);
	buffer_header->check = calc;
	
	if(lfs_file_write(&lfs_lfs, &lfs_file, (void *)buffer_header, sizeof(struct __parallel_buffer_header)) != sizeof(struct __parallel_buffer_header))
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		heap.free(buffer_header);
		return(false);
	}
	
	if(lfs_file_close(&lfs_lfs, &lfs_file) < 0)
	{
		heap.free(buffer_header);
		return(false);
	}
	
	heap.free(buffer_header);
	return(true);
}

/**
  * @brief  ���ػ���
  */
static bool disk_parallel_status(const char *name, uint32_t index)
{
	uint16_t loop;
	lfs_file_t lfs_file;
	uint8_t buff[2];
	char *info;
	int err;
	
	if(!name || lfs_err)
	{
		return(false);
	}
	
    for(loop=0; loop<AMOUNT_FILE; loop++)
    {
        if(file_entry[loop].attr == CT_PARALLEL)
        {
			if(strcmp(file_entry[loop].name, name) == 0)
			{
				break;
			}
        }
    }
	
	if(loop >= AMOUNT_FILE)
	{
		return(false);
	}
	
	info = heap.dzalloc(strlen(file_entry[loop].name) + strlen(".chk") + 1);
	
	if(!info)
	{
		return(false);
	}
	
	strcpy(info, file_entry[loop].name);
	strcat(info, ".chk");
	
	err = lfs_file_open(&lfs_lfs, &lfs_file, info, LFS_O_RDONLY);
	if(err)
	{
		heap.free(info);
		return(false);
	}
	
	heap.free(info);
	
	if(lfs_file_size(&lfs_lfs, &lfs_file) < (index / 2))
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		return(false);
	}
	
	if(lfs_file_seek(&lfs_lfs, &lfs_file, (index / 2), LFS_SEEK_SET) < 0)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		return(false);
	}
	
	if(lfs_file_read(&lfs_lfs, &lfs_file, (void *)buff, 1) != 1)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		return(false);
	}
	
	if(((buff[0] >> ((index % 2) * 4)) & 0x0f) == 0x0f)
	{
		return(true);
	}
	else
	{
		return(false);
	}
}

/**
  * @brief  ���ػ���
  */
static bool disk_parallel_renew(const char *name, uint32_t index)
{
	uint16_t loop;
	lfs_file_t lfs_file;
	uint8_t buff[2];
	char *info;
	int err;
	
	if(!name || lfs_err)
	{
		return(false);
	}
	
    if(lock != 0x5a)
    {
        return(false);
    }
	
    for(loop=0; loop<AMOUNT_FILE; loop++)
    {
        if(file_entry[loop].attr == CT_PARALLEL)
        {
			if(strcmp(file_entry[loop].name, name) == 0)
			{
				break;
			}
        }
    }
	
	if(loop >= AMOUNT_FILE)
	{
		return(false);
	}
	
	info = heap.dzalloc(strlen(file_entry[loop].name) + strlen(".chk") + 1);
	
	if(!info)
	{
		return(false);
	}
	
	strcpy(info, file_entry[loop].name);
	strcat(info, ".chk");
	
	err = lfs_file_open(&lfs_lfs, &lfs_file, info, LFS_O_RDWR);
	if(err)
	{
		heap.free(info);
		return(false);
	}
	
	heap.free(info);
	
	if(lfs_file_size(&lfs_lfs, &lfs_file) < (index / 2))
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		return(true);
	}
	
	if(lfs_file_seek(&lfs_lfs, &lfs_file, (index / 2), LFS_SEEK_SET) < 0)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		return(false);
	}
	
	if(lfs_file_read(&lfs_lfs, &lfs_file, (void *)buff, 1) != 1)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		return(false);
	}
	
	if(((buff[0] >> ((index % 2) * 4)) & 0x0f) != 0x0f)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		return(true);
	}
	
	(index % 2)? (buff[0] &= 0x0f) : (buff[0] &= 0xf0);
	
	if(lfs_file_seek(&lfs_lfs, &lfs_file, (index / 2), LFS_SEEK_SET) < 0)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		return(false);
	}
	
	if(lfs_file_write(&lfs_lfs, &lfs_file, (void *)buff, 1) != 1)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		return(false);
	}
	
	if(lfs_file_close(&lfs_lfs, &lfs_file) < 0)
	{
		return(false);
	}
	
	return(true);
}

/**
  * @brief  ���ػ���
  */
static bool disk_parallel_info(const char *name, struct __parallel_info *parallel_info)
{
	uint16_t loop;
	lfs_file_t lfs_file;
	struct __parallel_buffer_header *buffer_header;
	uint32_t check, calc;
	char *info;
	int err;
	
	if(!name || !parallel_info || lfs_err)
	{
		return(false);
	}
	
    for(loop=0; loop<AMOUNT_FILE; loop++)
    {
        if(file_entry[loop].attr == CT_PARALLEL)
        {
			if(strcmp(file_entry[loop].name, name) == 0)
			{
				break;
			}
        }
    }
	
	if(loop >= AMOUNT_FILE)
	{
		return(false);
	}
	
	info = heap.dzalloc(strlen(file_entry[loop].name) + strlen(".p0") + 1);
	
	if(!info)
	{
		return(false);
	}
	
	strcpy(info, file_entry[loop].name);
	strcat(info, ".p0");
	
	err = lfs_file_open(&lfs_lfs, &lfs_file, info, LFS_O_RDONLY);
	if(err)
	{
		heap.free(info);
		return(false);
	}
	
	heap.free(info);
	
	if(lfs_file_rewind(&lfs_lfs, &lfs_file) < 0)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		return(false);
	}
	
	buffer_header = heap.dzalloc(256);
	if(!buffer_header)
	{
		return(false);
	}
	
	if(lfs_file_read(&lfs_lfs, &lfs_file, (void *)buffer_header, sizeof(struct __parallel_buffer_header)) != sizeof(struct __parallel_buffer_header))
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		heap.free(buffer_header);
		return(false);
	}
	
	lfs_file_close(&lfs_lfs, &lfs_file);
	
	check = buffer_header->check;
	buffer_header->check = 0;
	
	__nand_calculate_ecc((void *)buffer_header, 256, (uint8_t *)&calc);
	
	if(calc != check)
	{
		if(__nand_correct_data((void *)buffer_header, (uint8_t *)&check, (uint8_t *)&calc, 256) < 0)
		{
			heap.free(buffer_header);
			return(false);
		}
	}
	
	parallel_info->capacity = buffer_header->capacity;
	parallel_info->length = buffer_header->length;
	heap.free(buffer_header);
	return(true);
}

/**
  * @brief  ���ػ���
  */
static bool disk_parallel_reset(const char *name)
{
	uint16_t loop;
	lfs_file_t lfs_file;
	char *info;
	int err;
	
	if(!name || lfs_err)
	{
		return(false);
	}
	
    if(lock != 0x5a)
    {
        return(false);
    }
	
    for(loop=0; loop<AMOUNT_FILE; loop++)
    {
        if(file_entry[loop].attr == CT_PARALLEL)
        {
			if(strcmp(file_entry[loop].name, name) == 0)
			{
				break;
			}
        }
    }
	
	if(loop >= AMOUNT_FILE)
	{
		return(false);
	}
	
	err = lfs_file_open(&lfs_lfs, &lfs_file, file_entry[loop].name, LFS_O_RDWR | LFS_O_CREAT);
	if(err)
	{
		return(false);
	}
	
	lfs_file_truncate(&lfs_lfs, &lfs_file, 0);
	lfs_file_close(&lfs_lfs, &lfs_file);
	
	info = heap.dzalloc(strlen(file_entry[loop].name) + strlen(".chk") + 1);
	
	if(!info)
	{
		return(false);
	}
	
	strcpy(info, file_entry[loop].name);
	strcat(info, ".chk");
	
	err = lfs_file_open(&lfs_lfs, &lfs_file, info, LFS_O_RDWR | LFS_O_CREAT);
	if(err)
	{
		heap.free(info);
		return(false);
	}
	
	heap.free(info);
	
	lfs_file_truncate(&lfs_lfs, &lfs_file, 0);
	lfs_file_close(&lfs_lfs, &lfs_file);
	
	return(true);
}

/**
  * @brief  ���ػ���
  */
static bool disk_parallel_init(const char *name, uint32_t length)
{
	uint16_t loop;
	lfs_file_t lfs_file;
	struct __parallel_buffer_header *buffer_header;
	uint32_t check;
	char *info;
	int err;
	
	if(!name || lfs_err)
	{
		return(false);
	}
	
    if(lock != 0x5a)
    {
        return(false);
    }
	
    for(loop=0; loop<AMOUNT_FILE; loop++)
    {
        if(file_entry[loop].attr == CT_PARALLEL)
        {
			if(strcmp(file_entry[loop].name, name) == 0)
			{
				break;
			}
        }
    }
	
	if(loop >= AMOUNT_FILE)
	{
		return(false);
	}
	
	info = heap.dzalloc(strlen(file_entry[loop].name) + strlen(".r0") + 1);
	
	if(!info)
	{
		return(false);
	}
	
	strcpy(info, file_entry[loop].name);
	strcat(info, ".p0");
	
	err = lfs_file_open(&lfs_lfs, &lfs_file, info, LFS_O_RDWR | LFS_O_CREAT);
	if(err)
	{
		heap.free(info);
		return(false);
	}
	
	heap.free(info);
	
	if(lfs_file_rewind(&lfs_lfs, &lfs_file) < 0)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		return(false);
	}
	
	buffer_header = heap.dzalloc(256);
	if(!buffer_header)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		return(false);
	}
	
	buffer_header->length = length;
	buffer_header->capacity = file_entry[loop].size / (buffer_header->length + 4);
	
	if(lfs_file_rewind(&lfs_lfs, &lfs_file) < 0)
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		heap.free(buffer_header);
		return(false);
	}
	
	__nand_calculate_ecc((void *)buffer_header, 256, (uint8_t *)&check);
	buffer_header->check = check;
	
	if(lfs_file_size(&lfs_lfs, &lfs_file) != sizeof(struct __parallel_buffer_header))
	{
		lfs_file_truncate(&lfs_lfs, &lfs_file, sizeof(struct __parallel_buffer_header));
	}
	
	if(lfs_file_write(&lfs_lfs, &lfs_file, (void *)buffer_header, sizeof(struct __parallel_buffer_header)) != sizeof(struct __parallel_buffer_header))
	{
		lfs_file_close(&lfs_lfs, &lfs_file);
		heap.free(buffer_header);
		return(false);
	}
	
	heap.free(buffer_header);
	lfs_file_close(&lfs_lfs, &lfs_file);
	
	info = heap.dzalloc(strlen(file_entry[loop].name) + strlen(".chk") + 1);
	
	if(!info)
	{
		return(false);
	}
	
	strcpy(info, file_entry[loop].name);
	strcat(info, ".chk");
	
	err = lfs_file_open(&lfs_lfs, &lfs_file, info, LFS_O_RDWR | LFS_O_CREAT);
	if(err)
	{
		heap.free(info);
		return(false);
	}
	
	heap.free(info);
	
	lfs_file_truncate(&lfs_lfs, &lfs_file, 0);
	lfs_file_close(&lfs_lfs, &lfs_file);
	
	return(true);
}



/**
  * @brief  ����ӿ�
  */
struct __file file = 
{
	.parameter		= 
	{
		.read		= disk_parameter_read,
		.write		= disk_parameter_write,
		.size		= disk_parameter_size,
	},
	
	.ring			= 
	{
		.read		=  disk_ring_read,
		.append		=  disk_ring_append,
		.truncate	=  disk_ring_truncate,
		.info		=  disk_ring_info,
		.reset		=  disk_ring_reset,
		.init		=  disk_ring_init,
	},
	
	.parallel			= 
	{
		.read		=  disk_parallel_read,
		.write		=  disk_parallel_write,
		.signature	= disk_parallel_signature,
		.status		=  disk_parallel_status,
		.renew		=  disk_parallel_renew,
		.info		=  disk_parallel_info,
		.reset		=  disk_parallel_reset,
		.init		=  disk_parallel_init,
	},
};
