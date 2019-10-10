/**
 * @brief		
 * @details		
 * @date		2016-11-15
 **/

/* Includes ------------------------------------------------------------------*/
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux )

#include "vspi1.h"
#include "stm32f0xx.h"

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
static void cs_delay(void)
{
}

/**
  * @brief  
  */
static enum __dev_status spi1_status(void)
{
    //...获取当前设备状态
    return(status);
}

/**
  * @brief  
  */
static void spi1_init(enum __dev_state state)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    SPI_InitTypeDef SPI_InitStruct;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    
    SPI_I2S_DeInit(SPI1);
    SPI_StructInit(&SPI_InitStruct);
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStruct);
    SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);
    
    SPI_Cmd(SPI1, ENABLE);
    
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_3;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_SetBits(GPIOA, GPIO_Pin_8);
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
    
    GPIO_PinAFConfig(GPIOB , GPIO_PinSource3, GPIO_AF_0);
    GPIO_PinAFConfig(GPIOB , GPIO_PinSource4, GPIO_AF_0);
    GPIO_PinAFConfig(GPIOB , GPIO_PinSource5, GPIO_AF_0);
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
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
static void spi1_suspend(void)
{
    //...IO，寄存器，时钟等关闭
    GPIO_InitTypeDef GPIO_InitStruct;
    
    SPI_I2S_DeInit(SPI1);
    
    
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    
    GPIO_Init(GPIOB, &GPIO_InitStruct);
    
    GPIO_SetBits(GPIOB, GPIO_Pin_3);
    GPIO_SetBits(GPIOB, GPIO_Pin_4);
    GPIO_SetBits(GPIOB, GPIO_Pin_5);
    
    
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    GPIO_SetBits(GPIOA, GPIO_Pin_8);
    
    status = DEVICE_NOTINIT;
}


static uint8_t spi1_select(uint8_t cs)
{
    GPIO_ResetBits(GPIOA, GPIO_Pin_8);
    cs_delay();
    
    return(0);
}

static uint8_t spi1_release(uint8_t cs)
{
    cs_delay();
    GPIO_SetBits(GPIOA, GPIO_Pin_8);

    return(0);
}

static uint32_t spi1_freq_get(void)
{
    return(1000000);
}

static uint32_t spi1_freq_set(uint32_t rate)
{
    return(1000000);
}


/**
  * @brief  
  */
static uint32_t spi1_read(uint32_t count, uint8_t * buffer)
{
    uint32_t i;
    
    for(i=0; i<count; i++)
    {
        buffer[i] = SPI_I2S_Send_ReceiveData(SPI1, 0xff);
    }
    
    return(count);
}

/**
  * @brief  
  */
static uint32_t spi1_write(uint32_t count, const uint8_t *buffer)
{
    uint32_t i;
    
    for(i=0; i<count; i++)
    {
        SPI_I2S_Send_ReceiveData(SPI1, buffer[i]);
    }
    
    return(count);
}

/**
  * @brief  
  */
static uint32_t spi1_readwrite(uint32_t count, const uint8_t *wbuffer, uint8_t * rbuffer)
{
    uint32_t i;
    
    for(i=0; i<count; i++)
    {
        rbuffer[i] = SPI_I2S_Send_ReceiveData(SPI1, wbuffer[i]);
    }
    
    return(count);
}

/**
  * @brief  
  */
static uint32_t spi1_readchar(void)
{
    uint8_t val;
    
    val = SPI_I2S_Send_ReceiveData(SPI1, 0xff);
    
    return((uint32_t)val);
}

/**
  * @brief  
  */
static uint32_t spi1_writechar(uint32_t ch)
{
    SPI_I2S_Send_ReceiveData(SPI1, (uint8_t)ch);
    
    return(ch);
}

/**
  * @brief  
  */
static uint32_t spi1_readwritechar(uint32_t ch)
{
    uint8_t val;
    
    val = SPI_I2S_Send_ReceiveData(SPI1, (uint8_t)ch);
    
    return((uint32_t)val);
}

/**
  * @brief  
  */
static enum __bus_status spi1_bus_status(void)
{
    return(BUS_IDLE);
}



/**
  * @brief  
  */
const struct __spi vspi1 = 
{
    .control            = 
    {
        .name           = "virtual spi 1",
        .status         = spi1_status,
        .init           = spi1_init,
        .suspend        = spi1_suspend,
    },
    
    .read               = spi1_read,
    .write              = spi1_write,
    .exchange           = spi1_readwrite,
    .select             = spi1_select,
    .release            = spi1_release,
    .status             = spi1_bus_status,
    
    .octet              = 
    {
        .read           = spi1_readchar,
        .write          = spi1_writechar,
        .exchange       = spi1_readwritechar,
    },
    
    .speed              = 
    {
        .get            = spi1_freq_get,
        .set            = spi1_freq_set,
    },
};

#endif /* !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux ) */