/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TYPES_COMM_H__
#define __TYPES_COMM_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "device.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief	端口类型
  */
enum __port_type
{
	PORT_TTL = 0,
	PORT_RS232 = 1,
	PORT_RS485 = 2,
	PORT_RS422 = 3,
	PORT_OPTICAL = 4,
	PORT_MODULE = 5,
};

/**
  * @brief	端口状态 
  */
enum __port_status
{
    PORT_IDLE = 0,
    PORT_BUSY,
};


/**
  * @brief  protocol task 的对外接口
  */
struct __comm
{
    struct
    {
		uint8_t						(*channel)(void); //获取当前通道
        enum __port_type			(*type)(uint8_t channel); //获取端口类型 
		uint16_t					(*protocol)(uint8_t channel); //获取端口支持的协议 
        enum __port_status			(*status)(uint8_t channel); //获取端口状态 
		bool						(*link)(uint8_t channel); //端口链路是否正常 
        
    }                               attrib;
    
    struct
    {
        uint16_t                    (*set)(uint8_t channel, uint16_t msecond); //设置帧超时时间
        uint16_t                    (*get)(uint8_t channel); //获取当前帧超时时间
        
    }                               timeout;
    
    struct
    {
        enum __baud                 (*set)(uint8_t channel, enum __baud baudrate); //设置波特率
        enum __baud                 (*get)(uint8_t channel); //获取当前波特率
        
    }                               baudrate;
    
    struct
    {
        enum __parity				(*set)(uint8_t channel, enum __parity parity); //设置校验位
        enum __parity				(*get)(uint8_t channel); //获取当前校验位
        
    }                                parity;
    
    struct
    {
        enum __stop					(*set)(uint8_t channel, enum __stop stop); //设置停止位
        enum __stop					(*get)(uint8_t channel); //获取当前停止位
        
    }                                stop;
	
	struct
	{
		bool						(*add)(void (*callback)(uint8_t, bool)); //添加回调
		bool						(*remove)(void (*callback)(uint8_t, bool)); //删除回调
		
	}								monitor;
	
	void							(*config)(bool state); //模块配置信息变化
	void							(*reset)(uint8_t channel); //复位端口
};


/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif /* __TYPES_COMM_H__ */
