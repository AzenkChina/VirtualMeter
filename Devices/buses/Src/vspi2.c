/**
 * @brief		
 * @details		
 * @date		2016-11-15
 **/

/* Includes ------------------------------------------------------------------*/
#if !defined ( _WIN32 ) && !defined ( _WIN64 ) && !defined ( __linux )

#include "vspi2.h"
#include "delay.h"

#if defined (BUILD_REAL_WORLD)
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
static enum __dev_status spi2_status(void)
{
    return(status);
}

/**
  * @brief  
  */
static void spi2_init(enum __dev_state state)
{
#if defined (BUILD_REAL_WORLD)
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
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
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
#endif
    
    status = DEVICE_INIT;
}

/**
  * @brief  
  */
static void spi2_suspend(void)
{
#if defined (BUILD_REAL_WORLD)
    SPI_I2S_DeInit(SPI2);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, DISABLE);
#endif
    
    status = DEVICE_SUSPENDED;
}


static uint8_t spi2_select(uint8_t cs)
{
#if defined (BUILD_REAL_WORLD)
    GPIO_ResetBits(GPIOB, GPIO_Pin_12);
    udelay(300);
#endif
    
    return(0);
}

static uint8_t spi2_release(uint8_t cs)
{
#if defined (BUILD_REAL_WORLD)
    udelay(300);
    GPIO_SetBits(GPIOB, GPIO_Pin_12);
#endif
    
    return(0);
}

static uint32_t spi2_freq_get(void)
{
    return(3*1000*1000);
}

static uint32_t spi2_freq_set(uint32_t rate)
{
    return(3*1000*1000);
}


/**
  * @brief  
  */
static uint32_t spi2_read(uint32_t count, uint8_t * buffer)
{
#if defined (BUILD_REAL_WORLD)
    uint32_t i;
    
    if(status != DEVICE_INIT)
    {
        return(0);
    }
    
    for(i=0; i<count; i++)
    {
        /* Loop while DR register in not emplty */
        while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
        /* Send byte through the SPI2 peripheral */
        SPI_SendData8(SPI2, 0xff);
        /* Wait to receive a byte */
        while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
        /* Return the byte read from the SPI bus */
        buffer[i] = SPI_ReceiveData8(SPI2);
    }
    
    return(count);
#endif
}

/**
  * @brief  
  */
static uint32_t spi2_write(uint32_t count, const uint8_t *buffer)
{
#if defined (BUILD_REAL_WORLD)
    uint32_t i;
    
    if(status != DEVICE_INIT)
    {
        return(0);
    }
    
    for(i=0; i<count; i++)
    {
        /* Loop while DR register in not emplty */
        while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
        /* Send byte through the SPI2 peripheral */
        SPI_SendData8(SPI2, buffer[i]);
        /* Wait to receive a byte */
        while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
        /* Return the byte read from the SPI bus */
        SPI_ReceiveData8(SPI2);
    }
    
    return(count);
#endif
}

/**
  * @brief  
  */
static uint32_t spi2_readwrite(uint32_t count, const uint8_t *wbuffer, uint8_t * rbuffer)
{
#if defined (BUILD_REAL_WORLD)
    uint32_t i;
    
    if(status != DEVICE_INIT)
    {
        return(0);
    }
    
    for(i=0; i<count; i++)
    {
        /* Loop while DR register in not emplty */
        while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
        /* Send byte through the SPI2 peripheral */
        SPI_SendData8(SPI2, wbuffer[i]);
        /* Wait to receive a byte */
        while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
        /* Return the byte read from the SPI bus */
        rbuffer[i] = SPI_ReceiveData8(SPI2);
    }
    
    return(count);
#endif
}

/**
  * @brief  
  */
static uint32_t spi2_readchar(void)
{
#if defined (BUILD_REAL_WORLD)
    if(status != DEVICE_INIT)
    {
        return(0);
    }
    
    /* Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
    /* Send byte through the SPI2 peripheral */
    SPI_SendData8(SPI2, 0xff);
    /* Wait to receive a byte */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
    /* Return the byte read from the SPI bus */
    return(SPI_ReceiveData8(SPI2));
#endif
}

/**
  * @brief  
  */
static uint32_t spi2_writechar(uint32_t ch)
{
#if defined (BUILD_REAL_WORLD)
    if(status != DEVICE_INIT)
    {
        return(0);
    }
    
    /* Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
    /* Send byte through the SPI2 peripheral */
    SPI_SendData8(SPI2, ch);
    /* Wait to receive a byte */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
    /* Return the byte read from the SPI bus */
    SPI_ReceiveData8(SPI2);
    return(ch);
#endif
}

/**
  * @brief  
  */
static uint32_t spi2_readwritechar(uint32_t ch)
{
#if defined (BUILD_REAL_WORLD)
    if(status != DEVICE_INIT)
    {
        return(0);
    }
    
    /* Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
    /* Send byte through the SPI2 peripheral */
    SPI_SendData8(SPI2, ch);
    /* Wait to receive a byte */
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
    /* Return the byte read from the SPI bus */
    return(SPI_ReceiveData8(SPI2));
#endif
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
const struct __spi vspi2 = 
{
    .control            = 
    {
        .name           = "virtual spi 2",
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
