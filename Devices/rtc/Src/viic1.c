/**
 * @brief		
 * @details		
 * @date		2016-11-15
 **/

/* Includes ------------------------------------------------------------------*/
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux )

#include "viic1.h"
#include "delay.h"

#if defined (DEMO_STM32F091)
#include "stm32f0xx.h"
#endif

/* Private typedef -----------------------------------------------------------*/
enum __iic_res
{
    IIC_NAK = 0,
    IIC_ACK,
};

/* Private define ------------------------------------------------------------*/
#if defined (DEMO_STM32F091)
#define VIIC_SCL_L                  GPIO_ResetBits(GPIOB,GPIO_Pin_8)
#define VIIC_SCL_H					GPIO_SetBits(GPIOB,GPIO_Pin_8)
#define VIIC_SDA_L					GPIO_ResetBits(GPIOB,GPIO_Pin_9)
#define VIIC_SDA_H					GPIO_SetBits(GPIOB,GPIO_Pin_9)
#define VIIC_SDA_DATA               (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9) == Bit_SET)
#define VIIC_SDA_DIR_OUT            
#define VIIC_SDA_DIR_IN             GPIO_SetBits(GPIOB,GPIO_Pin_9)
#endif

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static enum __dev_status status = DEVICE_NOTINIT;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  
  */
static enum __dev_status viic_status(void)
{
    return(status);
}

/**
  * @brief  
  */
static void viic_init(enum __dev_state state)
{
#if defined (DEMO_STM32F091)
    GPIO_InitTypeDef GPIO_InitStruct;
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    GPIO_SetBits(GPIOB, GPIO_Pin_8);
    GPIO_SetBits(GPIOB, GPIO_Pin_9);
#endif
    
    status = DEVICE_INIT;
}

/**
  * @brief  
  */
static void viic_suspend(void)
{
    status = DEVICE_SUSPENDED;
}


/**
  * @brief  
  */
static uint32_t viic_rate_get(void)
{
    return(85*1000);
}

/**
  * @brief  
  */
static uint32_t viic_rate_set(uint32_t rate)
{
    return(85*1000);
}



/**
  * @brief  start
  */
static void viic_low_start(void)
{
    VIIC_SDA_DIR_OUT;
	VIIC_SDA_H;
	VIIC_SCL_H;
    
	udelay(4);
	VIIC_SDA_H;
	udelay(4);
	VIIC_SDA_L;
	udelay(4);
	VIIC_SCL_L;
	udelay(4);
}

/**
  * @brief   stop
  */
static void viic_low_stop(void)
{
    VIIC_SDA_DIR_OUT;
	VIIC_SDA_L;
	udelay(4);
	VIIC_SCL_H;
	udelay(4);
	VIIC_SDA_H;
	udelay(4);
}

/**
  * @brief   get ack
  */
static enum __iic_res viic_low_ack_get(void)
{
	enum __iic_res res = IIC_ACK;
	uint8_t i = 5;
    
	VIIC_SDA_DIR_IN;
    
	udelay(4);
	VIIC_SCL_H;
    
	udelay(4);
    
    while((i--) && VIIC_SDA_DATA);
    
	if (i == 0)
	{
		res = IIC_NAK;
	}
    
	VIIC_SCL_L;
	udelay(4);
    
	return(res);
}

/**
  * @brief  set ack
  */
static void viic_low_ack_set(enum __iic_res res)
{
    VIIC_SDA_DIR_OUT;
    
    if(res == IIC_ACK)
    {
        VIIC_SDA_L;
        udelay(4);
        VIIC_SCL_H;
        udelay(4);
        VIIC_SCL_L;
        udelay(4);
        VIIC_SDA_H;
        udelay(4);
    }
    else if(res == IIC_NAK)
    {
        VIIC_SDA_H;
        udelay(4);
        VIIC_SCL_H;
        udelay(4);
        VIIC_SCL_L;
        udelay(4);
        VIIC_SDA_L;
        udelay(4);
    }
}


static uint8_t viic_raw_getchar(void)
{
	uint8_t loop;
    uint8_t val = 0;
    
	VIIC_SDA_DIR_IN;
    
	udelay(2);
    
	for(loop=0; loop<8; loop++)
	{
		VIIC_SCL_H;
		udelay(2);
        val = val << 1;
        
		if(VIIC_SDA_DATA)
		{
			val |= 0x01;
		}
		else
		{
			val &= 0xFE;
		}
        
		udelay(2);
		VIIC_SCL_L;
		udelay(4);
	}
    
	return val;
}

static uint8_t viic_raw_putchar(uint8_t ch)
{
	uint8_t loop;
    uint8_t val = ch;
    
    VIIC_SDA_DIR_OUT;
    
	for(loop=0; loop<8; loop++)
	{
		if(val & 0x80)
		{
			VIIC_SDA_H;
		}
		else
		{
			VIIC_SDA_L;
		}
        
		udelay(2);
		VIIC_SCL_H;
		udelay(4);
		VIIC_SCL_L;
		udelay(2);
		val = val << 1;
	}
    
    return(ch);
}

static uint16_t viic_raw_read(uint16_t count, uint8_t * buffer)
{
    uint16_t loop;
    uint8_t bits;
    
    if(!count || !buffer)
    {
        return(0);
    }
    
	VIIC_SDA_DIR_IN;
    
	udelay(2);
    
    for(loop=0; loop<count; loop++)
    {
        for(bits=0; bits<8; bits++)
        {
            VIIC_SCL_H;
            udelay(2);
            buffer[loop] = buffer[loop] << 1;
            
            if(VIIC_SDA_DATA)
            {
                buffer[loop] |= 0x01;
            }
            else
            {
                buffer[loop] &= 0xFE;
            }
            
            udelay(2);
            VIIC_SCL_L;
            udelay(4);
        }
    }
    
    return(count);
}

static uint16_t viic_raw_write(uint16_t count, const uint8_t *buffer)
{
    uint16_t loop;
    uint8_t bits;
    uint8_t val;
    
    if(!count || !buffer)
    {
        return(0);
    }
    
    VIIC_SDA_DIR_OUT;
    
    for(loop=0; loop<count; loop++)
    {
        val = buffer[loop];
        
        for(bits=0; bits<8; bits++)
        {
            if(val & 0x80)
            {
                VIIC_SDA_H;
            }
            else
            {
                VIIC_SDA_L;
            }
            
            udelay(2);
            VIIC_SCL_H;
            udelay(4);
            VIIC_SCL_L;
            udelay(2);
            val = val << 1;
        }
    }
    
    return(count);
}

static uint16_t viic_bus_read(uint16_t addr, uint32_t reg, uint8_t reglen, uint16_t count, uint8_t * buffer)
{
    uint16_t loop;
    
    if(!count || !buffer)
    {
        return(0);
    }
    
    if(reglen > 4)
    {
        return(0);
    }
    
    viic_low_start();
    
    viic_raw_putchar((uint8_t)(addr << 1));
    if(viic_low_ack_get() != IIC_ACK)
    {
        return(0);
    }
    
    while(reglen)
    {
        viic_raw_putchar((uint8_t)((reg >> ((reglen - 1) * 8)) & 0xff));
        if(viic_low_ack_get() != IIC_ACK)
        {
            return(0);
        }
        
        reglen -= 1;
    }
    
    viic_low_start();
    
    viic_raw_putchar((uint8_t)((addr << 1) | 0x01));
    if(viic_low_ack_get() != IIC_ACK)
    {
        return(0);
    }
    
    for(loop=0; loop<count; loop++)
    {
        buffer[loop] = viic_raw_getchar();
        
        if(loop < (count-1))
        {
            viic_low_ack_set(IIC_ACK);
        }
    }
    
    viic_low_ack_set(IIC_NAK);
    viic_low_stop();
    
    return(count);
}

static uint16_t viic_bus_write(uint16_t addr, uint32_t reg, uint8_t reglen, uint16_t count, const uint8_t *buffer)
{
    uint16_t loop;
    
    if(count && !buffer)
    {
        return(0);
    }
    
    if(reglen > 4)
    {
        return(0);
    }
    
    viic_low_start();
    udelay(4);
    
    viic_raw_putchar((uint8_t)(addr << 1));
    if(viic_low_ack_get() != IIC_ACK)
    {
        return(0);
    }
    
    while(reglen)
    {
        viic_raw_putchar((uint8_t)((reg >> ((reglen - 1) * 8)) & 0xff));
        if(viic_low_ack_get() != IIC_ACK)
        {
            return(0);
        }
        
        reglen -= 1;
    }
    
    for(loop=0; loop<count; loop++)
    {
        viic_raw_putchar(buffer[loop]);
        
        if(viic_low_ack_get() != IIC_ACK)
        {
            viic_low_stop();
            return(loop);
        }
    }
    
    viic_low_stop();
    
    return(count);
}

static enum __bus_status viic_bus_status(void)
{
    return(BUS_IDLE);
}

const struct __iic viic1 = 
{
    .control        = 
    {
        .name       = "virtual iic 1",
        .status     = viic_status,
        .init       = viic_init,
        .suspend    = viic_suspend,
    },
    
    .octet          = 
    {
        .read       = viic_raw_getchar,
        .write      = viic_raw_putchar,
    },
    
    .sequent        = 
    {
        .read       = viic_raw_read,
        .write      = viic_raw_write,
    },
    
    .bus            = 
    {
        .read       = viic_bus_read,
        .write      = viic_bus_write,
    },
    
    .status         = viic_bus_status,
    
    .speed          = 
    {
        .get        = viic_rate_get,
        .set        = viic_rate_set,
    },
};

#endif /* !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux ) */
