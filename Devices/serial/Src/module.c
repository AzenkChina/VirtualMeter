/**
 * @brief		
 * @details		
 * @date		2016-08-16
 **/

/* Includes ------------------------------------------------------------------*/
#include "module.h"
#include "string.h"
#include "vuart4.h"
#include "cpu.h"
#include "delay.h"
#include "trace.h"

#if defined (BUILD_REAL_WORLD)
#include "stm32f0xx.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define UART_USED			vuart4

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static enum __dev_status status = DEVICE_NOTINIT;

/**
  * @brief	
  */
static struct __serial_state
{
	enum __bus_status			status;//总线状态
	
	uint8_t						*rx_buff;//接收缓冲
	uint16_t					rx_buff_size;//接收缓冲大小
	uint16_t					rx_w_index;//写索引
	uint16_t					rx_frame_length;//帧长度
	
	uint8_t						*tx_buff;//发送缓冲
	uint16_t					tx_buff_size;//发送缓冲大小
	uint16_t					tx_data_size;//发送数据大小
	
	uint16_t					timeout_config;//数据帧超时时间
	uint32_t					timeout_rx_counter;//当前时间
	uint32_t					timeout_tx_counter;//当前时间
	
} serial_state;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  
  */
static void recv_callback(uint8_t ch)
{
	if((!(serial_state.rx_buff)) || (!(serial_state.rx_buff_size)))
	{
		return;
	}
	
	if(serial_state.rx_w_index >= serial_state.rx_buff_size)
	{
		TRACE(TRACE_WARN, "Optical rx buffer overflowed.");
		return;
	}
	
	*(serial_state.rx_buff + serial_state.rx_w_index) = ch;
	serial_state.rx_w_index += 1;
	serial_state.timeout_rx_counter = 0;
}

/**
  * @brief  
  */
static enum __dev_status module_status(void)
{
    return(status);
}

/**
  * @brief  
  */
static void module_init(enum __dev_state state)
{
#if defined (BUILD_REAL_WORLD)
    GPIO_InitTypeDef GPIO_InitStruct;
#endif
	UART_USED.control.init(state);
	
    serial_state.status = BUS_IDLE;
    serial_state.rx_buff = (uint8_t *)0;
    serial_state.rx_buff_size = 0;
    serial_state.rx_w_index = 0;
    serial_state.rx_frame_length = 0;
    serial_state.tx_buff = (uint8_t *)0;;
    serial_state.tx_buff_size = 0;
    serial_state.tx_data_size = 0;
    serial_state.timeout_config = 50;
    serial_state.timeout_rx_counter = 0;
    serial_state.timeout_tx_counter = 0;
    
	if(state == DEVICE_NORMAL)
	{
#if defined (BUILD_REAL_WORLD)
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
		
		//DECT
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOA, &GPIO_InitStruct);
		GPIO_SetBits(GPIOA, GPIO_Pin_0);
		
		//RST CFG
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_4;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOE, &GPIO_InitStruct);
		GPIO_SetBits(GPIOE, GPIO_Pin_2 | GPIO_Pin_4);
		
		//LINK
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
        GPIO_Init(GPIOE, &GPIO_InitStruct);
		
		module.reset();
#endif
		UART_USED.handler.filling(recv_callback);
	}
    
    status = DEVICE_INIT;
}

/**
  * @brief  
  */
static void module_suspend(void)
{
	UART_USED.handler.remove();
	UART_USED.control.suspend();
	
	serial_state.status = BUS_IDLE;
	serial_state.rx_buff = (uint8_t *)0;
	serial_state.rx_buff_size = 0;
	serial_state.rx_w_index = 0;
	serial_state.rx_frame_length = 0;
	serial_state.tx_buff = (uint8_t *)0;;
	serial_state.tx_buff_size = 0;
	serial_state.tx_data_size = 0;
	serial_state.timeout_config = 50;
	serial_state.timeout_rx_counter = 0;
	serial_state.timeout_tx_counter = 0;
	
    status = DEVICE_SUSPENDED;
}




/**
  * @brief  
  */
static void module_runner(uint16_t msecond)
{
    //接收没有超时
	if(serial_state.timeout_rx_counter < serial_state.timeout_config)
	{
		serial_state.timeout_rx_counter += msecond;
	}
    //接收超时
	else
	{
        //如果接收缓冲中有数据
		if(serial_state.rx_w_index)
		{
            //如果数据帧长度没有被赋值
			if(!(serial_state.rx_frame_length))
			{
                //赋值数据帧长度
				serial_state.rx_frame_length = serial_state.rx_w_index;
			}
            //如果数据帧长度已赋值
			else
			{
				//等待1秒后，复位接收写地址
				if(serial_state.timeout_rx_counter < ((uint32_t)(serial_state.timeout_config) + 1000))
				{
                    serial_state.timeout_rx_counter += msecond;
                    if(serial_state.timeout_rx_counter >= ((uint32_t)(serial_state.timeout_config) + 1000))
                    {
                        serial_state.rx_w_index = 0;
                    }
				}
			}
		}
	}
	
	//发送正在延时
	if(serial_state.timeout_tx_counter < serial_state.timeout_config)
	{
		serial_state.timeout_tx_counter += msecond;
	}
    //发送延时完成
	else
	{
        //串口状态忙
		if(UART_USED.status() != BUS_IDLE)
		{
			return;
		}
		
        //串口状态已空闲，如果总线状态忙，则复位总线状态
		if(serial_state.status == BUS_TRANSFER)
		{
            serial_state.status = BUS_IDLE;
			serial_state.tx_data_size = 0;
		}
		
        //有数据等待传输
		if(serial_state.tx_data_size)
		{
			serial_state.status = BUS_TRANSFER;
			UART_USED.write(serial_state.tx_data_size, serial_state.tx_buff);
		}
	}
}

/**
  * @brief  
  */
static uint16_t module_read(uint16_t max_size, uint8_t *buffer)
{
	uint16_t length = serial_state.rx_frame_length;
	
	if(!(serial_state.rx_buff))
	{
		return(0);
	}
	
	if(!length)
	{
		return(0);
	}
	
	if(length > max_size)
	{
		length = max_size;
	}
	
	memcpy((void *)buffer, (const void *)serial_state.rx_buff, length);
	
	serial_state.rx_w_index = 0;
	serial_state.rx_frame_length = 0;
	
	return(length);
}

/**
  * @brief  
  */
static uint16_t module_write(uint16_t count)
{
	if(!(serial_state.tx_buff))
	{
		return(0);
	}
	
	if(serial_state.status != BUS_IDLE)
	{
		return(0);
	}
	
	if(count > serial_state.tx_buff_size)
	{
		count = serial_state.tx_buff_size;
	}
	
	serial_state.status = BUS_BUSY;
	serial_state.tx_data_size = count;
	serial_state.timeout_tx_counter = 0;
	
	return(serial_state.tx_data_size);
}

/**
  * @brief  
  */
static enum __bus_status module_bus_status(void)
{
	return(serial_state.status);
}

/**
  * @brief  
  */
static void module_rxbuff_set(uint16_t size, uint8_t *buffer)
{
	enum __interrupt_status intr_status = cpu.interrupt.status();
	
	if(intr_status == INTR_ENABLED)
	{
		cpu.interrupt.disable();
	}
	
	serial_state.rx_buff = buffer;
	serial_state.rx_buff_size = size;
	serial_state.rx_w_index = 0;
	serial_state.rx_frame_length = 0;
	serial_state.timeout_rx_counter = 0;
	
	if(intr_status == INTR_ENABLED)
	{
		cpu.interrupt.enable();
	}
}

/**
  * @brief  
  */
static uint16_t module_rxbuff_get(uint8_t **buffer)
{
	*buffer = serial_state.rx_buff;
	
	return(serial_state.rx_buff_size);
}

/**
  * @brief  
  */
static void module_rxbuff_remove(void)
{
	enum __interrupt_status intr_status = cpu.interrupt.status();
	
	if(intr_status == INTR_ENABLED)
	{
		cpu.interrupt.disable();
	}
	
	serial_state.rx_buff = (uint8_t *)0;
	serial_state.rx_buff_size = 0;
	serial_state.rx_w_index = 0;
	serial_state.rx_frame_length = 0;
	serial_state.timeout_rx_counter = 0;
	
	if(intr_status == INTR_ENABLED)
	{
		cpu.interrupt.enable();
	}
}


/**
  * @brief  
  */
static void module_txbuff_set(uint16_t size, uint8_t *buffer)
{
	serial_state.status = BUS_IDLE;
	serial_state.tx_buff = (uint8_t *)buffer;
	serial_state.tx_buff_size = size;
	serial_state.tx_data_size = 0;
	serial_state.timeout_tx_counter = 0;
}

/**
  * @brief  
  */
static uint16_t module_txbuff_get(uint8_t **buffer)
{
	*buffer = serial_state.tx_buff;
	
	return(serial_state.tx_buff_size);
}

/**
  * @brief  
  */
static void module_txbuff_remove(void)
{
	serial_state.status = BUS_IDLE;
	serial_state.tx_buff = (uint8_t *)0;;
	serial_state.tx_buff_size = 0;
	serial_state.tx_data_size = 0;
	serial_state.timeout_tx_counter = 0;
}


/**
  * @brief  
  */
static uint16_t module_timeout_config(uint16_t msecond)
{
	if((msecond < 10) || (msecond > 3000))
	{
		msecond = 100;
	}
	
	serial_state.timeout_config = msecond;
	
	return(serial_state.timeout_config);
}

/**
  * @brief  
  */
static uint16_t module_timeout_read(void)
{
	return(serial_state.timeout_config);
}


/**
  * @brief  
  */
static bool module_linked(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	return(true);
#else
#if defined (BUILD_REAL_WORLD)
	static bool linked = false;
	
	if(linked == false)
	{
		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3) == Bit_RESET)
		{
			udelay(100);
			if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3) == Bit_RESET)
			{
				linked = true;
			}
		}
	}
	else
	{
		if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3) == Bit_SET)
		{
			udelay(100);
			if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3) == Bit_SET)
			{
				linked = false;
			}
		}
	}
	
	return(linked);
#endif
#endif
}

/**
  * @brief  
  */
static bool module_detect(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	return(true);
#else
#if defined (BUILD_REAL_WORLD)
	if(module_linked())
	{
		return(true);
	}
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_0);
	
	mdelay(20);
	
    if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3) == Bit_RESET)
    {
		GPIO_SetBits(GPIOA, GPIO_Pin_0);
		return(true);
    }
	
	GPIO_SetBits(GPIOA, GPIO_Pin_0);
	return(false);
#endif
#endif
}

/**
  * @brief  
  */
static void module_config(bool state)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	return;
#else
#if defined (BUILD_REAL_WORLD)
	if(state)
	{
		GPIO_ResetBits(GPIOE, GPIO_Pin_4);
	}
	else
	{
		GPIO_SetBits(GPIOE, GPIO_Pin_4);
	}
#endif
#endif
}

/**
  * @brief  
  */
static void module_reset(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	return;
#else
#if defined (BUILD_REAL_WORLD)
	GPIO_ResetBits(GPIOE, GPIO_Pin_2);
	mdelay(250);
	GPIO_SetBits(GPIOE, GPIO_Pin_2);
#endif
#endif
}



/**
  * @brief  
  */
const struct __module module = 
{
	.serial				= 
	{
		.control        = 
		{
			.name       = "module port",
			.status     = module_status,
			.init       = module_init,
			.suspend    = module_suspend,
		},
		
		.runner			= module_runner,
		.read			= module_read,
		.write			= module_write,
		.status			= module_bus_status,
		
		.rxbuff			= 
		{
			.get		= module_rxbuff_get,
			.set		= module_rxbuff_set,
			.remove		= module_rxbuff_remove,
		},
		
		.txbuff			= 
		{
			.get		= module_txbuff_get,
			.set		= module_txbuff_set,
			.remove		= module_txbuff_remove,
		},
		
		.timeout		=  
		{
			.get		= module_timeout_read,
			.set		= module_timeout_config,
		},
		
		.uart			= &UART_USED,
	},
	
	.linked				= module_linked,
	.detect				= module_detect,
	.config				= module_config,
	.reset				= module_reset,
};
