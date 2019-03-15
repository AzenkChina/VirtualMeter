/**
 * @brief		�ײ��豸����ģ��
 * @details		
 * @date		azenk@2016-11-15
 **/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEVICE_LOWLEVEL_H__
#define __DEVICE_LOWLEVEL_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  cpu��С��
  */
enum __cpu_endian
{
    CPU_LITTLE_ENDIAN = 0x55, //С�˸�ʽ
    CPU_BIG_ENDIAN = 0xAA, //��˸�ʽ
    CPU_UNKNOWN_ENDIAN = 0xFF, //�޷��ж�
};

/**
  * @brief  cpu���еȼ�
  */
enum __cpu_level
{
    CPU_NORMAL = 0,
    CPU_POWERSAVE,
};

/**
  * @brief  �ж�״̬
  */
enum __interrupt_status
{
    INTR_DISABLED = 0,
    INTR_ENABLED = 0xff,
};

/**
  * @brief  ��Դ
  */
enum __power_status
{
    SUPPLY_AC = 0, //������
    SUPPLY_DC, //ֱ����
    SUPPLY_AUX, //���õ�Դ
    SUPPLY_BATTERY, //��ع���
    
    POWER_WAKEUP, //����
    POWER_REBOOT = 0xf3, //����
    POWER_RESET = 0xfc, //����
};


/**
  * @brief  cpu����ģ�ͣ�������������������ӵ��豸�б��е��ȣ�
  */
struct __cpu
{
    struct
    {
        const char                  *(*details)(void); //�ں���Ϣ
        enum __cpu_endian           (*endian)(void); //��С��
        void                        (*init)(enum __cpu_level level); //��ʼ��������
        enum __cpu_level            (*status)(void); //������״̬
        void                        (*sleep)(void); //����
        void                        (*reset)(void); //����������
        
    }                               core;
    
    struct
    {
        void                        (*disable)(void); //����ȫ���ж�
        void                        (*enable)(void); //ʹ��ȫ���ж�
        enum __interrupt_status     (*status)(void); //ȫ���ж�״̬
        
    }                               interrupt;
    
    struct
    {
        uint32_t                    overflow; //���ʱ�� ����
        void                        (*feed)(void); //ι��
        
    }                               watchdog;
    
    struct
    {
        uint32_t                    base; //ram����ַ
        uint32_t                    size; //ram��С
        uint16_t                    (*read)(uint32_t offset, uint16_t size, uint8_t *buffer); //��ȡram����
        uint16_t                    (*write)(uint32_t offset, uint16_t size, const uint8_t *buffer); //����ram����
        
    }                               ram;
    
    struct
    {
        uint32_t                    base; //rom����ַ
        uint32_t                    size; //rom��С
        uint16_t                    (*read)(uint32_t offset, uint16_t size, uint8_t *buffer); //��ȡrom����
        uint16_t                    (*write)(uint32_t offset, uint16_t size, const uint8_t *buffer); //����rom����
        
    }                               rom;
    
    struct
    {
        uint32_t                    base; //reg����ַ
        uint32_t                    size; //reg��С
        uint16_t                    (*read)(uint32_t offset, uint16_t size, uint8_t *buffer); //��ȡreg����
        uint16_t                    (*write)(uint32_t offset, uint16_t size, const uint8_t *buffer); //����reg����
        
    }                               reg;
    
};

/**
  * @brief  jiffy����ģ�ͣ�������������������ӵ��豸�б��е��ȣ�
  */
struct __jiffy
{
    uint32_t                        (*value)(void); //��ȡ��ǰ jiffy ֵ
    uint32_t                        (*after)(uint32_t val); //���� jiffy ��ʷֵ val �� ��ǰ __jiffy ��ʱ����λ����
    uint32_t                        (*hook_intr)(void (*hook)(uint16_t val)); //����жϹ��Ӻ���
};

/**
  * @brief  ��Դ����ģ�ͣ�������������������ӵ��豸�б��е��ȣ�
  */
struct __power
{
    void                            (*init)(void); //��ʼ����Դ���ϵͳ
    enum __power_status             (*status)(void); //��ȡ��ǰϵͳ����Դ
    enum __power_status             (*alter)(enum __power_status status); //�л�״̬
};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif /* __DEVICE_LOWLEVEL_H__ */
