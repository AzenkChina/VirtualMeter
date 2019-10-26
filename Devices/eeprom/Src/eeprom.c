/**
 * @brief		
 * @details		
 * @date		2016-11-15
 **/

/* Includes ------------------------------------------------------------------*/
#include "eeprom.h"
#include "eeprom_1.h"
#include "eeprom_2.h"
#include "string.h"
#include "crc.h"
#include "trace.h"

#if defined (DEMO_STM32F091)
#include "stm32f0xx.h"
#endif

/* Private typedef -----------------------------------------------------------*/
struct __page_cache
{
    uint32_t page;//ҳ��ַ
    uint8_t data[64];//��Ҫ��֤�������豸ҳ��С����һ��
};

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
//ҳ��С��Ϊ�����豸ҳ��С��ȥҳУ����Ϣ��С
#define EEP_PAGE_SIZE          (eeprom_1.info.pagesize() - 4)
//ҳ����
#define EEP_PAGE_AMOUNT        eeprom_1.info.pageamount()
//оƬ����
#define EEP_CHIP_SIZE          EEP_PAGE_SIZE * eeprom_1.info.pageamount()

/* Private variables ---------------------------------------------------------*/
static enum __dev_status status = DEVICE_NOTINIT;
//ҳ�������
static struct __page_cache rcache;
//ҳ��д����
static struct __page_cache wcache;

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
    
    memset(&rcache, 0xff, sizeof(rcache));
    memset(&wcache, 0xff, sizeof(wcache));
	eeprom_1.control.init(state);
	eeprom_2.control.init(state);
    status = DEVICE_INIT;
    
    //��ƬEEPROM ʹ������ RAID 1 ��ʽ����֤���ݰ�ȫ
    //��˵���ƬEEPROM��һ��ʱ����Ҫ���⴦��
    ASSERT(eeprom_1.info.pagesize() != eeprom_2.info.pagesize());
    ASSERT(eeprom_1.info.pageamount() != eeprom_2.info.pageamount());
    ASSERT(eeprom_1.info.chipsize() != eeprom_2.info.chipsize());
    
    ASSERT(eeprom_1.info.pagesize() != sizeof(rcache.data));
}

/**
  * @brief  
  */
static void eep_suspend(void)
{
    uint32_t check;
    
    if(wcache.page < EEP_PAGE_AMOUNT)
    {
        check = wcache.data[sizeof(wcache.data) - 4];
        check <<= 8;
        check += wcache.data[sizeof(wcache.data) - 3];
        check <<= 8;
        check += wcache.data[sizeof(wcache.data) - 2];
        check <<= 8;
        check += wcache.data[sizeof(wcache.data) - 1];
        
        //У����ȷ��д�ش洢��
        if(check == crc32(wcache.data, (sizeof(wcache.data) - 4)))
        {
            if(eeprom_1.page.write(wcache.page, 0, sizeof(wcache.data), wcache.data) != sizeof(wcache.data))
            {
                TRACE(TRACE_WARN, "Write main eeprom faild while suspending.");
                
                if(eeprom_2.page.write(wcache.page, 0, sizeof(wcache.data), wcache.data) != sizeof(wcache.data))
                {
                    TRACE(TRACE_ERR, "Write redundancy eeprom faild while suspending.");
                }
            }
            else
            {
                if(eeprom_2.page.write(wcache.page, 0, sizeof(wcache.data), wcache.data) != sizeof(wcache.data))
                {
                    TRACE(TRACE_WARN, "Write redundancy eeprom faild while suspending.");
                }
            }
        }
    }
    
	eeprom_1.control.suspend();
	eeprom_2.control.suspend();
    
    memset(&rcache, 0xff, sizeof(rcache));
    memset(&wcache, 0xff, sizeof(wcache));
    
    status = DEVICE_SUSPENDED;
}

static uint32_t eep_page_read(uint32_t page, uint16_t offset, uint16_t size, uint8_t * buffer)
{
    uint32_t check;
    
    ASSERT((offset) >= (sizeof(rcache.data) - 4));
    ASSERT((offset + size) > (sizeof(rcache.data) - 4));
    
    //��� ����ҳ�� �Ƿ����� ��������
    //��� ����ҳ�� ���� �������� ��ֱ�Ӷ�ȡ
    if(page == rcache.page)
    {
        check = rcache.data[sizeof(rcache.data) - 4];
        check <<= 8;
        check += rcache.data[sizeof(rcache.data) - 3];
        check <<= 8;
        check += rcache.data[sizeof(rcache.data) - 2];
        check <<= 8;
        check += rcache.data[sizeof(rcache.data) - 1];
        
        if(check == crc32(rcache.data, (sizeof(rcache.data) - 4)))
        {
            memcpy(buffer, &rcache.data[offset], size);
            return(size);
        }
    }
    
    rcache.page = 0xffffffff;
    
    //���洢��ȡʧ��
    if(eeprom_1.page.read(page, 0, sizeof(rcache.data), rcache.data) != sizeof(rcache.data))
    {
        TRACE(TRACE_WARN, "Read main eeprom faild.");
        
        //����洢��ȡʧ��
        if(eeprom_2.page.read(page, 0, sizeof(rcache.data), rcache.data) != sizeof(rcache.data))
        {
            TRACE(TRACE_ERR, "Read redundancy eeprom faild.");
            return(0);
        }
        //����洢��ȡ�ɹ�
        else
        {
            check = rcache.data[sizeof(rcache.data) - 4];
            check <<= 8;
            check += rcache.data[sizeof(rcache.data) - 3];
            check <<= 8;
            check += rcache.data[sizeof(rcache.data) - 2];
            check <<= 8;
            check += rcache.data[sizeof(rcache.data) - 1];
            
            //����洢У��ɹ�
            if(check == crc32(rcache.data, (sizeof(rcache.data) - 4)))
            {
                memcpy(buffer, &rcache.data[offset], size);
                rcache.page = page;
                return(size);
            }
            //����洢У��ʧ��
            else
            {
                TRACE(TRACE_ERR, "Check redundancy eeprom faild.");
                return(0);
            }
        }
    }
    //���洢��ȡ�ɹ�
    else
    {
        check = rcache.data[sizeof(rcache.data) - 4];
        check <<= 8;
        check += rcache.data[sizeof(rcache.data) - 3];
        check <<= 8;
        check += rcache.data[sizeof(rcache.data) - 2];
        check <<= 8;
        check += rcache.data[sizeof(rcache.data) - 1];
        
        //���洢У��ɹ�
        if(check == crc32(rcache.data, (sizeof(rcache.data) - 4)))
        {
            memcpy(buffer, &rcache.data[offset], size);
            rcache.page = page;
            return(size);
        }
        //���洢У��ʧ��
        else
        {
            TRACE(TRACE_WARN, "Check main eeprom faild.");
            
            //����洢��ȡʧ��
            if(eeprom_2.page.read(page, 0, sizeof(rcache.data), rcache.data) != sizeof(rcache.data))
            {
                TRACE(TRACE_ERR, "Read redundancy eeprom faild.");
                return(0);
            }
            //����洢��ȡ�ɹ�
            else
            {
                check = rcache.data[sizeof(rcache.data) - 4];
                check <<= 8;
                check += rcache.data[sizeof(rcache.data) - 3];
                check <<= 8;
                check += rcache.data[sizeof(rcache.data) - 2];
                check <<= 8;
                check += rcache.data[sizeof(rcache.data) - 1];
                
                //����洢У��ɹ�
                if(check == crc32(rcache.data, (sizeof(rcache.data) - 4)))
                {
                    memcpy(buffer, &rcache.data[offset], size);
                    rcache.page = page;
                    return(size);
                }
                //����洢У��ʧ��
                else
                {
                    TRACE(TRACE_ERR, "Check redundancy eeprom faild.");
                    return(0);
                }
            }
        }
    }
}

static uint32_t eep_page_write(uint32_t page, uint16_t offset, uint16_t size, const uint8_t *buffer)
{
    uint32_t check;
    
    ASSERT((offset) >= (sizeof(rcache.data) - 4));
    ASSERT((offset + size) > (sizeof(rcache.data) - 4));
    
    //��� ����ҳ�� �Ƿ����� д������
    //��� ����ҳ�� ���� д������ ����� д����
    //��� ����ҳ�� ���� д������ �� д���� д�� Ȼ�����¼����µ�ҳ�浽д�������ٸ���
    if(page == wcache.page)
    {
        //����д����
        memcpy(&wcache.data[offset], buffer, size);
        
        //���ҳУ��
        check = crc32(wcache.data, (sizeof(wcache.data) - 4));
        wcache.data[sizeof(wcache.data) - 4] = ((check >> 24) & 0xff);
        wcache.data[sizeof(wcache.data) - 3] = ((check >> 16) & 0xff);
        wcache.data[sizeof(wcache.data) - 2] = ((check >> 8) & 0xff);
        wcache.data[sizeof(wcache.data) - 1] = ((check >> 0) & 0xff);
        
        return(size);
    }
    else
    {
        //д�ص�ǰ��Ч�� д����
        if(wcache.page < EEP_PAGE_AMOUNT)
        {
            check = wcache.data[sizeof(wcache.data) - 4];
            check <<= 8;
            check += wcache.data[sizeof(wcache.data) - 3];
            check <<= 8;
            check += wcache.data[sizeof(wcache.data) - 2];
            check <<= 8;
            check += wcache.data[sizeof(wcache.data) - 1];
            
            //У����ȷ��д�ش洢��
            if(check == crc32(wcache.data, (sizeof(wcache.data) - 4)))
            {
                if(eeprom_1.page.write(wcache.page, 0, sizeof(wcache.data), wcache.data) != sizeof(wcache.data))
                {
                    TRACE(TRACE_WARN, "Write main eeprom faild.");
                    
                    if(eeprom_2.page.write(wcache.page, 0, sizeof(wcache.data), wcache.data) != sizeof(wcache.data))
                    {
                        TRACE(TRACE_ERR, "Write redundancy eeprom faild.");
                    }
                }
                else
                {
                    if(eeprom_2.page.write(wcache.page, 0, sizeof(wcache.data), wcache.data) != sizeof(wcache.data))
                    {
                        TRACE(TRACE_WARN, "Write redundancy eeprom faild.");
                    }
                }
            }
        }
        
        //���洢��ȡʧ��
        if(eeprom_1.page.read(page, 0, sizeof(wcache.data), wcache.data) != sizeof(wcache.data))
        {
            TRACE(TRACE_WARN, "Read main eeprom faild.");
            
            //����洢��ȡʧ��
            if(eeprom_2.page.read(page, 0, sizeof(wcache.data), wcache.data) != sizeof(wcache.data))
            {
                TRACE(TRACE_ERR, "Read redundancy eeprom faild.");
            }
            //����洢��ȡ�ɹ�
            else
            {
                check = wcache.data[sizeof(wcache.data) - 4];
                check <<= 8;
                check += wcache.data[sizeof(wcache.data) - 3];
                check <<= 8;
                check += wcache.data[sizeof(wcache.data) - 2];
                check <<= 8;
                check += wcache.data[sizeof(wcache.data) - 1];
                
                //����洢У��ʧ��
                if(check != crc32(wcache.data, (sizeof(wcache.data) - 4)))
                {
                    TRACE(TRACE_ERR, "Check redundancy eeprom faild.");
                }
            }
        }
        //���洢��ȡ�ɹ�
        else
        {
            check = wcache.data[sizeof(wcache.data) - 4];
            check <<= 8;
            check += wcache.data[sizeof(wcache.data) - 3];
            check <<= 8;
            check += wcache.data[sizeof(wcache.data) - 2];
            check <<= 8;
            check += wcache.data[sizeof(wcache.data) - 1];
            
            //���洢У��ʧ��
            if(check != crc32(wcache.data, (sizeof(wcache.data) - 4)))
            {
                TRACE(TRACE_WARN, "Check main eeprom faild.");
                
                //����洢��ȡʧ��
                if(eeprom_2.page.read(page, 0, sizeof(wcache.data), wcache.data) != sizeof(wcache.data))
                {
                    TRACE(TRACE_ERR, "Read redundancy eeprom faild.");
                }
                //����洢��ȡ�ɹ�
                else
                {
                    check = wcache.data[sizeof(wcache.data) - 4];
                    check <<= 8;
                    check += wcache.data[sizeof(wcache.data) - 3];
                    check <<= 8;
                    check += wcache.data[sizeof(wcache.data) - 2];
                    check <<= 8;
                    check += wcache.data[sizeof(wcache.data) - 1];
                    
                    //����洢У��ʧ��
                    if(check != crc32(wcache.data, (sizeof(wcache.data) - 4)))
                    {
                        TRACE(TRACE_ERR, "Check redundancy eeprom faild.");
                    }
                }
            }
        }
        
        //����д����
        memcpy(&wcache.data[offset], buffer, size);
        
        wcache.page = page;
        
        return(size);
    }
}

static uint32_t eep_erase(void)
{
	eeprom_1.erase();
	eeprom_2.erase();
    memset(&rcache, 0xff, sizeof(rcache));
    memset(&wcache, 0xff, sizeof(wcache));
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
