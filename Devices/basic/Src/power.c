/**
 * @brief		
 * @details		�͹�������Ҫ��������
 * @date		2016-11-15
 **/

/* Includes ------------------------------------------------------------------*/
#include "power.h"

#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
#include "meter.h"
#else

#if defined (STM32F091)
#include "stm32f0xx.h"
#endif

#endif


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#if defined (STM32F091)
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

#if defined (STM32F091)
    static enum __power_status status_before = SUPPLY_BATTERY;
    
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    int64_t Voltage;
    int64_t Ref;
    uint16_t *Cali = (uint16_t *)0x1FFFF7BA;
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    /* ADC1 Periph clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
    /* ADCs DeInit */  
    ADC_DeInit(ADC1);
    /* Initialize ADC structure */
    ADC_StructInit(&ADC_InitStructure);
    /* Configure the ADC1 in continuous mode with a resolution equal to 12 bits  */
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_VrefintCmd(ENABLE);
    ADC_ChannelConfig(ADC1, ADC_Channel_Vrefint , ADC_SampleTime_239_5Cycles);
    /* ADC Calibration */
    ADC_GetCalibrationFactor(ADC1);
    /* Enable the ADC peripheral */
    ADC_Cmd(ADC1, ENABLE);
    /* ADC1 regular Software Start Conv */ 
    ADC_StartOfConversion(ADC1);
    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    /* Get ADC1 converted data */
    Voltage = ADC_GetConversionValue(ADC1);
    if(Voltage)
    {
        Ref = ((int64_t)*Cali) * 3300 / Voltage;
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
	ADC_StartOfConversion(ADC1);
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	Voltage = ADC_GetConversionValue(ADC1);
	Voltage = Voltage * Ref / 4096;
	Voltage *= 3;
    
    ADC_DeInit(ADC1);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
    
    if((status_before == SUPPLY_BATTERY) && (Voltage > POWER_UP_VOL))
    {
        status_before = SUPPLY_AC;
    }
    
    if((status_before == SUPPLY_AC) && (Voltage < POWER_DOWN_VOL))
    {
        status_before = SUPPLY_BATTERY;
    }
    
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
	//��ʼ����Դ��⹦��
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
