/**
 * @brief		
 * @details		
 * @date		2016-11-15
 **/

/* Includes ------------------------------------------------------------------*/
#include "bcd.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  һ�ֽ�BCDת����uint8_t
  */
uint8_t B2U8(uint8_t val)
{
    return(((val) & 0x0f) + ((val) >> 4) * 10);
}

/**
  * @brief  uint8_tת����BCD
  */
uint8_t U2B8(uint8_t val)
{
    return((((val) / 10) << 4) + (val) % 10);
}

/**
  * @brief  ���ֽ�BCDת����uint16_t
  */
uint16_t B2U16(uint16_t val)
{
    
}

/**
  * @brief  uint16_tת����BCD
  */
uint16_t U2B16(uint16_t val)
{
    
}

/**
  * @brief  ���ֽ�BCDת����uint32_t
  */
uint32_t B2U32(uint32_t val)
{
    
}

/**
  * @brief  uint32_tת����BCD
  */
uint32_t U2B32(uint32_t val)
{
    
}