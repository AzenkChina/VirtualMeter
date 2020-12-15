/**
 * @brief		
 * @details		
 * @date		2016-08-16
 **/

/* Includes ------------------------------------------------------------------*/
#include "optical.h"
#include "string.h"
#include "vuart3.h"
#include "cpu.h"
#include "delay.h"
#include "trace.h"

#if defined (BUILD_REAL_WORLD)
#include "stm32f0xx.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define UART_USED			vuart3

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
	
	//void self receive
	if(serial_state.status == BUS_TRANSFER)
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
static enum __dev_status optical_status(void)
{
    return(status);
}

/**
  * @brief  
  */
static void optical_init(enum __dev_state state)
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
		
		//CVIR
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOA, &GPIO_InitStruct);
		GPIO_SetBits(GPIOA, GPIO_Pin_6);
		
		//CVTXD
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOE, &GPIO_InitStruct);
		GPIO_SetBits(GPIOE, GPIO_Pin_9);
#endif
		UART_USED.handler.filling(recv_callback);
	}
    
    status = DEVICE_INIT;
}

/**
  * @brief  
  */
static void optical_suspend(void)
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
static void optical_runner(uint16_t msecond)
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
#if defined (BUILD_REAL_WORLD)
			GPIO_SetBits(GPIOE, GPIO_Pin_9);
#endif
            serial_state.status = BUS_IDLE;
			serial_state.tx_data_size = 0;
		}
		
        //有数据等待传输
		if(serial_state.tx_data_size)
		{
#if defined (BUILD_REAL_WORLD)
			GPIO_ResetBits(GPIOE, GPIO_Pin_9);
			udelay(100);
#endif
			serial_state.status = BUS_TRANSFER;
			UART_USED.write(serial_state.tx_data_size, serial_state.tx_buff);
		}
	}
}

/**
  * @brief  
  */
static uint16_t optical_read(uint16_t max_size, uint8_t *buffer)
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
static uint16_t optical_write(uint16_t count)
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
static enum __bus_status optical_bus_status(void)
{
	return(serial_state.status);
}

/**
  * @brief  
  */
static void optical_rxbuff_set(uint16_t size, uint8_t *buffer)
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
static uint16_t optical_rxbuff_get(uint8_t **buffer)
{
	*buffer = serial_state.rx_buff;
	
	return(serial_state.rx_buff_size);
}

/**
  * @brief  
  */
static void optical_rxbuff_remove(void)
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
static void optical_txbuff_set(uint16_t size, uint8_t *buffer)
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
static uint16_t optical_txbuff_get(uint8_t **buffer)
{
	*buffer = serial_state.tx_buff;
	
	return(serial_state.tx_buff_size);
}

/**
  * @brief  
  */
static void optical_txbuff_remove(void)
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
static uint16_t optical_timeout_config(uint16_t msecond)
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
static uint16_t optical_timeout_read(void)
{
	return(serial_state.timeout_config);
}



/**
  * @brief  
  */
const struct __serial optical = 
{
    .control        = 
    {
        .name       = "optical",
        .status     = optical_status,
        .init       = optical_init,
        .suspend    = optical_suspend,
    },
    
	.runner			= optical_runner,
	.read			= optical_read,
	.write			= optical_write,
	.status			= optical_bus_status,
	
    .rxbuff			= 
    {
		.get		= optical_rxbuff_get,
		.set		= optical_rxbuff_set,
		.remove		= optical_rxbuff_remove,
    },
	
    .txbuff			= 
    {
		.get		= optical_txbuff_get,
		.set		= optical_txbuff_set,
		.remove		= optical_txbuff_remove,
    },
    
    .timeout		=  
    {
		.get		= optical_timeout_read,
		.set		= optical_timeout_config,
    },
    
    .uart			= &UART_USED,
};

