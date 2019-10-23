/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __METER_H__
#define __METER_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "device.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  校准基础
  */
struct __calibrate_param
{
	uint32_t step; //校准流程
	uint32_t voltage; //校准电压
	uint32_t current; //校准电流
	uint32_t ppulse; //有功电能脉冲系数
	uint32_t qpulse; //无功电能脉冲系数
};

#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
/**
  * @brief  校准值
  */
struct __calibrate_data
{
	uint8_t reserve[504]; //虚拟电表，无校准参数，仅供占位
	
	uint32_t check;
};
#else

#if defined (DEMO_STM32F091)
/**
  * @brief  7022 寄存器
  */
struct __calibrate_register
{
	uint16_t address;
	uint16_t value;
};

/**
  * @brief  7022 校准值
  */
struct __calibrate_data
{
	struct __calibrate_register reg[59];
	
	uint32_t check;
};
#endif

#endif


/**
  * @brief 校准
  */
struct __calibrates
{
	struct __calibrate_param param;
	struct __calibrate_data data;
};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern const struct __meter meter;
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
uint8_t is_powered(void);
#endif

#endif /* __METER_H__ */
