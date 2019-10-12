/**
 * @brief		
 * @details		低功耗下需要正常运行
 * @date		2016-11-15
 **/

/* Includes ------------------------------------------------------------------*/
#include "power.h"

#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
#include "meter.h"
#else

#if defined (DEMO_STM32F091)
#include "delay.h"
#include "stm32f0xx.h"
#endif

#endif


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#if defined (DEMO_STM32F091)
#define POWER_UP_VOL        (4900)
#define POWER_DOWN_VOL      (4700)
#endif


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static enum __power_status status_current = SUPPLY_BATTERY;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static enum __power_status power_check(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    if(is_powered())
    {
        return(SUPPLY_AC);
    }
    else
    {
        return(SUPPLY_BATTERY);
    }
#else

#if defined (DEMO_STM32F091)
    static enum __power_status status_before = SUPPLY_BATTERY;
    
    GPIO_InitTypeDef GPIO_InitStruct;
    ADC_InitTypeDef ADC_InitStruct;
    uint32_t Voltage[3];
    uint16_t Ref;
    uint16_t *Cali = (uint16_t *)0x1FFFF7BA;
    uint8_t cnt;
    
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    /* ADC1 Periph clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
    /* ADCs DeInit */  
    ADC_DeInit(ADC1);
    
    /* Select ADC clock source */
    ADC_ClockModeConfig(ADC1, ADC_ClockMode_SynClkDiv2);
    
    /* Initialize ADC structure */
    ADC_StructInit(&ADC_InitStruct);
    ADC_Init(ADC1, &ADC_InitStruct);
    ADC_VrefintCmd(ENABLE);
    ADC_ChannelConfig(ADC1, ADC_Channel_Vrefint, ADC_SampleTime_71_5Cycles);
    /* Enable the ADC peripheral */
    ADC_Cmd(ADC1, ENABLE);
    
    
    /* ADC1 regular Software Start Conv */
    ADC_StartOfConversion(ADC1);
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    /* Get ADC1 converted data */
    Voltage[0] = ADC_GetConversionValue(ADC1);
    if(Voltage[0])
    {
        Ref = (uint16_t)(((uint32_t)(*Cali) * 3300) / Voltage[0]);
        ADC_VrefintCmd(DISABLE);
    }
    else
    {
        ADC_VrefintCmd(DISABLE);
        ADC_DeInit(ADC1);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
        status_before = SUPPLY_BATTERY;
        return(status_before);
    }
	
    ADC1->CHSELR = 0;
	ADC_ChannelConfig(ADC1, ADC_Channel_12 , ADC_SampleTime_41_5Cycles);
    for(cnt=0; cnt<3; cnt++)
    {
        ADC_StartOfConversion(ADC1);
        while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
        Voltage[cnt] = ADC_GetConversionValue(ADC1);
        Voltage[cnt] = Voltage[cnt] * Ref / 4096;
        Voltage[cnt] *= 3;
    }
    
    if((status_before == SUPPLY_BATTERY) && \
        (Voltage[0] > POWER_UP_VOL) && \
        (Voltage[1] > POWER_UP_VOL) && \
        (Voltage[2] > POWER_UP_VOL))
    {
        mdelay(10);
        ADC_StartOfConversion(ADC1);
        while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
        Voltage[0] = ADC_GetConversionValue(ADC1);
        Voltage[0] = Voltage[0] * Ref / 4096;
        Voltage[0] *= 3;
        
        if(Voltage[0] > POWER_UP_VOL)
        {
            status_before = SUPPLY_AC;
        }
    }
    
    if((status_before == SUPPLY_AC) && \
        (Voltage[0] < POWER_DOWN_VOL) && \
        (Voltage[1] < POWER_DOWN_VOL) && \
        (Voltage[2] < POWER_DOWN_VOL))
    {
        mdelay(10);
        ADC_StartOfConversion(ADC1);
        while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
        Voltage[0] = ADC_GetConversionValue(ADC1);
        Voltage[0] = Voltage[0] * Ref / 4096;
        Voltage[0] *= 3;
        
        if(Voltage[0] < POWER_UP_VOL)
        {
            status_before = SUPPLY_BATTERY;
        }
    }
    
    ADC_DeInit(ADC1);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
    
    return(status_before);
#endif

#endif
}





/**
  * @brief  
  */
static void power_init(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	meter.control.init(DEVICE_NORMAL);
	mdelay(500);
	meter.control.suspend();
#else
	//初始化电源检测功能
#endif
}



/**
  * @brief  
  */
static enum __power_status power_status(void)
{
    enum __power_status status = power_check();
    
    if((status_current == POWER_WAKEUP) && (status == SUPPLY_BATTERY))
    {
        return(status_current);
    }
    else if((status_current == POWER_REBOOT) || (status_current == POWER_RESET))
    {
    	return(status_current);
    }
    else
    {
        status_current = status;
        
        return(status_current);
    }
}

/**
  * @brief  
  */
static enum __power_status power_alter(enum __power_status status)
{
    if(status == POWER_WAKEUP)
    {
        if(status_current == SUPPLY_BATTERY)
        {
            status_current = POWER_WAKEUP;
        }
    }
    else if(status == SUPPLY_BATTERY)
    {
        status_current = power_check();
    }
    else if(status == POWER_REBOOT)
    {
        status_current = POWER_REBOOT;
    }
    else if(status == POWER_RESET)
    {
        if((status_current == SUPPLY_AC) || (status_current == SUPPLY_DC) || (status_current == SUPPLY_AUX))
        {
            status_current = POWER_RESET;
        }
    }

    return(status_current);
}

/**
  * @brief  
  */
const struct __power power = 
{
    .init               = power_init,
    .status             = power_status,
    .alter              = power_alter,
};
