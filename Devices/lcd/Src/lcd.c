/**
 * @brief		
 * @details		
 * @date		2016-08-16
 **/

/* Includes ------------------------------------------------------------------*/
#include "lcd.h"

#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
#include "comm_socket.h"
#else

#if defined (DEMO_STM32F091)
#include "string.h"
#include "stm32f0xx.h"
#include "viic3.h"
#include "delay.h"
#endif

#endif

/* Private define ------------------------------------------------------------*/
#if defined (DEMO_STM32F091)
#define deviic      viic3
#define GDRAM_SIZE  35
#endif

/* Private macro -------------------------------------------------------------*/
#if defined (DEMO_STM32F091)
#define LCD_ADDR                0x3E

#define LCD_SOFTRST				0xf7 //显示开启状态下软件复位
#define LCD_MODSET_ON			0xf5 //显示开启状态下的模式设置
#define LCD_MODSET_OFF			0xf4 //显示关闭状态下的模式设置
#define LCD_DISCTL				0xec //显示控制命令字
#define LCD_EVRSET				0xc0 //电子可调电阻设置寄存器值设置
#define LCD_ADSET				0x00 //缓冲区地址设置命令字
#define LCD_INIT				((LCD_SOFTRST << 24)|(LCD_DISCTL<<16)|((LCD_EVRSET|0xc0)<<8)|LCD_ADSET) //初始化
#endif

/* Private typedef -----------------------------------------------------------*/
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
struct __win_lcd_message
{
    struct
    {
        enum __lcd_dot          dot;
        enum __lcd_unit         unit;
        enum __lcd_date_format  format;
        enum
        {
            LCD_WIN_SHOW_BIN = 0,
            LCD_WIN_SHOW_DEC,
            LCD_WIN_SHOW_HEX,
            LCD_WIN_SHOW_DATE,
            LCD_WIN_SHOW_MSG,
            LCD_WIN_SHOW_NONE,
            LCD_WIN_SHOW_ALL,
            
        }                       type;
        
        union
        {
            uint16_t            bin;
            int32_t             dec;
            uint32_t            hex;
            uint64_t            date;
            uint8_t             message[8];
            
        }                       value;
        
    }                           windows[LCD_MAX_WINDOWS];

    struct
    {
        enum
        {
            LCD_LAB_SHOW_OFF = 0,
            LCD_LAB_SHOW_ON,
            LCD_LAB_SHOW_FLASH,
            
        }                       status;
        
        uint8_t                 value;
        
    }                           label[LCD_MAX_LABELS];
    
    enum
    {
        LCD_GLO_NONE_OPTION = 0,
        LCD_GLO_SHOW_NONE,
        LCD_GLO_SHOW_ALL,
        
    }                           global;
    
    enum
    {
        LCD_BKL_NONE = 0,
        LCD_BKL_OPEN,
        
    }                           backlight;
};
#else

#if defined (DEMO_STM32F091)
struct __lcd_params
{
    uint8_t gdram[GDRAM_SIZE];
    uint8_t blink[LCD_MAX_LABELS / 8 + 1];
	uint16_t counter;
	uint8_t flash;
    uint8_t flush;
};
#endif

#endif

/* Private variables ---------------------------------------------------------*/
static enum __dev_status status = DEVICE_NOTINIT;

#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
static SOCKADDR_IN src;
static SOCKET sock = INVALID_SOCKET;
static struct __win_lcd_message lcd_message;
#else

#if defined (DEMO_STM32F091)
static struct __lcd_params params;
#endif

#endif

/* Private function prototypes -----------------------------------------------*/
static void lcd_label_on(uint8_t channel, uint8_t state);
static void lcd_label_off(uint8_t channel);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  
  */
static enum __dev_status lcd_status(void)
{
    return(status);
}

/**
  * @brief  
  */
static void lcd_init(enum __dev_state state)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    memset(&lcd_message, 0, sizeof(lcd_message));
    
    lcd_message.global = LCD_GLO_SHOW_NONE;
    lcd_message.backlight = LCD_BKL_NONE;

	sock = emitter.open(50001, &src);

    if(sock == INVALID_SOCKET)
    {
        status = DEVICE_ERROR;
    }
    else
    {
		emitter.write(sock, &src, (uint8_t *)&lcd_message, sizeof(lcd_message));
        status = DEVICE_INIT;
    }
#else
    
#if defined (DEMO_STM32F091)
    GPIO_InitTypeDef GPIO_InitStruct;
    
    deviic.control.init(state);
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);
    
    if(state == DEVICE_NORMAL)
    {
        //n backlight
        GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Level_1;
        GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
        GPIO_Init(GPIOE, &GPIO_InitStruct);

        GPIO_SetBits(GPIOE, GPIO_Pin_13);
    }
    
    //p power
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_Init(GPIOE, &GPIO_InitStruct);
	
    GPIO_SetBits(GPIOE, GPIO_Pin_14);
    
	//init
    udelay(200);
    memset(&params, 0, sizeof(params));
	deviic.bus.write(LCD_ADDR, LCD_INIT, 4, sizeof(params.gdram), params.gdram);
	
	//enable display
	deviic.bus.write(LCD_ADDR, LCD_MODSET_ON, 1, 0, 0);
    
    status = DEVICE_INIT;
#endif

#endif
}

/**
  * @brief  
  */
static void lcd_suspend(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    memset(&lcd_message, 0, sizeof(lcd_message));
    lcd_message.global = LCD_GLO_SHOW_NONE;
    
    if(sock != INVALID_SOCKET)
    {
		emitter.write(sock, &src, (uint8_t *)&lcd_message, sizeof(lcd_message));
		emitter.close(sock);
		sock = INVALID_SOCKET;
    }
#else
    
#if defined (DEMO_STM32F091)
	//disable display
	deviic.bus.write(LCD_ADDR, LCD_MODSET_OFF, 1, 0, 0);
    deviic.control.suspend();
#endif
    
#endif
    
    status = DEVICE_NOTINIT;
}



/**
  * @brief  
  */
static void lcd_runner(uint16_t msecond)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    if((sock != INVALID_SOCKET) && (status == DEVICE_INIT))
    {
        if(emitter.write(sock, &src, (uint8_t *)&lcd_message, sizeof(lcd_message)) != sizeof(lcd_message))
        {
			emitter.close(sock);
			sock = INVALID_SOCKET;
        }
    }
    else
    {
		sock = emitter.open(50001, &src);

		if (sock != INVALID_SOCKET)
		{
			status = DEVICE_INIT;
		}
    }
#else
    
#if defined (DEMO_STM32F091)
	uint8_t labels;
	
	if(status != DEVICE_INIT)
	{
		return;
	}
	
    if((params.counter + msecond) > 999)
    {
		for(labels=0; labels<LCD_MAX_LABELS; labels++)
		{
			if(params.blink[labels/8] & (1<<(labels%8)))
			{
				if(params.flash)
				{
					lcd_label_on(labels, 0);
				}
				else
				{
					lcd_label_off(labels);
				}
			}
		}
		
		if(params.flash)
		{
			params.flash = 0;
		}
		else
		{
			params.flash = 0xff;
		}
		
		params.counter = 0;
    }
	
	params.counter += msecond;
	
	if(params.flush)
	{
		deviic.bus.write(LCD_ADDR, 0, 1,  sizeof(params.gdram), params.gdram);
		deviic.bus.write(LCD_ADDR, LCD_MODSET_ON, 1, 0, 0);
		params.flush = 0;
	}
#endif
    
#endif
}

/**
  * @brief  
  */
static void lcd_show_none(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    lcd_message.global = LCD_GLO_SHOW_NONE;
#else
	
#if defined (DEMO_STM32F091)
	memset(params.gdram, 0, sizeof(params.gdram));
	memset(params.blink, 0, sizeof(params.blink));
	//write data
	deviic.bus.write(LCD_ADDR, 0, 1, sizeof(params.gdram), params.gdram);
	//enable display
	deviic.bus.write(LCD_ADDR, LCD_MODSET_ON, 1, 0, 0);
#endif

#endif
}

/**
  * @brief  
  */
static void lcd_show_all(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    lcd_message.global = LCD_GLO_SHOW_ALL;
#else
	
#if defined (DEMO_STM32F091)
	memset(params.gdram, 0xff, sizeof(params.gdram));
	memset(params.blink, 0, sizeof(params.blink));
	//write data
	deviic.bus.write(LCD_ADDR, 0, 1,  sizeof(params.gdram), params.gdram);
	//enable display
	deviic.bus.write(LCD_ADDR, LCD_MODSET_ON, 1, 0, 0);
#endif
	
#endif
}

/**
  * @brief  
  */
static enum __lcd_backlight lcd_backlight_open(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    lcd_message.backlight = LCD_BKL_OPEN;
    return(lcd_message.backlight);
#else
    
#if defined (DEMO_STM32F091)
    GPIO_ResetBits(GPIOE, GPIO_Pin_13);
    return(LCD_BACKLIGHT_ON);
#endif
    
#endif
}

/**
  * @brief  
  */
static enum __lcd_backlight lcd_backlight_close(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    lcd_message.backlight = LCD_BKL_NONE;
    return(lcd_message.backlight);
#else
    
#if defined (DEMO_STM32F091)
    GPIO_SetBits(GPIOE, GPIO_Pin_13);
    return(LCD_BACKLIGHT_OFF);
#endif
    
#endif
}

/**
  * @brief  
  */
static enum __lcd_backlight lcd_backlight_status(void)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
    return(lcd_message.backlight);
#else
    
#if defined (DEMO_STM32F091)
    if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_13) == Bit_SET)
    {
        return(LCD_BACKLIGHT_OFF);
    }
    else
    {
        return(LCD_BACKLIGHT_ON);
    }
#endif
    
#endif
}



/**
  * @brief  
  */
static void window_show_bin(uint8_t channel, uint16_t val, enum __lcd_dot dot, enum __lcd_unit unit)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	if(channel >= LCD_MAX_WINDOWS)
	{
		return;
	}
    lcd_message.global = LCD_GLO_NONE_OPTION;
	lcd_message.windows[channel].dot = dot;
	lcd_message.windows[channel].unit = unit;
	lcd_message.windows[channel].type = LCD_WIN_SHOW_BIN;
	lcd_message.windows[channel].value.bin = val;
#else

#endif
}

/**
  * @brief  
  */
static void window_show_dec(uint8_t channel, int32_t val, enum __lcd_dot dot, enum __lcd_unit unit)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	if(channel >= LCD_MAX_WINDOWS)
	{
		return;
	}
    lcd_message.global = LCD_GLO_NONE_OPTION;
	lcd_message.windows[channel].dot = dot;
	lcd_message.windows[channel].unit = unit;
	lcd_message.windows[channel].type = LCD_WIN_SHOW_DEC;
	lcd_message.windows[channel].value.dec = val;
#else

#endif
}


static void window_show_hex(uint8_t channel, uint32_t val, enum __lcd_dot dot, enum __lcd_unit unit)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	if(channel >= LCD_MAX_WINDOWS)
	{
		return;
	}
    lcd_message.global = LCD_GLO_NONE_OPTION;
	lcd_message.windows[channel].dot = dot;
	lcd_message.windows[channel].unit = unit;
	lcd_message.windows[channel].type = LCD_WIN_SHOW_HEX;
	lcd_message.windows[channel].value.hex = val;
#else

#endif
}

/**
  * @brief  
  */
static void window_show_date(uint8_t channel, uint64_t val, enum __lcd_date_format theme)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	if(channel >= LCD_MAX_WINDOWS)
	{
		return;
	}
    lcd_message.global = LCD_GLO_NONE_OPTION;
	lcd_message.windows[channel].format = theme;
	lcd_message.windows[channel].type = LCD_WIN_SHOW_DATE;
	lcd_message.windows[channel].value.date = val;
#else

#endif
}

/**
  * @brief  
  */
static uint8_t window_show_msg(uint8_t channel, const char *msg)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	if(channel >= LCD_MAX_WINDOWS)
	{
		return(0);
	}
    lcd_message.global = LCD_GLO_NONE_OPTION;
	lcd_message.windows[channel].type = LCD_WIN_SHOW_MSG;
	if(strlen(msg) > 8)
	{
		memcpy(lcd_message.windows[channel].value.message, msg, 8);
		return(8);
	}
	else
	{
		memset(lcd_message.windows[channel].value.message, 0, sizeof(lcd_message.windows[channel].value.message));
		memcpy(lcd_message.windows[channel].value.message, msg, strlen(msg));
		return((uint8_t)strlen(msg));
	}
#else

#endif
}

/**
  * @brief  
  */
static void window_show_none(uint8_t channel)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	if(channel >= LCD_MAX_WINDOWS)
	{
		return;
	}
    lcd_message.global = LCD_GLO_NONE_OPTION;
	lcd_message.windows[channel].dot = LCD_DOT_NONE;
	lcd_message.windows[channel].unit = LCD_UNIT_NONE;
	lcd_message.windows[channel].type = LCD_WIN_SHOW_DEC;
	lcd_message.windows[channel].value.dec = 0;
#else
	
#endif
}

/**
  * @brief  
  */
static void window_show_all(uint8_t channel)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	if(channel >= LCD_MAX_WINDOWS)
	{
		return;
	}
    lcd_message.global = LCD_GLO_NONE_OPTION;
	lcd_message.windows[channel].dot = LCD_DOT_NONE;
	lcd_message.windows[channel].unit = LCD_UNIT_NONE;
	lcd_message.windows[channel].type = LCD_WIN_SHOW_DEC;
	lcd_message.windows[channel].value.dec = 88888888;
#else

#endif
}

/**
  * @brief  
  */
static void window_read(uint8_t channel, void *dat)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	if(channel >= LCD_MAX_WINDOWS)
	{
		return;
	}
#else

#endif
}





/**
  * @brief  
  */
static void lcd_label_on(uint8_t channel, uint8_t state)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	if(channel >= LCD_MAX_LABELS)
	{
		return;
	}
    lcd_message.global = LCD_GLO_NONE_OPTION;
	lcd_message.label[channel].status = LCD_LAB_SHOW_ON;
	lcd_message.label[channel].value = state;
#else
    
#if defined (DEMO_STM32F091)
    uint8_t gdram[GDRAM_SIZE];
    
    if(channel >= LCD_MAX_LABELS)
    {
        return;
    }
    
    params.blink[channel / 8] &= ~(1 << (channel & 8));
    
	if(!params.flush)
	{
		memcpy(gdram, params.gdram, sizeof(gdram));
	}
    
    switch(channel)
    {
        case LCD_LABEL_U1:
        {
            gdram[29] |= 0x20;
            break;
        }
        case LCD_LABEL_U2:
        {
            gdram[28] |= 0x20;
            break;
        }
        case LCD_LABEL_U3:
        {
            gdram[27] |= 0x20;
            break;
        }
        case LCD_LABEL_I1:
        {
            if(state)
            {
                gdram[18] |= 0x60;
            }
            else
            {
                gdram[18] |= 0x04;
            }
            break;
        }
		case LCD_LABEL_I2:
        {
            if(state)
            {
                gdram[26] |= 0x60;
            }
            else
            {
                gdram[26] |= 0x40;
            }
            break;
        }
		case LCD_LABEL_I3:
        {
            if(state)
            {
                gdram[25] |= 0x20;
                gdram[26] |= 0x80;
            }
            else
            {
                gdram[25] |= 0x20;
            }
            break;
        }
		case LCD_LABEL_L1:
        {
            gdram[2] |= 0x04;
            break;
        }
		case LCD_LABEL_L2:
        {
            gdram[32] |= 0x08;
            break;
        }
		case LCD_LABEL_L3:
        {
            gdram[32] |= 0x04;
            break;
        }
		case LCD_LABEL_LN:
        {
            gdram[32] |= 0x02;
            break;
        }
		case LCD_LABEL_PF:
        {
            gdram[31] |= 0x04;
            break;
        }
		case LCD_LABEL_SIGNAL:
        {
            if(state < 20)
            {
                gdram[24] |= 0x20;
            }
            else if(state < 40)
            {
                gdram[24] |= 0x28;
            }
            else if(state < 60)
            {
                gdram[24] |= 0x28;
                gdram[23] |= 0x08;
            }
            else if(state < 80)
            {
                gdram[24] |= 0x28;
                gdram[23] |= 0x28;
            }
            else if(state < 100)
            {
                gdram[24] |= 0x28;
                gdram[23] |= 0x28;
                gdram[22] |= 0x20;
            }
            break;
        }
		case LCD_LABEL_COMM:
        {
            gdram[21] &= ~0xe0;
            gdram[21] |= 0x60;
            break;
        }
		case LCD_LABEL_QUAD:
        {
            gdram[22] &= ~0xc0;
            gdram[23] &= ~0x40;
            gdram[24] &= ~0xc0;
            switch(state)
            {
                case 1:
                {
                    gdram[22] |= 0x80;
                    break;
                }
                case 2:
                {
                    gdram[24] |= 0x80;
                    break;
                }
                case 3:
                {
                    gdram[24] |= 0x40;
                    break;
                }
                case 4:
                {
                    gdram[22] |= 0x40;
                    break;
                }
            }
            break;
        }
		case LCD_LABEL_RATE:
        {
            gdram[1] &= ~0x0c;
            gdram[2] &= ~0x0a;
            switch(state)
            {
                case 1:
                {
                    gdram[1] |= 0x08;
                    break;
                }
                case 2:
                {
                    gdram[2] |= 0x08;
                    break;
                }
                case 3:
                {
                    gdram[1] |= 0x04;
                    break;
                }
                case 4:
                {
                    gdram[2] |= 0x02;
                    break;
                }
            }
            break;
        }
		case LCD_LABEL_BATRTC:
        {
            gdram[26] |= 0x80;
            break;
        }
		case LCD_LABEL_BATBAK:
        {
            gdram[25] |= 0x80;
            break;
        }
		case LCD_LABEL_DATE:
        {
            gdram[29] |= 0x10;
            break;
        }
		case LCD_LABEL_TIM:
        {
            gdram[31] |= 0x08;
            break;
        }
    }
    
	if(!params.flush)
	{
		if(memcmp(gdram, params.gdram, sizeof(gdram)))
		{
			memcpy(params.gdram, gdram, sizeof(gdram));
			params.flush = 0xff;
		}
	}
    
#endif
    
#endif
}

/**
  * @brief  
  */
static void lcd_label_off(uint8_t channel)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	if(channel >= LCD_MAX_LABELS)
	{
		return;
	}
    lcd_message.global = LCD_GLO_NONE_OPTION;
	lcd_message.label[channel].status = LCD_LAB_SHOW_OFF;
#else
    
#if defined (DEMO_STM32F091)
    uint8_t gdram[GDRAM_SIZE];
    
    if(channel >= LCD_MAX_LABELS)
    {
        return;
    }
    
    params.blink[channel / 8] &= ~(1 << (channel & 8));
    
	if(!params.flush)
	{
		memcpy(gdram, params.gdram, sizeof(gdram));
	}
    
    switch(channel)
    {
        case LCD_LABEL_U1:
        {
            gdram[29] &= ~0x20;
            break;
        }
        case LCD_LABEL_U2:
        {
            gdram[28] &= ~0x20;
            break;
        }
        case LCD_LABEL_U3:
        {
            gdram[27] &= ~0x20;
            break;
        }
        case LCD_LABEL_I1:
        {
			gdram[18] &= ~0x60;
            break;
        }
		case LCD_LABEL_I2:
        {
			gdram[26] &= ~0x60;
            break;
        }
		case LCD_LABEL_I3:
        {
			gdram[25] &= ~0x20;
			gdram[26] &= ~0x80;
            break;
        }
		case LCD_LABEL_L1:
        {
            gdram[2] &= ~0x04;
            break;
        }
		case LCD_LABEL_L2:
        {
            gdram[32] &= ~0x08;
            break;
        }
		case LCD_LABEL_L3:
        {
            gdram[32] &= ~0x04;
            break;
        }
		case LCD_LABEL_LN:
        {
            gdram[32] &= ~0x02;
            break;
        }
		case LCD_LABEL_PF:
        {
            gdram[31] &= ~0x04;
            break;
        }
		case LCD_LABEL_SIGNAL:
        {
			gdram[24] &= ~0x28;
			gdram[23] &= ~0x28;
			gdram[22] &= ~0x20;
            break;
        }
		case LCD_LABEL_COMM:
        {
            gdram[21] &= ~0xe0;
            break;
        }
		case LCD_LABEL_QUAD:
        {
            gdram[22] &= ~0xc0;
            gdram[23] &= ~0x40;
            gdram[24] &= ~0xc0;
            break;
        }
		case LCD_LABEL_RATE:
        {
            gdram[1] &= ~0x0c;
            gdram[2] &= ~0x0a;
            break;
        }
		case LCD_LABEL_BATRTC:
        {
            gdram[26] &= ~0x80;
            break;
        }
		case LCD_LABEL_BATBAK:
        {
            gdram[25] &= ~0x80;
            break;
        }
		case LCD_LABEL_DATE:
        {
            gdram[29] &= ~0x10;
            break;
        }
		case LCD_LABEL_TIM:
        {
            gdram[31] &= ~0x08;
            break;
        }
    }
    
	if(!params.flush)
	{
		if(memcmp(gdram, params.gdram, sizeof(gdram)))
		{
			memcpy(params.gdram, gdram, sizeof(gdram));
			params.flush = 0xff;
		}
	}
    
#endif
    
#endif
}

/**
  * @brief  
  */
static void lcd_label_flash(uint8_t channel)
{
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )
	if(channel >= LCD_MAX_LABELS)
	{
		return;
	}
    lcd_message.global = LCD_GLO_NONE_OPTION;
	lcd_message.label[channel].status = LCD_LAB_SHOW_FLASH;
#else
    
#if defined (DEMO_STM32F091)
    if(channel >= LCD_MAX_LABELS)
    {
        return;
    }
    
    params.blink[channel / 8] |= (1 << (channel & 8));
#endif
    
#endif
}



















/**
  * @brief  
  */
const struct __lcd lcd = 
{
    .control                = 
    {
        .name               = "lcd driver",
        .status             = lcd_status,
        .init               = lcd_init,
        .suspend            = lcd_suspend,
    },
    
    .runner                 = lcd_runner,
    
    .show                   = 
    {
        .none               = lcd_show_none,
        .all                = lcd_show_all,
    },

    .backlight              = 
    {
        .open               = lcd_backlight_open,
        .close              = lcd_backlight_close,
        .status             = lcd_backlight_status,
    },
    
    .window                 = 
    {
        .show               = 
        {
            .bin            = window_show_bin,
            .dec            = window_show_dec,
            .hex            = window_show_hex,
            .date           = window_show_date,
            .msg            = window_show_msg,
            .none           = window_show_none,
            .all            = window_show_all,
        },
        
        .read               = window_read,
    },
    
    .label                  = 
    {
        .on                 = lcd_label_on,
        .off                = lcd_label_off,
        .flash              = lcd_label_flash,
    },
};


