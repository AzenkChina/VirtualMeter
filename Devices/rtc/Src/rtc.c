/**
 * @brief		
 * @details		
 * @date		2016-08-16
 **/

/* Includes ------------------------------------------------------------------*/
#include "rtc.h"
#include <time.h>

#if defined (DEMO_STM32F091)
#include <stdbool.h>
#include <string.h>
#include "stm32f0xx.h"
#include "bcd.h"
#include "viic1.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#if defined (DEMO_STM32F091)
#define deviic      viic1
#endif

/* Private macro -------------------------------------------------------------*/
#if defined (DEMO_STM32F091)
#define RTC_ADDR                0x32

#define RX8025_REG_SEC          0x00
#define RX8025_REG_MIN          0x01
#define RX8025_REG_HOUR         0x02
#define RX8025_REG_WDAY         0x03
#define RX8025_REG_MDAY         0x04
#define RX8025_REG_MONTH        0x05
#define RX8025_REG_YEAR         0x06

#define RX8025_REG_CTRL1        0x0e
#define RX8025_REG_CTRL2        0x0f

#define RX8025_BIT_CTRL1_VDET   (1 << 0)
#define RX8025_BIT_CTRL1_VLF    (1 << 1)

#define RX8025_BIT_CTRL2_RST    (1 << 0)
#endif

/* Private variables ---------------------------------------------------------*/
static enum __dev_status status = DEVICE_NOTINIT;
#if defined (DEMO_STM32F091)
static const unsigned char rtc_days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
#endif

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
#if defined (DEMO_STM32F091)
static bool is_leap_year(uint16_t year)
{
	return (!(year % 4) && (year % 100)) || !(year % 400);
}

/*
 * The number of days in the month.
 */
static uint8_t rtc_month_days(uint16_t month, uint16_t year)
{
	return rtc_days_in_month[month] + (is_leap_year(year) && month == 1);
}
#endif

/**
  * @brief  
  */
static enum __dev_status rtc_status(void)
{
    return(status);
}

/**
  * @brief  
  */
static void rtc_init(enum __dev_state state)
{
#if defined (DEMO_STM32F091)
    deviic.control.init(state);
#endif
    status = DEVICE_INIT;
}

/**
  * @brief  
  */
static void rtc_suspend(void)
{
#if defined (DEMO_STM32F091)
    deviic.control.suspend();
#endif
    status = DEVICE_NOTINIT;
}

/**
  * @brief  
  */
static uint8_t rtc_config_read(uint8_t addr, uint8_t count, uint8_t *param)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	return(0);
#else
    
#if defined (DEMO_STM32F091)
	return(0);
#endif
    
#endif
}


/**
  * @brief  
  */
static uint8_t rtc_config_write(uint8_t addr, uint8_t count, const uint8_t *param)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	return(0);
#else
    
#if defined (DEMO_STM32F091)
	return(0);
#endif
    
#endif
}

/**
  * @brief  
  */
static uint64_t rtc_read(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	time_t stamp;
	time(&stamp);
	
	return((uint64_t)stamp);
#else
    
#if defined (DEMO_STM32F091)
    struct tm tim;
    uint8_t date[7];
    
    if(status == DEVICE_INIT)
    {
        memset(date, 0, sizeof(date));
        memset(&tim, 0, sizeof(tim));
        
        if(deviic.bus.read(RTC_ADDR, RX8025_REG_SEC, 1, 7, date) != 7)
        {
            return(0);
        }
        
        tim.tm_sec = B2U8(date[RX8025_REG_SEC] & 0x7f);
        tim.tm_min = B2U8(date[RX8025_REG_MIN] & 0x7f);
        tim.tm_hour = B2U8(date[RX8025_REG_HOUR] & 0x3f);
        tim.tm_mday = B2U8(date[RX8025_REG_MDAY] & 0x3f);
        tim.tm_mon = B2U8(date[RX8025_REG_MONTH] & 0x1f) - 1;
        tim.tm_year = B2U8(date[RX8025_REG_YEAR]) + 100;
        
        if(tim.tm_year < 70 || \
            ((unsigned)tim.tm_mon) >= 12 || \
            tim.tm_mday < 1 || \
            tim.tm_mday > rtc_month_days(tim.tm_mon, tim.tm_year + 1900) || \
            ((unsigned)tim.tm_hour) >= 24 || \
            ((unsigned)tim.tm_min) >= 60 || \
            ((unsigned)tim.tm_sec) >= 60)
        {
            return(0);
        }
        
        return(mktime(&tim));
    }
    else
    {
        return(0);
    }
#endif
    
#endif
}


/**
  * @brief  
  */
static uint64_t rtc_write(uint64_t stamp)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	return(0);
#else
    
#if defined (DEMO_STM32F091)
    time_t t = stamp;
    struct tm *tim;
    uint8_t date[7];
    
    if(status == DEVICE_INIT)
    {
        tim = gmtime(&t);
        
        date[RX8025_REG_SEC] = U2B8(tim->tm_sec);
        date[RX8025_REG_MIN] = U2B8(tim->tm_min);
        date[RX8025_REG_HOUR] = U2B8(tim->tm_hour);
        date[RX8025_REG_WDAY] = (1 << U2B8(tim->tm_wday));
        date[RX8025_REG_MDAY] = U2B8(tim->tm_mday);
        date[RX8025_REG_MONTH] = U2B8(tim->tm_mon + 1);
        date[RX8025_REG_YEAR] = U2B8((tim->tm_year > 100)? (tim->tm_year - 100):0);
        
        deviic.bus.write(RTC_ADDR, RX8025_REG_SEC, 1, 7, date);
        
        return(t);
    }
    else
    {
        return(0);
    }
#endif
    
#endif
}


const struct __rtc rtc = 
{
    .control        = 
    {
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
        .name       = "virtual rtc",
#else

#if defined (DEMO_STM32F091)
        .name       = "rx8025t",
#endif

#endif
        .status     = rtc_status,
        .init       = rtc_init,
        .suspend    = rtc_suspend,
    },
	
	.read			= rtc_read,
	.write			= rtc_write,
    
    .config			= 
    {
        .read		= rtc_config_read,
        .write		= rtc_config_write,
    },
};
