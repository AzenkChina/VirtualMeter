/**
 * @brief		底层设备驱动模型
 * @details		
 * @date		azenk@2016-11-15
 **/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEVICE_LOWLEVEL_H__
#define __DEVICE_LOWLEVEL_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "stdbool.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  cpu大小端
  */
enum __cpu_endian
{
    CPU_LITTLE_ENDIAN = 0x55, //小端格式
    CPU_BIG_ENDIAN = 0xAA, //大端格式
    CPU_UNKNOWN_ENDIAN = 0xFF, //无法判断
};

/**
  * @brief  cpu运行等级
  */
enum __cpu_level
{
    CPU_NORMAL = 0,
    CPU_POWERSAVE,
};

/**
  * @brief  中断状态
  */
enum __interrupt_status
{
    INTR_DISABLED = 0,
    INTR_ENABLED = 0xff,
};

/**
  * @brief  电源
  */
enum __power_status
{
    SUPPLY_AC = 0, //交流电
    SUPPLY_DC, //直流电
    SUPPLY_AUX, //备用电源
    SUPPLY_BATTERY, //电池供电
    
    POWER_WAKEUP, //唤醒
    POWER_REBOOT = 0xf3, //重启
    POWER_RESET = 0xfc, //重置
};


/**
  * @brief  cpu驱动模型（非外设驱动，不可添加到设备列表中调度）
  */
struct __cpu
{
    struct
    {
        const char                  *(*details)(void); //内核信息
        enum __cpu_endian           (*endian)(void); //大小端
        void                        (*init)(enum __cpu_level level); //初始化处理器
        enum __cpu_level            (*status)(void); //处理器状态
        void                        (*sleep)(void); //休眠
        void                        (*reset)(void); //重启处理器
        void                        (*idle)(uint16_t tick); //空闲
		
    }                               core;
    
    struct
    {
        void                        (*disable)(void); //禁用全局中断
        void                        (*enable)(void); //使能全局中断
        enum __interrupt_status     (*status)(void); //全局中断状态
        bool                        (*request)(uint8_t n, void(*hook)(void)); //注册一个中断
        bool                        (*release)(uint8_t n); //释放中断
        
    }                               interrupt;
    
    struct
    {
        uint32_t                    overflow; //溢出时间 毫秒
        void                        (*feed)(void); //喂狗
        
    }                               watchdog;
    
    struct
    {
        uint32_t                    base; //ram基地址
        uint32_t                    size; //ram大小
        uint16_t                    (*read)(uint32_t offset, uint16_t size, uint8_t *buffer); //获取ram数据
        uint16_t                    (*write)(uint32_t offset, uint16_t size, const uint8_t *buffer); //设置ram数据
        
    }                               ram;
    
    struct
    {
        uint32_t                    base; //rom基地址
        uint32_t                    size; //rom大小
        uint16_t                    (*read)(uint32_t offset, uint16_t size, uint8_t *buffer); //获取rom数据
        uint16_t                    (*write)(uint32_t offset, uint16_t size, const uint8_t *buffer); //设置rom数据
        
    }                               rom;
    
    struct
    {
        uint32_t                    base; //reg基地址
        uint32_t                    size; //reg大小
        uint16_t                    (*read)(uint32_t offset, uint16_t size, uint8_t *buffer); //获取reg数据
        uint16_t                    (*write)(uint32_t offset, uint16_t size, const uint8_t *buffer); //设置reg数据
        
    }                               reg;
    
};

/**
  * @brief  jiffy驱动模型（非外设驱动，不可添加到设备列表中调度）
  */
struct __jiffy
{
    uint32_t                        (*value)(void); //获取当前 jiffy 值
    uint32_t                        (*after)(uint32_t val); //计算 jiffy 历史值 val 与 当前 __jiffy 的时间差，单位毫秒
};

/**
  * @brief  电源驱动模型（非外设驱动，不可添加到设备列表中调度）
  */
struct __power
{
    void                            (*init)(void); //初始化电源检测系统
    enum __power_status             (*status)(void); //获取当前系统供电源
    enum __power_status             (*alter)(enum __power_status status); //切换状态
};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif /* __DEVICE_LOWLEVEL_H__ */
