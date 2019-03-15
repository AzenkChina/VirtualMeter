/**
 * @brief		
 * @details		
 * @date		2016-11-15
 **/

/* Includes ------------------------------------------------------------------*/
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux )

#include "viic.h"
#include "stm32f0xx.h"
#include "delay.h"


/* Private typedef -----------------------------------------------------------*/
enum __iic_res
{
    IIC_NAK = 0,
    IIC_ACK,
};

/* Private define ------------------------------------------------------------*/
#define VIIC_SDA_DIR_OUT            ;
#define VIIC_SDA_DIR_IN             ;
#define VIIC_SCL_L                  GPIO_ResetBits(GPIOB,GPIO_Pin_7)
#define VIIC_SCL_H					GPIO_SetBits(GPIOB,GPIO_Pin_7)
#define VIIC_SDA_L					GPIO_ResetBits(GPIOB,GPIO_Pin_6)
#define VIIC_SDA_H					GPIO_SetBits(GPIOB,GPIO_Pin_6)
#define VIIC_SDA_DATA               GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6)

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
    GPIO_InitTypeDef GPIO_InitStruct;
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_2;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    GPIO_SetBits(GPIOB, GPIO_Pin_6);
    GPIO_SetBits(GPIOB, GPIO_Pin_7);
    
    status = DEVICE_INIT;
}

/**
  * @brief  
  */
static void viic_suspend(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    GPIO_SetBits(GPIOB, GPIO_Pin_6);
    GPIO_SetBits(GPIOB, GPIO_Pin_7);

    status = DEVICE_SUSPENDED;
}


/**
  * @brief  
  */
static uint32_t viic_rate_get(void)
{
    return(100000);
}

/**
  * @brief  
  */
static uint32_t viic_rate_set(uint32_t rate)
{
    return(100000);
}



/**
  * @brief  start
  */
static void viic_low_start(void)
{
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
  * @brief   top
  */
static void viic_low_stop(void)
{
	VIIC_SDA_L;
	udelay(4);
	VIIC_SCL_H;
	udelay(4);
	VIIC_SDA_H;
	udelay(4);
}

/**
  * @brief   end
  */
static void viic_low_send(uint8_t ch)
{
	uint8_t i;
    
	for (i=0;i<8;i++)
	{
		if(ch & 0x80)
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
		ch = ch << 1;
	}
}

/**
  * @brief   et ack
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
	VIIC_SDA_DIR_OUT;
	udelay(4);
    
	return(res);
}

/**
  * @brief  set ack
  */
static void viic_low_ack_set(enum __iic_res res)
{
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









static uint16_t viic_raw_read(uint16_t count, uint8_t * buffer)
{
    
}

static uint16_t viic_raw_write(uint16_t count, const uint8_t *buffer)
{
    
}

static uint8_t viic_raw_getchar(void)
{
    
}

static uint8_t viic_raw_putchar(uint8_t ch)
{
    
}


static uint16_t viic_bus_read(uint16_t addr, uint16_t reg, uint16_t count, uint8_t * buffer)
{
    
}

static uint16_t viic_bus_write(uint16_t addr, uint16_t reg, uint16_t count, const uint8_t *buffer)
{
    
}

enum __bus_status viic_bus_status(void)
{
    
}

const struct __iic viic = 
{
    .control        = 
    {
        .name       = "viic",
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
