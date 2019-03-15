/**
 * @brief		
 * @details		
 * @date		2016-11-15
 **/

/* Includes ------------------------------------------------------------------*/
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux )

#include "spi2.h"
#include "stm32f0xx.h"
#include "delay.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static enum __dev_status status = DEVICE_NOTINIT;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  
  */
static enum __dev_status spi2_status(void)
{
    return(status);
}

/**
  * @brief  
  */
static void spi2_init(enum __dev_state state)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    SPI_InitTypeDef SPI_InitStruct;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    
    SPI_I2S_DeInit(SPI2);
    SPI_StructInit(&SPI_InitStruct);
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitStruct);
    SPI_RxFIFOThresholdConfig(SPI2, SPI_RxFIFOThreshold_QF);
    
    SPI_Cmd(SPI2, ENABLE);
    
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_SetBits(GPIOB, GPIO_Pin_12);
    
    GPIO_PinAFConfig(GPIOB , GPIO_PinSource13, GPIO_AF_0);
    GPIO_PinAFConfig(GPIOB , GPIO_PinSource14, GPIO_AF_0);
    GPIO_PinAFConfig(GPIOB , GPIO_PinSource15, GPIO_AF_0);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    status = DEVICE_INIT;
}

/**
  * @brief  
  */
static void spi2_suspend(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    SPI_I2S_DeInit(SPI2);
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    GPIO_SetBits(GPIOB, GPIO_Pin_12);
    GPIO_SetBits(GPIOB, GPIO_Pin_13);
    GPIO_SetBits(GPIOB, GPIO_Pin_14);
    GPIO_SetBits(GPIOB, GPIO_Pin_15);
    
    status = DEVICE_SUSPENDED;
}


static uint8_t spi2_select(uint8_t cs)
{
    GPIO_ResetBits(GPIOB, GPIO_Pin_12);
    udelay(300);
    
    return(0);
}

static uint8_t spi2_release(uint8_t cs)
{
    udelay(300);
    GPIO_SetBits(GPIOB, GPIO_Pin_12);
    return(0);
}

static uint32_t spi2_freq_get(void)
{
    return(1000000);
}

static uint32_t spi2_freq_set(uint32_t rate)
{
    return(1000000);
}


/**
  * @brief  
  */
static uint32_t spi2_read(uint32_t count, uint8_t * buffer)
{
    uint32_t i;
    
    for(i=0; i<count; i++)
    {
        buffer[i] = SPI_I2S_Send_ReceiveData(SPI2, 0xff);
    }
    
    return(count);
}

/**
  * @brief  
  */
static uint32_t spi2_write(uint32_t count, const uint8_t *buffer)
{
    uint32_t i;
    
    for(i=0; i<count; i++)
    {
        SPI_I2S_Send_ReceiveData(SPI2, buffer[i]);
    }
    
    return(count);
}

/**
  * @brief  
  */
static uint32_t spi2_readwrite(uint32_t count, const uint8_t *wbuffer, uint8_t * rbuffer)
{
    uint32_t i;
    
    for(i=0; i<count; i++)
    {
        rbuffer[i] = SPI_I2S_Send_ReceiveData(SPI2, wbuffer[i]);
    }
    
    return(count);
}

/**
  * @brief  
  */
static uint32_t spi2_readchar(void)
{
    uint8_t val;
    
    val = SPI_I2S_Send_ReceiveData(SPI2, 0xff);
    
    return((uint32_t)val);
}

/**
  * @brief  
  */
static uint32_t spi2_writechar(uint32_t ch)
{
    SPI_I2S_Send_ReceiveData(SPI2, (uint8_t)ch);
    
    return(ch);
}

/**
  * @brief  
  */
static uint32_t spi2_readwritechar(uint32_t ch)
{
    uint8_t val;
    
    val = (uint8_t)SPI_I2S_Send_ReceiveData(SPI2, ch);
    
    return((uint32_t)val);
}

/**
  * @brief  
  */
static enum __bus_status spi2_bus_status(void)
{
    return(BUS_IDLE);
}



/**
  * @brief  
  */
const struct __spi spi2 = 
{
    .control            = 
    {
        .name           = "spi2",
        .status         = spi2_status,
        .init           = spi2_init,
        .suspend        = spi2_suspend,
    },
    
    .read               = spi2_read,
    .write              = spi2_write,
    .exchange           = spi2_readwrite,
    .select             = spi2_select,
    .release            = spi2_release,
    .status             = spi2_bus_status,
    
    .octet              = 
    {
        .read           = spi2_readchar,
        .write          = spi2_writechar,
        .exchange       = spi2_readwritechar,
    },
    
    .speed              = 
    {
        .get            = spi2_freq_get,
        .set            = spi2_freq_set,
    },
};

#endif /* !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux ) */
