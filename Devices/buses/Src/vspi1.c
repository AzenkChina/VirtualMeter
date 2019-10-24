/**
 * @brief		
 * @details		
 * @date		2016-11-15
 **/

/* Includes ------------------------------------------------------------------*/
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux )

#include "vspi1.h"
#include "delay.h"

#if defined (DEMO_STM32F091)
#include "stm32f0xx.h"
#endif

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
#if defined (DEMO_STM32F091)
    GPIO_InitTypeDef GPIO_InitStruct;
    SPI_InitTypeDef SPI_InitStruct;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    
    SPI_I2S_DeInit(SPI1);
    SPI_StructInit(&SPI_InitStruct);
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
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
#endif

    status = DEVICE_INIT;
}

/**
  * @brief  
  */
static void spi1_suspend(void)
{
#if defined (DEMO_STM32F091)
    SPI_I2S_DeInit(SPI1);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, DISABLE);
#endif

    status = DEVICE_NOTINIT;
}


static uint8_t spi1_select(uint8_t cs)
{
#if defined (DEMO_STM32F091)
    GPIO_ResetBits(GPIOA, GPIO_Pin_8);
    udelay(300);
#endif

    return(0);
}

static uint8_t spi1_release(uint8_t cs)
{
#if defined (DEMO_STM32F091)
    udelay(300);
    GPIO_SetBits(GPIOA, GPIO_Pin_8);
#endif

    return(0);
}

static uint32_t spi1_freq_get(void)
{
    return(750*000);
}

static uint32_t spi1_freq_set(uint32_t rate)
{
    return(750*000);
}


/**
  * @brief  
  */
static uint32_t spi1_read(uint32_t count, uint8_t * buffer)
{
#if defined (DEMO_STM32F091)
    uint32_t i;
    
    if(status != DEVICE_INIT)
    {
        return(0);
    }
    
    for(i=0; i<count; i++)
    {
        /* Loop while DR register in not emplty */
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
        /* Send byte through the SPI1 peripheral */
        SPI_SendData8(SPI1, 0xff);
        /* Wait to receive a byte */
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
        /* Return the byte read from the SPI bus */
        buffer[i] = SPI_ReceiveData8(SPI1);
    }
    
    return(count);
#endif
}

/**
  * @brief  
  */
static uint32_t spi1_write(uint32_t count, const uint8_t *buffer)
{
#if defined (DEMO_STM32F091)
    uint32_t i;
    
    if(status != DEVICE_INIT)
    {
        return(0);
    }
    
    for(i=0; i<count; i++)
    {
        /* Loop while DR register in not emplty */
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
        /* Send byte through the SPI1 peripheral */
        SPI_SendData8(SPI1, buffer[i]);
        /* Wait to receive a byte */
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
        /* Return the byte read from the SPI bus */
        SPI_ReceiveData8(SPI1);
    }
    
    return(count);
#endif
}

/**
  * @brief  
  */
static uint32_t spi1_readwrite(uint32_t count, const uint8_t *wbuffer, uint8_t * rbuffer)
{
#if defined (DEMO_STM32F091)
    uint32_t i;
    
    if(status != DEVICE_INIT)
    {
        return(0);
    }
    
    for(i=0; i<count; i++)
    {
        /* Loop while DR register in not emplty */
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
        /* Send byte through the SPI1 peripheral */
        SPI_SendData8(SPI1, wbuffer[i]);
        /* Wait to receive a byte */
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
        /* Return the byte read from the SPI bus */
        rbuffer[i] = SPI_ReceiveData8(SPI1);
    }
    
    return(count);
#endif
}

/**
  * @brief  
  */
static uint32_t spi1_readchar(void)
{
#if defined (DEMO_STM32F091)
    if(status != DEVICE_INIT)
    {
        return(0);
    }
    
    /* Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    /* Send byte through the SPI1 peripheral */
    SPI_SendData8(SPI1, 0xff);
    /* Wait to receive a byte */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    /* Return the byte read from the SPI bus */
    return(SPI_ReceiveData8(SPI1));
#endif
}

/**
  * @brief  
  */
static uint32_t spi1_writechar(uint32_t ch)
{
#if defined (DEMO_STM32F091)
    if(status != DEVICE_INIT)
    {
        return(0);
    }
    
    /* Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    /* Send byte through the SPI1 peripheral */
    SPI_SendData8(SPI1, ch);
    /* Wait to receive a byte */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    /* Return the byte read from the SPI bus */
    SPI_ReceiveData8(SPI1);
    return(ch);
#endif
}

/**
  * @brief  
  */
static uint32_t spi1_readwritechar(uint32_t ch)
{
#if defined (DEMO_STM32F091)
    if(status != DEVICE_INIT)
    {
        return(0);
    }
    
    /* Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    /* Send byte through the SPI1 peripheral */
    SPI_SendData8(SPI1, ch);
    /* Wait to receive a byte */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    /* Return the byte read from the SPI bus */
    return(SPI_ReceiveData8(SPI1));
#endif
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