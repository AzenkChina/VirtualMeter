/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TYPES_LOGGER_H__
#define __TYPES_LOGGER_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "stdbool.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  事件标识
  * 		这里只提供标识，不代表事件是有效的，事件的有效与否在 事件配置表 中定义
  */
enum __event_id
{
    EVI_ALL				= 0, //事件ID开始
    
    EVI_POWERUP			= 1, //上电事件
    EVI_POWERDOWN		= 2, //掉电事件
    //...添加其它事件
    
    EVI_MAX				= 256, //事件ID上限（数值可修改，名称不要随意修改）
};

/**
  * @brief  事件状态
  */
enum __event_status
{
	EVS_STARTING	= 1, //事件正要发生（延时中）
    EVS_ENDING		= 2, //事件正要结束（延时中）
	EVS_STARTED		= 3, //事件已发生
    EVS_ENDED		= 0, //事件已结束
};


/**
  * @brief  logger task 的对外接口
  *			void (*callback)(enum __event_id id, enum __event_status status, uint16_t mapping)
  */
struct __logger
{
	struct
	{
    	enum __event_status			(*read)(enum __event_id id); //读取事件状态
		bool						(*toggle)(enum __event_id id, enum __event_status status); //修改事件状态
		
	}								status;
	struct
	{
		bool						(*add)(void (*callback)(enum __event_id, enum __event_status, uint16_t)); //添加回调
		bool						(*remove)(void (*callback)(enum __event_id, enum __event_status, uint16_t)); //删除回调
		
	}								monitor;
	
	uint16_t						(*map)(enum __event_id id);
	enum __event_id					(*unmap)(uint16_t mapping);
};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif /* __API_LOGGER_H__ */
