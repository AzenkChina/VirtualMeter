/**
 * @brief		
 * @details		
 * @date		2016-08-16
 **/

/* Includes ------------------------------------------------------------------*/
#include "time.h"
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

//digitals 数组中的特殊字符
#define DIGIT_H                 16
#define DIGIT_h                 17
#define DIGIT_L                 18
#define DIGIT_n                 19
#define DIGIT_N                 20
#define DIGIT_o                 21
#define DIGIT_P                 22
#define DIGIT_r                 23
#define DIGIT_t                 24
#define DIGIT_U                 25
#define DIGIT_INVALID           26

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
    enum
    {
        LCD_SHOW_NORMAL = 0,
        LCD_SHOW_ALL = 0x0F,
        LCD_SHOW_NONE = 0xF0,
    } global;
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

/**
  * @ 数码管段位排序如下：
  * @
  * @     a
  * @   -----
  * @  |     |
  * @  |f    |b
  * @  |  g  |
  * @   -----
  * @  |     |
  * @  |e    |c
  * @  |  d  |
  * @   ----- .dp
  * @
  */

/**
  * @brief  共阴数码管取模
  */
static const uint8_t digitals[] = 
{
    0x3f, 0x06, 0x5b, 0x4f,//0 1 2 3
    0x66, 0x6d, 0x7d, 0x07,//4 5 6 7
    0x7f, 0x6f, 0x77, 0x7c,//8 9 a b
    0x39, 0x5e, 0x79, 0x71,//c d e f
	0x76, 0x74, 0x38, 0x54,//H h L n
	0x37, 0x5C, 0x73, 0x50,//N o P r
	0x78, 0x3E, 0x40,      //t U -
};

/**
  * @brief  主数码管阵列在显示缓冲中的bit编码表
  */
static const uint16_t matrix_main[8][7] = 
{
	// a    b    c    d    e    f    g
    {139, 138, 137, 128, 129, 131, 130},//右 0
    {123, 122, 121, 112, 113, 115, 114},//右 1
    {107, 106, 105,  96,  97,  99,  98},//右 2
    {91,   90,  89, 216, 217, 219, 218},//右 3
    {83,   82,  81,  72,  73,  75,  74},//右 4
    {67,   66,  65,  56,  57,  59,  58},//右 5
    {51,   50,  49,  40,  41,  43,  42},//右 6
    {35,   34,  33,  24,  25,  27,  26},//右 7
};

/**
  * @brief  副数码管阵列在显示缓冲中的bit编码表
  */
static const uint16_t matrix_sub[8][7] = 
{
	// a    b    c    d    e    f    g
    {103, 102, 101,  92,  93,  95,  94},//右 0
    {223, 222, 221, 228, 229, 231, 230},//右 1
    {239, 238, 237, 244, 245, 247, 246},//右 2
    {71,   70,  69,  60,  61,  63,  62},//右 3
    {55,   54,  53,  44,  45,  47,  46},//右 4
    {39,   38,  37,  28,  29,  31,  30},//右 5
    {255, 254, 253, 260, 261, 263, 262},//右 6
    {23,   22,  21,  12,  13,  15,  14},//右 7
};

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
	
	if(params.global != LCD_SHOW_NORMAL)
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
	uint8_t gdram[GDRAM_SIZE];
	
	memset(gdram, 0, sizeof(gdram));
	deviic.bus.write(LCD_ADDR, 0, 1,  sizeof(gdram), gdram);
	deviic.bus.write(LCD_ADDR, LCD_MODSET_ON, 1, 0, 0);
    params.global = LCD_SHOW_NONE;
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
	uint8_t gdram[GDRAM_SIZE];
	
	memset(gdram, 0xff, sizeof(gdram));
	deviic.bus.write(LCD_ADDR, 0, 1,  sizeof(gdram), gdram);
	deviic.bus.write(LCD_ADDR, LCD_MODSET_ON, 1, 0, 0);
	
    params.global = LCD_SHOW_ALL;
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
    
#if defined (DEMO_STM32F091)
    uint8_t tubes, bits, effect;
    uint8_t number[8];
	uint8_t gdram[GDRAM_SIZE];
	
	if(channel > LCD_MAX_WINDOWS)
	{
		return;
	}
	
	params.global = LCD_SHOW_NORMAL;
	
	if(!params.flush)
	{
		memcpy(gdram, params.gdram, sizeof(gdram));
	}
	
	//数字处理
	memset(number, 0, sizeof(number));
	for(tubes=0; tubes<8; tubes++)
	{
		number[tubes] = val % 2;
		val = val / 2;
		
		if(val == 0)
		{
			break;
		}
	}
	
	effect = 8;
	
	for(tubes=0; tubes<8; tubes++)
	{
		if(number[7 - tubes] == 0)
		{
			effect = 7 - tubes;
		}
		else
		{
			break;
		}
	}
	
	if(effect == 0)
	{
		effect = 1;
	}

    if(channel == LCD_WINDOW_MAIN)
    {
        //小数点处理
        params.gdram[28] &= ~0x04;//冒号1
        params.gdram[26] &= ~0x08;//冒号2
        
        params.gdram[15] &= ~0x01;
        params.gdram[13] &= ~0x01;
        params.gdram[11] &= ~0x01;
        params.gdram[10] &= ~0x01;
        params.gdram[8] &= ~0x01;
        params.gdram[6] &= ~0x01;
        params.gdram[4] &= ~0x01;
        
        switch(dot)
        {
            case LCD_DOT_1:
            {
                params.gdram[15] |= 0x01;
				if(effect < 2)
				{
					effect = 2;
				}
                break;
            }
            case LCD_DOT_2:
            {
                params.gdram[13] |= 0x01;
				if(effect < 3)
				{
					effect = 3;
				}
                break;
            }
            case LCD_DOT_3:
            {
                params.gdram[11] |= 0x01;
				if(effect < 4)
				{
					effect = 4;
				}
                break;
            }
            case LCD_DOT_4:
            {
                params.gdram[10] |= 0x01;
				if(effect < 5)
				{
					effect = 5;
				}
                break;
            }
            case LCD_DOT_5:
            {
                params.gdram[8] |= 0x01;
				if(effect < 6)
				{
					effect = 6;
				}
                break;
            }
            case LCD_DOT_6:
            {
                params.gdram[6] |= 0x01;
				if(effect < 7)
				{
					effect = 7;
				}
                break;
            }
            case LCD_DOT_7:
            {
                params.gdram[4] |= 0x01;
				if(effect < 8)
				{
					effect = 8;
				}
                break;
            }
        }
        
        //数字处理
        for(tubes=0; tubes<8; tubes++)
        {
            for(bits=0; bits<7; bits++)
            {
                params.gdram[matrix_main[tubes][bits] / 8] &= ~(1 << (matrix_main[tubes][bits] % 8));
            }
        }
        
        for(tubes=0; tubes<effect; tubes++)
        {
            for(bits=0; bits<7; bits++)
            {
                if((digitals[number[tubes]] >> bits) & 0x01)
                {
                    params.gdram[matrix_main[tubes][bits] / 8] |= (1 << (matrix_main[tubes][bits] % 8));
                }
            }
        }
		
		//负号处理
        params.gdram[2] &= ~0x01;
        
        //单位处理
        params.gdram[18] &= ~0x02;//M
        params.gdram[19] &= ~0x02;//K
        params.gdram[20] &= ~0x06;//VV
        params.gdram[20] &= ~0x08;//A
        params.gdram[21] &= ~0x08;//H
        
        params.gdram[17] &= ~0x01;//M
        params.gdram[18] &= ~0x01;//K
        params.gdram[19] &= ~0x01;//var
        params.gdram[20] &= ~0x01;//H
        
        params.gdram[18] &= ~0x04;
        params.gdram[19] &= ~0x04;//x1000
        params.gdram[24] &= ~0x08;//Hz
        
        switch(unit)
        {
            case LCD_UNIT_V: 
            {
                params.gdram[20] |= 0x04;
                break;
            }
            case LCD_UNIT_KV: 
            {
                params.gdram[19] |= 0x02;
                params.gdram[20] |= 0x04;
                break;
            }
            case LCD_UNIT_MV: 
            {
                params.gdram[18] |= 0x02;
                params.gdram[20] |= 0x04;
                break;
            }
            case LCD_UNIT_A: 
            {
                params.gdram[20] |= 0x08;
                break;
            }
            case LCD_UNIT_KA: 
            {
                params.gdram[19] |= 0x02;
                params.gdram[20] |= 0x08;
                break;
            }
            case LCD_UNIT_MA: 
            {
                params.gdram[18] |= 0x02;
                params.gdram[20] |= 0x08;
                break;
            }
            case LCD_UNIT_W: 
            {
                params.gdram[20] |= 0x06;
                break;
            }
            case LCD_UNIT_KW: 
            {
                params.gdram[19] |= 0x02;
                params.gdram[20] |= 0x06;
                break;
            }
            case LCD_UNIT_MW: 
            {
                params.gdram[18] |= 0x02;
                params.gdram[20] |= 0x06;
                break;
            }
            case LCD_UNIT_VAR: 
            {
                params.gdram[19] |= 0x01;
                break;
            }
            case LCD_UNIT_KVAR: 
            {
                params.gdram[18] |= 0x01;
                params.gdram[19] |= 0x01;
                break;
            }
            case LCD_UNIT_MVAR: 
            {
                params.gdram[17] |= 0x01;
                params.gdram[19] |= 0x01;
                break;
            }
            case LCD_UNIT_VA: 
            {
                params.gdram[20] |= 0x0c;
                break;
            }
            case LCD_UNIT_KVA: 
            {
                params.gdram[19] |= 0x02;
                params.gdram[20] |= 0x0a;
                break;
            }
            case LCD_UNIT_MVA: 
            {
                params.gdram[18] |= 0x02;
                params.gdram[20] |= 0x0a;
                break;
            }
            case LCD_UNIT_WH: 
            {
                params.gdram[20] |= 0x06;
                params.gdram[21] |= 0x08;
                break;
            }
            case LCD_UNIT_KWH: 
            {
                params.gdram[19] |= 0x02;
                params.gdram[20] |= 0x06;
                params.gdram[21] |= 0x08;
                break;
            }
            case LCD_UNIT_MWH: 
            {
                params.gdram[18] |= 0x02;
                params.gdram[20] |= 0x06;
                params.gdram[21] |= 0x08;
                break;
            }
            case LCD_UNIT_VARH: 
            {
                params.gdram[19] |= 0x01;
                params.gdram[20] |= 0x01;
                break;
            }
            case LCD_UNIT_KVARH: 
            {
                params.gdram[18] |= 0x01;
                params.gdram[19] |= 0x01;
                params.gdram[20] |= 0x01;
                break;
            }
            case LCD_UNIT_MVARH: 
            {
                params.gdram[17] |= 0x01;
                params.gdram[19] |= 0x01;
                params.gdram[20] |= 0x01;
                break;
            }
            case LCD_UNIT_VAH: 
            {
                params.gdram[20] |= 0x0a;
                params.gdram[21] |= 0x08;
                break;
            }
            case LCD_UNIT_KVAH: 
            {
                params.gdram[19] |= 0x02;
                params.gdram[20] |= 0x0a;
                params.gdram[21] |= 0x08;
                break;
            }
            case LCD_UNIT_MVAH: 
            {
                params.gdram[18] |= 0x02;
                params.gdram[20] |= 0x0a;
                params.gdram[21] |= 0x08;
                break;
            }
            case LCD_UNIT_HZ: 
            {
                params.gdram[24] |= 0x08;
                break;
            }
            case LCD_UNIT_KHZ: 
            {
                params.gdram[18] |= 0x04;
                params.gdram[19] |= 0x04;
                params.gdram[24] |= 0x08;
                break;
            }
            case LCD_UNIT_MHZ: 
            {
                params.gdram[18] |= 0x02;
                params.gdram[24] |= 0x08;
                break;
            }
        }
        
        //符号处理
        params.gdram[32] &= ~0x02;
    }
	else if(channel == LCD_WINDOW_SUB)
	{
        //小数点处理
        params.gdram[11] &= ~0x10;
        params.gdram[10] &= ~0x10;
        params.gdram[8] &= ~0x10;
        params.gdram[31] &= ~0x10;
        params.gdram[6] &= ~0x10;
        params.gdram[4] &= ~0x10;
        params.gdram[2] &= ~0x10;
        
        switch(dot)
        {
            case LCD_DOT_1:
            {
                params.gdram[27] |= 0x10;
				if(effect < 2)
				{
					effect = 2;
				}
                break;
            }
            case LCD_DOT_2:
            {
                params.gdram[29] |= 0x10;
				if(effect < 3)
				{
					effect = 3;
				}
                break;
            }
            case LCD_DOT_3:
            {
                params.gdram[8] |= 0x10;
				if(effect < 4)
				{
					effect = 4;
				}
                break;
            }
            case LCD_DOT_4:
            {
                params.gdram[6] |= 0x10;
				if(effect < 5)
				{
					effect = 5;
				}
                break;
            }
            case LCD_DOT_5:
            {
				params.gdram[4] |= 0x10;
				if(effect < 6)
				{
					effect = 6;
				}
                break;
            }
            case LCD_DOT_6:
            {
				params.gdram[31] |= 0x10;
				if(effect < 7)
				{
					effect = 7;
				}
                break;
            }
            case LCD_DOT_7:
            {
				params.gdram[2] |= 0x10;
				if(effect < 8)
				{
					effect = 8;
				}
                break;
            }
        }
		
        //数字处理
        for(tubes=0; tubes<8; tubes++)
        {
            for(bits=0; bits<7; bits++)
            {
                params.gdram[matrix_sub[tubes][bits] / 8] &= ~(1 << (matrix_sub[tubes][bits] % 8));
            }
        }
        
        for(tubes=0; tubes<effect; tubes++)
        {
            for(bits=0; bits<7; bits++)
            {
                if((digitals[number[tubes]] >> bits) & 0x01)
                {
                    params.gdram[matrix_sub[tubes][bits] / 8] |= (1 << (matrix_sub[tubes][bits] % 8));
                }
            }
        }
	}
	
    if(!params.flush)
    {
        if(memcmp(gdram, params.gdram, sizeof(gdram)))
        {
            params.flush = 0xff;
        }
    }
#endif
    
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
    
#if defined (DEMO_STM32F091)
    uint8_t tubes, bits, effect, minus;
    uint8_t number[8];
	uint8_t gdram[GDRAM_SIZE];
	
	if(channel > LCD_MAX_WINDOWS)
	{
		return;
	}
	
	params.global = LCD_SHOW_NORMAL;
	
	if(!params.flush)
	{
		memcpy(gdram, params.gdram, sizeof(gdram));
	}
	
	//负数处理
	if(val < 0)
	{
		minus = 0xff;
		val = -val;
	}
	else
	{
		minus = 0;
	}
	
	//数字处理
	memset(number, 0, sizeof(number));
	for(tubes=0; tubes<8; tubes++)
	{
		number[tubes] = val % 10;
		val = val / 10;
		
		if(val == 0)
		{
			break;
		}
	}
	
	effect = 8;
	
	for(tubes=0; tubes<8; tubes++)
	{
		if(number[7 - tubes] == 0)
		{
			effect = 7 - tubes;
		}
		else
		{
			break;
		}
	}
	
	if(effect == 0)
	{
		effect = 1;
	}

    if(channel == LCD_WINDOW_MAIN)
    {
        //小数点处理
        params.gdram[28] &= ~0x04;//冒号1
        params.gdram[26] &= ~0x08;//冒号2
        
        params.gdram[15] &= ~0x01;
        params.gdram[13] &= ~0x01;
        params.gdram[11] &= ~0x01;
        params.gdram[10] &= ~0x01;
        params.gdram[8] &= ~0x01;
        params.gdram[6] &= ~0x01;
        params.gdram[4] &= ~0x01;
        
        switch(dot)
        {
            case LCD_DOT_1:
            {
                params.gdram[15] |= 0x01;
				if(effect < 2)
				{
					effect = 2;
				}
                break;
            }
            case LCD_DOT_2:
            {
                params.gdram[13] |= 0x01;
				if(effect < 3)
				{
					effect = 3;
				}
                break;
            }
            case LCD_DOT_3:
            {
                params.gdram[11] |= 0x01;
				if(effect < 4)
				{
					effect = 4;
				}
                break;
            }
            case LCD_DOT_4:
            {
                params.gdram[10] |= 0x01;
				if(effect < 5)
				{
					effect = 5;
				}
                break;
            }
            case LCD_DOT_5:
            {
                params.gdram[8] |= 0x01;
				if(effect < 6)
				{
					effect = 6;
				}
                break;
            }
            case LCD_DOT_6:
            {
                params.gdram[6] |= 0x01;
				if(effect < 7)
				{
					effect = 7;
				}
                break;
            }
            case LCD_DOT_7:
            {
                params.gdram[4] |= 0x01;
				if(effect < 8)
				{
					effect = 8;
				}
                break;
            }
        }
        
        //数字处理
        for(tubes=0; tubes<8; tubes++)
        {
            for(bits=0; bits<7; bits++)
            {
                params.gdram[matrix_main[tubes][bits] / 8] &= ~(1 << (matrix_main[tubes][bits] % 8));
            }
        }
        
        for(tubes=0; tubes<effect; tubes++)
        {
            for(bits=0; bits<7; bits++)
            {
                if((digitals[number[tubes]] >> bits) & 0x01)
                {
                    params.gdram[matrix_main[tubes][bits] / 8] |= (1 << (matrix_main[tubes][bits] % 8));
                }
            }
        }
		
		//负号处理
        if(minus)
        {
            params.gdram[32] |= 0x02;
        }
        else
        {
            params.gdram[32] &= ~0x02;
        }
        
        //单位处理
        params.gdram[18] &= ~0x02;//M
        params.gdram[19] &= ~0x02;//K
        params.gdram[20] &= ~0x06;//VV
        params.gdram[20] &= ~0x08;//A
        params.gdram[21] &= ~0x08;//H
        
        params.gdram[17] &= ~0x01;//M
        params.gdram[18] &= ~0x01;//K
        params.gdram[19] &= ~0x01;//var
        params.gdram[20] &= ~0x01;//H
        
        params.gdram[18] &= ~0x04;
        params.gdram[19] &= ~0x04;//x1000
        params.gdram[24] &= ~0x08;//Hz
        
        switch(unit)
        {
            case LCD_UNIT_V: 
            {
                params.gdram[20] |= 0x04;
                break;
            }
            case LCD_UNIT_KV: 
            {
                params.gdram[19] |= 0x02;
                params.gdram[20] |= 0x04;
                break;
            }
            case LCD_UNIT_MV: 
            {
                params.gdram[18] |= 0x02;
                params.gdram[20] |= 0x04;
                break;
            }
            case LCD_UNIT_A: 
            {
                params.gdram[20] |= 0x08;
                break;
            }
            case LCD_UNIT_KA: 
            {
                params.gdram[19] |= 0x02;
                params.gdram[20] |= 0x08;
                break;
            }
            case LCD_UNIT_MA: 
            {
                params.gdram[18] |= 0x02;
                params.gdram[20] |= 0x08;
                break;
            }
            case LCD_UNIT_W: 
            {
                params.gdram[20] |= 0x06;
                break;
            }
            case LCD_UNIT_KW: 
            {
                params.gdram[19] |= 0x02;
                params.gdram[20] |= 0x06;
                break;
            }
            case LCD_UNIT_MW: 
            {
                params.gdram[18] |= 0x02;
                params.gdram[20] |= 0x06;
                break;
            }
            case LCD_UNIT_VAR: 
            {
                params.gdram[19] |= 0x01;
                break;
            }
            case LCD_UNIT_KVAR: 
            {
                params.gdram[18] |= 0x01;
                params.gdram[19] |= 0x01;
                break;
            }
            case LCD_UNIT_MVAR: 
            {
                params.gdram[17] |= 0x01;
                params.gdram[19] |= 0x01;
                break;
            }
            case LCD_UNIT_VA: 
            {
                params.gdram[20] |= 0x0c;
                break;
            }
            case LCD_UNIT_KVA: 
            {
                params.gdram[19] |= 0x02;
                params.gdram[20] |= 0x0a;
                break;
            }
            case LCD_UNIT_MVA: 
            {
                params.gdram[18] |= 0x02;
                params.gdram[20] |= 0x0a;
                break;
            }
            case LCD_UNIT_WH: 
            {
                params.gdram[20] |= 0x06;
                params.gdram[21] |= 0x08;
                break;
            }
            case LCD_UNIT_KWH: 
            {
                params.gdram[19] |= 0x02;
                params.gdram[20] |= 0x06;
                params.gdram[21] |= 0x08;
                break;
            }
            case LCD_UNIT_MWH: 
            {
                params.gdram[18] |= 0x02;
                params.gdram[20] |= 0x06;
                params.gdram[21] |= 0x08;
                break;
            }
            case LCD_UNIT_VARH: 
            {
                params.gdram[19] |= 0x01;
                params.gdram[20] |= 0x01;
                break;
            }
            case LCD_UNIT_KVARH: 
            {
                params.gdram[18] |= 0x01;
                params.gdram[19] |= 0x01;
                params.gdram[20] |= 0x01;
                break;
            }
            case LCD_UNIT_MVARH: 
            {
                params.gdram[17] |= 0x01;
                params.gdram[19] |= 0x01;
                params.gdram[20] |= 0x01;
                break;
            }
            case LCD_UNIT_VAH: 
            {
                params.gdram[20] |= 0x0a;
                params.gdram[21] |= 0x08;
                break;
            }
            case LCD_UNIT_KVAH: 
            {
                params.gdram[19] |= 0x02;
                params.gdram[20] |= 0x0a;
                params.gdram[21] |= 0x08;
                break;
            }
            case LCD_UNIT_MVAH: 
            {
                params.gdram[18] |= 0x02;
                params.gdram[20] |= 0x0a;
                params.gdram[21] |= 0x08;
                break;
            }
            case LCD_UNIT_HZ: 
            {
                params.gdram[24] |= 0x08;
                break;
            }
            case LCD_UNIT_KHZ: 
            {
                params.gdram[18] |= 0x04;
                params.gdram[19] |= 0x04;
                params.gdram[24] |= 0x08;
                break;
            }
            case LCD_UNIT_MHZ: 
            {
                params.gdram[18] |= 0x02;
                params.gdram[24] |= 0x08;
                break;
            }
        }
    }
	else if(channel == LCD_WINDOW_SUB)
	{
        //小数点处理
        params.gdram[11] &= ~0x10;
        params.gdram[10] &= ~0x10;
        params.gdram[8] &= ~0x10;
        params.gdram[31] &= ~0x10;
        params.gdram[6] &= ~0x10;
        params.gdram[4] &= ~0x10;
        params.gdram[2] &= ~0x10;
        
        switch(dot)
        {
            case LCD_DOT_1:
            {
                params.gdram[27] |= 0x10;
				if(effect < 2)
				{
					effect = 2;
				}
                break;
            }
            case LCD_DOT_2:
            {
                params.gdram[29] |= 0x10;
				if(effect < 3)
				{
					effect = 3;
				}
                break;
            }
            case LCD_DOT_3:
            {
                params.gdram[8] |= 0x10;
				if(effect < 4)
				{
					effect = 4;
				}
                break;
            }
            case LCD_DOT_4:
            {
                params.gdram[6] |= 0x10;
				if(effect < 5)
				{
					effect = 5;
				}
                break;
            }
            case LCD_DOT_5:
            {
				params.gdram[4] |= 0x10;
				if(effect < 6)
				{
					effect = 6;
				}
                break;
            }
            case LCD_DOT_6:
            {
				params.gdram[31] |= 0x10;
				if(effect < 7)
				{
					effect = 7;
				}
                break;
            }
            case LCD_DOT_7:
            {
				params.gdram[2] |= 0x10;
				if(effect < 8)
				{
					effect = 8;
				}
                break;
            }
        }
		
        //数字处理
        for(tubes=0; tubes<8; tubes++)
        {
            for(bits=0; bits<7; bits++)
            {
                params.gdram[matrix_sub[tubes][bits] / 8] &= ~(1 << (matrix_sub[tubes][bits] % 8));
            }
        }
        
        for(tubes=0; tubes<effect; tubes++)
        {
            for(bits=0; bits<7; bits++)
            {
                if((digitals[number[tubes]] >> bits) & 0x01)
                {
                    params.gdram[matrix_sub[tubes][bits] / 8] |= (1 << (matrix_sub[tubes][bits] % 8));
                }
            }
        }
	}
	
    if(!params.flush)
    {
        if(memcmp(gdram, params.gdram, sizeof(gdram)))
        {
            params.flush = 0xff;
        }
    }
#endif
    
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
    
#if defined (DEMO_STM32F091)
    uint8_t tubes, bits, effect;
    uint8_t number[8];
	uint8_t gdram[GDRAM_SIZE];
	
	if(channel > LCD_MAX_WINDOWS)
	{
		return;
	}
	
	params.global = LCD_SHOW_NORMAL;
	
	if(!params.flush)
	{
		memcpy(gdram, params.gdram, sizeof(gdram));
	}
	
	//数字处理
	memset(number, 0, sizeof(number));
	for(tubes=0; tubes<8; tubes++)
	{
		number[tubes] = val % 16;
		val = val / 16;
		
		if(val == 0)
		{
			break;
		}
	}
	
	effect = 8;
	
	for(tubes=0; tubes<8; tubes++)
	{
		if(number[7 - tubes] == 0)
		{
			effect = 7 - tubes;
		}
		else
		{
			break;
		}
	}
	
	if(effect == 0)
	{
		effect = 1;
	}

    if(channel == LCD_WINDOW_MAIN)
    {
        //小数点处理
        params.gdram[28] &= ~0x04;//冒号1
        params.gdram[26] &= ~0x08;//冒号2
        
        params.gdram[15] &= ~0x01;
        params.gdram[13] &= ~0x01;
        params.gdram[11] &= ~0x01;
        params.gdram[10] &= ~0x01;
        params.gdram[8] &= ~0x01;
        params.gdram[6] &= ~0x01;
        params.gdram[4] &= ~0x01;
        
        switch(dot)
        {
            case LCD_DOT_1:
            {
                params.gdram[15] |= 0x01;
				if(effect < 2)
				{
					effect = 2;
				}
                break;
            }
            case LCD_DOT_2:
            {
                params.gdram[13] |= 0x01;
				if(effect < 3)
				{
					effect = 3;
				}
                break;
            }
            case LCD_DOT_3:
            {
                params.gdram[11] |= 0x01;
				if(effect < 4)
				{
					effect = 4;
				}
                break;
            }
            case LCD_DOT_4:
            {
                params.gdram[10] |= 0x01;
				if(effect < 5)
				{
					effect = 5;
				}
                break;
            }
            case LCD_DOT_5:
            {
                params.gdram[8] |= 0x01;
				if(effect < 6)
				{
					effect = 6;
				}
                break;
            }
            case LCD_DOT_6:
            {
                params.gdram[6] |= 0x01;
				if(effect < 7)
				{
					effect = 7;
				}
                break;
            }
            case LCD_DOT_7:
            {
                params.gdram[4] |= 0x01;
				if(effect < 8)
				{
					effect = 8;
				}
                break;
            }
        }
        
        //数字处理
        for(tubes=0; tubes<8; tubes++)
        {
            for(bits=0; bits<7; bits++)
            {
                params.gdram[matrix_main[tubes][bits] / 8] &= ~(1 << (matrix_main[tubes][bits] % 8));
            }
        }
        
        for(tubes=0; tubes<effect; tubes++)
        {
            for(bits=0; bits<7; bits++)
            {
                if((digitals[number[tubes]] >> bits) & 0x01)
                {
                    params.gdram[matrix_main[tubes][bits] / 8] |= (1 << (matrix_main[tubes][bits] % 8));
                }
            }
        }
        
        //单位处理
        params.gdram[18] &= ~0x02;//M
        params.gdram[19] &= ~0x02;//K
        params.gdram[20] &= ~0x06;//VV
        params.gdram[20] &= ~0x08;//A
        params.gdram[21] &= ~0x08;//H
        
        params.gdram[17] &= ~0x01;//M
        params.gdram[18] &= ~0x01;//K
        params.gdram[19] &= ~0x01;//var
        params.gdram[20] &= ~0x01;//H
        
        params.gdram[18] &= ~0x04;
        params.gdram[19] &= ~0x04;//x1000
        params.gdram[24] &= ~0x08;//Hz
        
        switch(unit)
        {
            case LCD_UNIT_V: 
            {
                params.gdram[20] |= 0x04;
                break;
            }
            case LCD_UNIT_KV: 
            {
                params.gdram[19] |= 0x02;
                params.gdram[20] |= 0x04;
                break;
            }
            case LCD_UNIT_MV: 
            {
                params.gdram[18] |= 0x02;
                params.gdram[20] |= 0x04;
                break;
            }
            case LCD_UNIT_A: 
            {
                params.gdram[20] |= 0x08;
                break;
            }
            case LCD_UNIT_KA: 
            {
                params.gdram[19] |= 0x02;
                params.gdram[20] |= 0x08;
                break;
            }
            case LCD_UNIT_MA: 
            {
                params.gdram[18] |= 0x02;
                params.gdram[20] |= 0x08;
                break;
            }
            case LCD_UNIT_W: 
            {
                params.gdram[20] |= 0x06;
                break;
            }
            case LCD_UNIT_KW: 
            {
                params.gdram[19] |= 0x02;
                params.gdram[20] |= 0x06;
                break;
            }
            case LCD_UNIT_MW: 
            {
                params.gdram[18] |= 0x02;
                params.gdram[20] |= 0x06;
                break;
            }
            case LCD_UNIT_VAR: 
            {
                params.gdram[19] |= 0x01;
                break;
            }
            case LCD_UNIT_KVAR: 
            {
                params.gdram[18] |= 0x01;
                params.gdram[19] |= 0x01;
                break;
            }
            case LCD_UNIT_MVAR: 
            {
                params.gdram[17] |= 0x01;
                params.gdram[19] |= 0x01;
                break;
            }
            case LCD_UNIT_VA: 
            {
                params.gdram[20] |= 0x0c;
                break;
            }
            case LCD_UNIT_KVA: 
            {
                params.gdram[19] |= 0x02;
                params.gdram[20] |= 0x0a;
                break;
            }
            case LCD_UNIT_MVA: 
            {
                params.gdram[18] |= 0x02;
                params.gdram[20] |= 0x0a;
                break;
            }
            case LCD_UNIT_WH: 
            {
                params.gdram[20] |= 0x06;
                params.gdram[21] |= 0x08;
                break;
            }
            case LCD_UNIT_KWH: 
            {
                params.gdram[19] |= 0x02;
                params.gdram[20] |= 0x06;
                params.gdram[21] |= 0x08;
                break;
            }
            case LCD_UNIT_MWH: 
            {
                params.gdram[18] |= 0x02;
                params.gdram[20] |= 0x06;
                params.gdram[21] |= 0x08;
                break;
            }
            case LCD_UNIT_VARH: 
            {
                params.gdram[19] |= 0x01;
                params.gdram[20] |= 0x01;
                break;
            }
            case LCD_UNIT_KVARH: 
            {
                params.gdram[18] |= 0x01;
                params.gdram[19] |= 0x01;
                params.gdram[20] |= 0x01;
                break;
            }
            case LCD_UNIT_MVARH: 
            {
                params.gdram[17] |= 0x01;
                params.gdram[19] |= 0x01;
                params.gdram[20] |= 0x01;
                break;
            }
            case LCD_UNIT_VAH: 
            {
                params.gdram[20] |= 0x0a;
                params.gdram[21] |= 0x08;
                break;
            }
            case LCD_UNIT_KVAH: 
            {
                params.gdram[19] |= 0x02;
                params.gdram[20] |= 0x0a;
                params.gdram[21] |= 0x08;
                break;
            }
            case LCD_UNIT_MVAH: 
            {
                params.gdram[18] |= 0x02;
                params.gdram[20] |= 0x0a;
                params.gdram[21] |= 0x08;
                break;
            }
            case LCD_UNIT_HZ: 
            {
                params.gdram[24] |= 0x08;
                break;
            }
            case LCD_UNIT_KHZ: 
            {
                params.gdram[18] |= 0x04;
                params.gdram[19] |= 0x04;
                params.gdram[24] |= 0x08;
                break;
            }
            case LCD_UNIT_MHZ: 
            {
                params.gdram[18] |= 0x02;
                params.gdram[24] |= 0x08;
                break;
            }
        }
        
        //符号处理
        params.gdram[32] &= ~0x02;
    }
	else if(channel == LCD_WINDOW_SUB)
	{
        //小数点处理
        params.gdram[11] &= ~0x10;
        params.gdram[10] &= ~0x10;
        params.gdram[8] &= ~0x10;
        params.gdram[31] &= ~0x10;
        params.gdram[6] &= ~0x10;
        params.gdram[4] &= ~0x10;
        params.gdram[2] &= ~0x10;
        
        switch(dot)
        {
            case LCD_DOT_1:
            {
                params.gdram[27] |= 0x10;
				if(effect < 2)
				{
					effect = 2;
				}
                break;
            }
            case LCD_DOT_2:
            {
                params.gdram[29] |= 0x10;
				if(effect < 3)
				{
					effect = 3;
				}
                break;
            }
            case LCD_DOT_3:
            {
                params.gdram[8] |= 0x10;
				if(effect < 4)
				{
					effect = 4;
				}
                break;
            }
            case LCD_DOT_4:
            {
                params.gdram[6] |= 0x10;
				if(effect < 5)
				{
					effect = 5;
				}
                break;
            }
            case LCD_DOT_5:
            {
				params.gdram[4] |= 0x10;
				if(effect < 6)
				{
					effect = 6;
				}
                break;
            }
            case LCD_DOT_6:
            {
				params.gdram[31] |= 0x10;
				if(effect < 7)
				{
					effect = 7;
				}
                break;
            }
            case LCD_DOT_7:
            {
				params.gdram[2] |= 0x10;
				if(effect < 8)
				{
					effect = 8;
				}
                break;
            }
        }
		
        //数字处理
        for(tubes=0; tubes<8; tubes++)
        {
            for(bits=0; bits<7; bits++)
            {
                params.gdram[matrix_sub[tubes][bits] / 8] &= ~(1 << (matrix_sub[tubes][bits] % 8));
            }
        }
        
        for(tubes=0; tubes<effect; tubes++)
        {
            for(bits=0; bits<7; bits++)
            {
                if((digitals[number[tubes]] >> bits) & 0x01)
                {
                    params.gdram[matrix_sub[tubes][bits] / 8] |= (1 << (matrix_sub[tubes][bits] % 8));
                }
            }
        }
	}
	
    if(!params.flush)
    {
        if(memcmp(gdram, params.gdram, sizeof(gdram)))
        {
            params.flush = 0xff;
        }
    }
#endif
    
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
    
#if defined (DEMO_STM32F091)
    uint8_t tubes, bits;
	uint8_t gdram[GDRAM_SIZE];
    time_t stamp;
    struct tm *tim;
    uint8_t length_year = 0xff;
    uint8_t index_year = 0xff;
    uint8_t index_mon = 0xff;
    uint8_t index_day = 0xff;
    uint8_t index_hour = 0xff;
    uint8_t index_min = 0xff;
    uint8_t index_sec = 0xff;
    uint16_t number;
	
	if(channel > LCD_MAX_WINDOWS)
	{
		return;
	}
	
	params.global = LCD_SHOW_NORMAL;
	
	if(!params.flush)
	{
		memcpy(gdram, params.gdram, sizeof(gdram));
	}
    
    stamp = (time_t)val;
    tim = gmtime(&stamp);
    
    switch(theme)
    {
        case LCD_DATE_YYMMDD:
        {
            length_year = 4;
            index_year = 4, 
            index_mon = 2;
            index_day = 0;
            break;
        }
        case LCD_DATE_DDMMYY:
        {
            length_year = 4;
            index_year = 0, 
            index_mon = 4;
            index_day = 6;
            break;
        }
        case LCD_DATE_MMDDYY:
        {
            length_year = 4;
            index_year = 0, 
            index_mon = 6;
            index_day = 4;
            break;
        }
        case LCD_DATE_hhmmss:
        {
            index_hour = 4;
            index_min = 2;
            index_sec = 0;
            break;
        }
        case LCD_DATE_ssmmhh:
        {
            index_hour = 0;
            index_min = 2;
            index_sec = 4;
            break;
        }
        case LCD_DATE_mmsshh:
        {
            index_hour = 0;
            index_min = 4;
            index_sec = 2;
            break;
        }
        case LCD_DATE_MMDDhhmm:
        {
            index_mon = 6;
            index_day = 4;
            index_hour = 2;
            index_min = 0;
            break;
        }
        case LCD_DATE_DDMMhhmm:
        {
            index_mon = 4;
            index_day = 6;
            index_hour = 2;
            index_min = 0;
            break;
        }
        case LCD_DATE_MMDDmmhh:
        {
            index_mon = 6;
            index_day = 4;
            index_hour = 0;
            index_min = 2;
            break;
        }
        case LCD_DATE_DDMMmmhh:
        {
            index_mon = 4;
            index_day = 6;
            index_hour = 0;
            index_min = 2;
            break;
        }
    }
    
    if(channel == LCD_WINDOW_MAIN)
    {
        //小数点处理
        params.gdram[28] &= ~0x04;//冒号1
        params.gdram[26] &= ~0x08;//冒号2
        params.gdram[28] &= ~0x08;//TIME
        params.gdram[26] &= ~0x10;//DATE
        
        params.gdram[15] &= ~0x01;
        params.gdram[13] &= ~0x01;
        params.gdram[11] &= ~0x01;
        params.gdram[10] &= ~0x01;
        params.gdram[8] &= ~0x01;
        params.gdram[6] &= ~0x01;
        params.gdram[4] &= ~0x01;
        
        //单位处理
        params.gdram[18] &= ~0x02;//M
        params.gdram[19] &= ~0x02;//K
        params.gdram[20] &= ~0x06;//VV
        params.gdram[20] &= ~0x08;//A
        params.gdram[21] &= ~0x08;//H
        
        params.gdram[17] &= ~0x01;//M
        params.gdram[18] &= ~0x01;//K
        params.gdram[19] &= ~0x01;//var
        params.gdram[20] &= ~0x01;//H
        
        params.gdram[18] &= ~0x04;
        params.gdram[19] &= ~0x04;//x1000
        params.gdram[24] &= ~0x08;//Hz
        
        //数字处理
        for(tubes=0; tubes<8; tubes++)
        {
            for(bits=0; bits<7; bits++)
            {
                params.gdram[matrix_main[tubes][bits] / 8] &= ~(1 << (matrix_main[tubes][bits] % 8));
            }
        }
        
        //时间处理
        if(index_year < 7)
        {
            number = tim->tm_year + 1900;
            for(tubes=index_year; tubes<(index_year + length_year); tubes++)
            {
                for(bits=0; bits<7; bits++)
                {
                    if((digitals[number % 10] >> bits) & 0x01)
                    {
                        params.gdram[matrix_main[tubes][bits] / 8] |= (1 << (matrix_main[tubes][bits] % 8));
                    }
                }
                number = number / 10;
            }
        }
        
        if(index_mon < 7)
        {
            number = tim->tm_mon + 1;
            for(tubes=index_mon; tubes<(index_mon + 2); tubes++)
            {
                for(bits=0; bits<7; bits++)
                {
                    if((digitals[number % 10] >> bits) & 0x01)
                    {
                        params.gdram[matrix_main[tubes][bits] / 8] |= (1 << (matrix_main[tubes][bits] % 8));
                    }
                }
                number = number / 10;
            }
        }
        
        if(index_day < 7)
        {
            number = tim->tm_mday;
            for(tubes=index_day; tubes<(index_day + 2); tubes++)
            {
                for(bits=0; bits<7; bits++)
                {
                    if((digitals[number % 10] >> bits) & 0x01)
                    {
                        params.gdram[matrix_main[tubes][bits] / 8] |= (1 << (matrix_main[tubes][bits] % 8));
                    }
                }
                number = number / 10;
            }
        }
        
        if(index_hour < 7)
        {
            number = tim->tm_hour;
            for(tubes=index_hour; tubes<(index_hour + 2); tubes++)
            {
                for(bits=0; bits<7; bits++)
                {
                    if((digitals[number % 10] >> bits) & 0x01)
                    {
                        params.gdram[matrix_main[tubes][bits] / 8] |= (1 << (matrix_main[tubes][bits] % 8));
                    }
                }
                number = number / 10;
            }
        }
        
        if(index_min < 7)
        {
            number = tim->tm_min;
            for(tubes=index_min; tubes<(index_min + 2); tubes++)
            {
                for(bits=0; bits<7; bits++)
                {
                    if((digitals[number % 10] >> bits) & 0x01)
                    {
                        params.gdram[matrix_main[tubes][bits] / 8] |= (1 << (matrix_main[tubes][bits] % 8));
                    }
                }
                number = number / 10;
            }
        }
        
        if(index_sec < 7)
        {
            number = tim->tm_sec;
            for(tubes=index_sec; tubes<(index_sec + 2); tubes++)
            {
                for(bits=0; bits<7; bits++)
                {
                    if((digitals[number % 10] >> bits) & 0x01)
                    {
                        params.gdram[matrix_main[tubes][bits] / 8] |= (1 << (matrix_main[tubes][bits] % 8));
                    }
                }
                number = number / 10;
            }
        }
        
        //符号处理
        params.gdram[32] &= ~0x02;
        
        switch(theme)
        {
            case LCD_DATE_YYMMDD:
            {
                params.gdram[13] |= 0x01;//2
                params.gdram[10] |= 0x01;//4
                break;
            }
            case LCD_DATE_DDMMYY:
            case LCD_DATE_MMDDYY:
            {
                params.gdram[10] |= 0x01;//4
                params.gdram[6] |= 0x01;//6
                break;
            }
            case LCD_DATE_hhmmss:
            case LCD_DATE_ssmmhh:
            case LCD_DATE_mmsshh:
            {
                params.gdram[13] |= 0x01;//2
                params.gdram[10] |= 0x01;//4
                params.gdram[28] |= 0x04;//冒号1
                params.gdram[26] |= 0x08;//冒号2
                break;
            }
            case LCD_DATE_MMDDhhmm:
            case LCD_DATE_DDMMhhmm:
            case LCD_DATE_MMDDmmhh:
            case LCD_DATE_DDMMmmhh:
            {
                params.gdram[13] |= 0x01;//2
                params.gdram[10] |= 0x01;//4
                params.gdram[6] |= 0x01;//6
                break;
            }
        }
    }
	else if(channel == LCD_WINDOW_SUB)
	{
        //小数点处理
        params.gdram[11] &= ~0x10;
        params.gdram[10] &= ~0x10;
        params.gdram[8] &= ~0x10;
        params.gdram[31] &= ~0x10;
        params.gdram[6] &= ~0x10;
        params.gdram[4] &= ~0x10;
        params.gdram[2] &= ~0x10;
		
        //数字处理
        for(tubes=0; tubes<8; tubes++)
        {
            for(bits=0; bits<7; bits++)
            {
                params.gdram[matrix_sub[tubes][bits] / 8] &= ~(1 << (matrix_sub[tubes][bits] % 8));
            }
        }
        
        //时间处理
        if(index_year < 7)
        {
            number = tim->tm_year + 1900;
            for(tubes=index_year; tubes<(index_year + length_year); tubes++)
            {
                for(bits=0; bits<7; bits++)
                {
                    if((digitals[number % 10] >> bits) & 0x01)
                    {
                        params.gdram[matrix_sub[tubes][bits] / 8] |= (1 << (matrix_sub[tubes][bits] % 8));
                    }
                }
                number = number / 10;
            }
        }
        
        if(index_mon < 7)
        {
            number = tim->tm_mon + 1;
            for(tubes=index_mon; tubes<(index_mon + 2); tubes++)
            {
                for(bits=0; bits<7; bits++)
                {
                    if((digitals[number % 10] >> bits) & 0x01)
                    {
                        params.gdram[matrix_sub[tubes][bits] / 8] |= (1 << (matrix_sub[tubes][bits] % 8));
                    }
                }
                number = number / 10;
            }
        }
        
        if(index_day < 7)
        {
            number = tim->tm_mday;
            for(tubes=index_day; tubes<(index_day + 2); tubes++)
            {
                for(bits=0; bits<7; bits++)
                {
                    if((digitals[number % 10] >> bits) & 0x01)
                    {
                        params.gdram[matrix_sub[tubes][bits] / 8] |= (1 << (matrix_sub[tubes][bits] % 8));
                    }
                }
                number = number / 10;
            }
        }
        
        if(index_hour < 7)
        {
            number = tim->tm_hour;
            for(tubes=index_hour; tubes<(index_hour + 2); tubes++)
            {
                for(bits=0; bits<7; bits++)
                {
                    if((digitals[number % 10] >> bits) & 0x01)
                    {
                        params.gdram[matrix_sub[tubes][bits] / 8] |= (1 << (matrix_sub[tubes][bits] % 8));
                    }
                }
                number = number / 10;
            }
        }
        
        if(index_min < 7)
        {
            number = tim->tm_min;
            for(tubes=index_min; tubes<(index_min + 2); tubes++)
            {
                for(bits=0; bits<7; bits++)
                {
                    if((digitals[number % 10] >> bits) & 0x01)
                    {
                        params.gdram[matrix_sub[tubes][bits] / 8] |= (1 << (matrix_sub[tubes][bits] % 8));
                    }
                }
                number = number / 10;
            }
        }
        
        if(index_sec < 7)
        {
            number = tim->tm_sec;
            for(tubes=index_sec; tubes<(index_sec + 2); tubes++)
            {
                for(bits=0; bits<7; bits++)
                {
                    if((digitals[number % 10] >> bits) & 0x01)
                    {
                        params.gdram[matrix_sub[tubes][bits] / 8] |= (1 << (matrix_sub[tubes][bits] % 8));
                    }
                }
                number = number / 10;
            }
        }
        
        switch(theme)
        {
            case LCD_DATE_YYMMDD:
            {
                params.gdram[29] |= 0x10;//2
                params.gdram[6] |= 0x10;//4
                break;
            }
            case LCD_DATE_DDMMYY:
            case LCD_DATE_MMDDYY:
            {
                params.gdram[6] |= 0x10;//4
				params.gdram[31] |= 0x10;//6
                break;
            }
            case LCD_DATE_hhmmss:
            case LCD_DATE_ssmmhh:
            case LCD_DATE_mmsshh:
            {
                params.gdram[29] |= 0x10;//2
                params.gdram[6] |= 0x10;//4
                break;
            }
            case LCD_DATE_MMDDhhmm:
            case LCD_DATE_DDMMhhmm:
            case LCD_DATE_MMDDmmhh:
            case LCD_DATE_DDMMmmhh:
            {
                params.gdram[29] |= 0x10;//2
                params.gdram[6] |= 0x10;//4
				params.gdram[31] |= 0x10;//6
                break;
            }
        }
	}
	
    if(!params.flush)
    {
        if(memcmp(gdram, params.gdram, sizeof(gdram)))
        {
            params.flush = 0xff;
        }
    }
#endif
    
#endif
}

/**
  * @brief  
  */
static uint8_t window_show_msg(uint8_t channel, uint8_t from, const char *msg)
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
    
#if defined (DEMO_STM32F091)
    uint8_t tubes, bits, strl, index, points;
	uint8_t gdram[GDRAM_SIZE];
    
	if(channel > LCD_MAX_WINDOWS)
	{
		return(0);
	}
	
	params.global = LCD_SHOW_NORMAL;
	
	if(!params.flush)
	{
		memcpy(gdram, params.gdram, sizeof(gdram));
	}
    
    if(from > 7)
    {
        from = 7;
    }
    
    strl = strlen(msg);
    
    points = 0;
    for(tubes=0; tubes<strl; tubes++)
    {
        if(msg[tubes] == '.')
        {
            points += 1;
            
            if(tubes > (from + 1 + points))
            {
                strl = (from + 1 + points);
                break;
            }
        }
    }

    if(channel == LCD_WINDOW_MAIN)
    {
        //小数点处理
        params.gdram[28] &= ~0x04;//冒号1
        params.gdram[26] &= ~0x08;//冒号2
        
        params.gdram[15] &= ~0x01;
        params.gdram[13] &= ~0x01;
        params.gdram[11] &= ~0x01;
        params.gdram[10] &= ~0x01;
        params.gdram[8] &= ~0x01;
        params.gdram[6] &= ~0x01;
        params.gdram[4] &= ~0x01;
        
        //数字处理
        for(tubes=0; tubes<8; tubes++)
        {
            for(bits=0; bits<7; bits++)
            {
                params.gdram[matrix_main[tubes][bits] / 8] &= ~(1 << (matrix_main[tubes][bits] % 8));
            }
        }
        
        //单位处理
        params.gdram[18] &= ~0x02;//M
        params.gdram[19] &= ~0x02;//K
        params.gdram[20] &= ~0x06;//VV
        params.gdram[20] &= ~0x08;//A
        params.gdram[21] &= ~0x08;//H
        
        params.gdram[17] &= ~0x01;//M
        params.gdram[18] &= ~0x01;//K
        params.gdram[19] &= ~0x01;//var
        params.gdram[20] &= ~0x01;//H
        
        params.gdram[18] &= ~0x04;
        params.gdram[19] &= ~0x04;//x1000
        params.gdram[24] &= ~0x08;//Hz
        
        //符号处理
        params.gdram[32] &= ~0x02;
        
        //数字处理
        index = 0;
        
        for(tubes=from; index < strl;)
        {
            if((msg[index] >= '0') && (msg[index] <= '9'))
            {
                for(bits=0; bits<7; bits++)
                {
                    if((digitals[msg[index] - '0'] >> bits) & 0x01)
                    {
                        params.gdram[matrix_main[tubes][bits] / 8] |= (1 << (matrix_main[tubes][bits] % 8));
                    }
                }
                tubes --;
            }
            else if((msg[index] >= 'A') && (msg[index] <= 'F'))
            {
                for(bits=0; bits<7; bits++)
                {
                    if((digitals[msg[index] - 'A' + 10] >> bits) & 0x01)
                    {
                        params.gdram[matrix_main[tubes][bits] / 8] |= (1 << (matrix_main[tubes][bits] % 8));
                    }
                }
                tubes --;
            }
            else if((msg[index] >= 'a') && (msg[index] <= 'f'))
            {
                for(bits=0; bits<7; bits++)
                {
                    if((digitals[msg[index] - 'a' + 10] >> bits) & 0x01)
                    {
                        params.gdram[matrix_main[tubes][bits] / 8] |= (1 << (matrix_main[tubes][bits] % 8));
                    }
                }
                tubes --;
            }
            else if(msg[index] == '.')
            {
                switch(tubes + 1)
                {
                    case 1:params.gdram[15] |= 0x01; break;
                    case 2:params.gdram[13] |= 0x01; break;
                    case 3:params.gdram[11] |= 0x01; break;
                    case 4:params.gdram[10] |= 0x01; break;
                    case 5:params.gdram[8] |= 0x01; break;
                    case 6:params.gdram[6] |= 0x01; break;
                    case 7:params.gdram[4] |= 0x01; break;
                }
            }
            else
            {
                if(msg[index] == 'H')
                {
                    for(bits=0; bits<7; bits++)
                    {
                        if((digitals[DIGIT_H] >> bits) & 0x01)
                        {
                            params.gdram[matrix_main[tubes][bits] / 8] |= (1 << (matrix_main[tubes][bits] % 8));
                        }
                    }
                }
                else if(msg[index] == 'h')
                {
                    for(bits=0; bits<7; bits++)
                    {
                        if((digitals[DIGIT_h] >> bits) & 0x01)
                        {
                            params.gdram[matrix_main[tubes][bits] / 8] |= (1 << (matrix_main[tubes][bits] % 8));
                        }
                    }
                }
                else if(msg[index] == 'L')
                {
                    for(bits=0; bits<7; bits++)
                    {
                        if((digitals[DIGIT_L] >> bits) & 0x01)
                        {
                            params.gdram[matrix_main[tubes][bits] / 8] |= (1 << (matrix_main[tubes][bits] % 8));
                        }
                    }
                }
                else if(msg[index] == 'n')
                {
                    for(bits=0; bits<7; bits++)
                    {
                        if((digitals[DIGIT_n] >> bits) & 0x01)
                        {
                            params.gdram[matrix_main[tubes][bits] / 8] |= (1 << (matrix_main[tubes][bits] % 8));
                        }
                    }
                }
                else if(msg[index] == 'N')
                {
                    for(bits=0; bits<7; bits++)
                    {
                        if((digitals[DIGIT_N] >> bits) & 0x01)
                        {
                            params.gdram[matrix_main[tubes][bits] / 8] |= (1 << (matrix_main[tubes][bits] % 8));
                        }
                    }
                }
                else if(msg[index] == 'o')
                {
                    for(bits=0; bits<7; bits++)
                    {
                        if((digitals[DIGIT_o] >> bits) & 0x01)
                        {
                            params.gdram[matrix_main[tubes][bits] / 8] |= (1 << (matrix_main[tubes][bits] % 8));
                        }
                    }
                }
                else if(msg[index] == 'P')
                {
                    for(bits=0; bits<7; bits++)
                    {
                        if((digitals[DIGIT_P] >> bits) & 0x01)
                        {
                            params.gdram[matrix_main[tubes][bits] / 8] |= (1 << (matrix_main[tubes][bits] % 8));
                        }
                    }
                }
                else if(msg[index] == 'r')
                {
                    for(bits=0; bits<7; bits++)
                    {
                        if((digitals[DIGIT_r] >> bits) & 0x01)
                        {
                            params.gdram[matrix_main[tubes][bits] / 8] |= (1 << (matrix_main[tubes][bits] % 8));
                        }
                    }
                }
                else if(msg[index] == 't')
                {
                    for(bits=0; bits<7; bits++)
                    {
                        if((digitals[DIGIT_t] >> bits) & 0x01)
                        {
                            params.gdram[matrix_main[tubes][bits] / 8] |= (1 << (matrix_main[tubes][bits] % 8));
                        }
                    }
                }
                else if(msg[index] == 'U')
                {
                    for(bits=0; bits<7; bits++)
                    {
                        if((digitals[DIGIT_U] >> bits) & 0x01)
                        {
                            params.gdram[matrix_main[tubes][bits] / 8] |= (1 << (matrix_main[tubes][bits] % 8));
                        }
                    }
                }
                else if(msg[index] == ' ')
                {
                }
                else
                {
                    for(bits=0; bits<7; bits++)
                    {
                        if((digitals[DIGIT_INVALID] >> bits) & 0x01)
                        {
                            params.gdram[matrix_main[tubes][bits] / 8] |= (1 << (matrix_main[tubes][bits] % 8));
                        }
                    }
                }
                tubes --;
            }
            
            index += 1;
        }
    }
	else if(channel == LCD_WINDOW_SUB)
	{
        //小数点处理
        params.gdram[11] &= ~0x10;
        params.gdram[10] &= ~0x10;
        params.gdram[8] &= ~0x10;
        params.gdram[31] &= ~0x10;
        params.gdram[6] &= ~0x10;
        params.gdram[4] &= ~0x10;
        params.gdram[2] &= ~0x10;
		
        //数字处理
        for(tubes=0; tubes<8; tubes++)
        {
            for(bits=0; bits<7; bits++)
            {
                params.gdram[matrix_sub[tubes][bits] / 8] &= ~(1 << (matrix_sub[tubes][bits] % 8));
            }
        }
        
        //数字处理
        index = 0;
        
        for(tubes=from; index < strl;)
        {
            if((msg[index] >= '0') && (msg[index] <= '9'))
            {
                for(bits=0; bits<7; bits++)
                {
                    if((digitals[msg[index] - '0'] >> bits) & 0x01)
                    {
                        params.gdram[matrix_sub[tubes][bits] / 8] |= (1 << (matrix_sub[tubes][bits] % 8));
                    }
                }
                tubes --;
            }
            else if((msg[index] >= 'A') && (msg[index] <= 'F'))
            {
                for(bits=0; bits<7; bits++)
                {
                    if((digitals[msg[index] - 'A' + 10] >> bits) & 0x01)
                    {
                        params.gdram[matrix_sub[tubes][bits] / 8] |= (1 << (matrix_sub[tubes][bits] % 8));
                    }
                }
                tubes --;
            }
            else if((msg[index] >= 'a') && (msg[index] <= 'f'))
            {
                for(bits=0; bits<7; bits++)
                {
                    if((digitals[msg[index] - 'a' + 10] >> bits) & 0x01)
                    {
                        params.gdram[matrix_sub[tubes][bits] / 8] |= (1 << (matrix_sub[tubes][bits] % 8));
                    }
                }
                tubes --;
            }
            else if(msg[index] == '.')
            {
                switch(tubes + 1)
                {
                    case 1:params.gdram[27] |= 0x10; break;
                    case 2:params.gdram[29] |= 0x10; break;
                    case 3:params.gdram[8] |= 0x10; break;
                    case 4:params.gdram[6] |= 0x10; break;
                    case 5:params.gdram[4] |= 0x10; break;
                    case 6:params.gdram[31] |= 0x10; break;
                    case 7:params.gdram[2] |= 0x10; break;
                }
            }
            else
            {
                if(msg[index] == 'H')
                {
                    for(bits=0; bits<7; bits++)
                    {
                        if((digitals[DIGIT_H] >> bits) & 0x01)
                        {
                            params.gdram[matrix_sub[tubes][bits] / 8] |= (1 << (matrix_sub[tubes][bits] % 8));
                        }
                    }
                }
                else if(msg[index] == 'h')
                {
                    for(bits=0; bits<7; bits++)
                    {
                        if((digitals[DIGIT_h] >> bits) & 0x01)
                        {
                            params.gdram[matrix_sub[tubes][bits] / 8] |= (1 << (matrix_sub[tubes][bits] % 8));
                        }
                    }
                }
                else if(msg[index] == 'L')
                {
                    for(bits=0; bits<7; bits++)
                    {
                        if((digitals[DIGIT_L] >> bits) & 0x01)
                        {
                            params.gdram[matrix_sub[tubes][bits] / 8] |= (1 << (matrix_sub[tubes][bits] % 8));
                        }
                    }
                }
                else if(msg[index] == 'n')
                {
                    for(bits=0; bits<7; bits++)
                    {
                        if((digitals[DIGIT_n] >> bits) & 0x01)
                        {
                            params.gdram[matrix_sub[tubes][bits] / 8] |= (1 << (matrix_sub[tubes][bits] % 8));
                        }
                    }
                }
                else if(msg[index] == 'N')
                {
                    for(bits=0; bits<7; bits++)
                    {
                        if((digitals[DIGIT_N] >> bits) & 0x01)
                        {
                            params.gdram[matrix_sub[tubes][bits] / 8] |= (1 << (matrix_sub[tubes][bits] % 8));
                        }
                    }
                }
                else if(msg[index] == 'o')
                {
                    for(bits=0; bits<7; bits++)
                    {
                        if((digitals[DIGIT_o] >> bits) & 0x01)
                        {
                            params.gdram[matrix_sub[tubes][bits] / 8] |= (1 << (matrix_sub[tubes][bits] % 8));
                        }
                    }
                }
                else if(msg[index] == 'P')
                {
                    for(bits=0; bits<7; bits++)
                    {
                        if((digitals[DIGIT_P] >> bits) & 0x01)
                        {
                            params.gdram[matrix_sub[tubes][bits] / 8] |= (1 << (matrix_sub[tubes][bits] % 8));
                        }
                    }
                }
                else if(msg[index] == 'r')
                {
                    for(bits=0; bits<7; bits++)
                    {
                        if((digitals[DIGIT_r] >> bits) & 0x01)
                        {
                            params.gdram[matrix_sub[tubes][bits] / 8] |= (1 << (matrix_sub[tubes][bits] % 8));
                        }
                    }
                }
                else if(msg[index] == 't')
                {
                    for(bits=0; bits<7; bits++)
                    {
                        if((digitals[DIGIT_t] >> bits) & 0x01)
                        {
                            params.gdram[matrix_sub[tubes][bits] / 8] |= (1 << (matrix_sub[tubes][bits] % 8));
                        }
                    }
                }
                else if(msg[index] == 'U')
                {
                    for(bits=0; bits<7; bits++)
                    {
                        if((digitals[DIGIT_U] >> bits) & 0x01)
                        {
                            params.gdram[matrix_sub[tubes][bits] / 8] |= (1 << (matrix_sub[tubes][bits] % 8));
                        }
                    }
                }
                else if(msg[index] == ' ')
                {
                }
                else
                {
                    for(bits=0; bits<7; bits++)
                    {
                        if((digitals[DIGIT_INVALID] >> bits) & 0x01)
                        {
                            params.gdram[matrix_sub[tubes][bits] / 8] |= (1 << (matrix_sub[tubes][bits] % 8));
                        }
                    }
                }
                tubes --;
            }
            
            index += 1;
        }
	}
	
    if(!params.flush)
    {
        if(memcmp(gdram, params.gdram, sizeof(gdram)))
        {
            params.flush = 0xff;
        }
    }
    
    return(strl);
#endif
    
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
    
#if defined (DEMO_STM32F091)
    uint8_t tubes, bits;
	uint8_t gdram[GDRAM_SIZE];
    
	if(channel > LCD_MAX_WINDOWS)
	{
		return;
	}
	
	params.global = LCD_SHOW_NORMAL;
	
	if(!params.flush)
	{
		memcpy(gdram, params.gdram, sizeof(gdram));
	}

    if(channel == LCD_WINDOW_MAIN)
    {
        //小数点处理
        params.gdram[28] &= ~0x04;//冒号1
        params.gdram[26] &= ~0x08;//冒号2
        
        params.gdram[15] &= ~0x01;
        params.gdram[13] &= ~0x01;
        params.gdram[11] &= ~0x01;
        params.gdram[10] &= ~0x01;
        params.gdram[8] &= ~0x01;
        params.gdram[6] &= ~0x01;
        params.gdram[4] &= ~0x01;
        
        //数字处理
        for(tubes=0; tubes<8; tubes++)
        {
            for(bits=0; bits<7; bits++)
            {
                params.gdram[matrix_main[tubes][bits] / 8] &= ~(1 << (matrix_main[tubes][bits] % 8));
            }
        }
        
        //单位处理
        params.gdram[18] &= ~0x02;//M
        params.gdram[19] &= ~0x02;//K
        params.gdram[20] &= ~0x06;//VV
        params.gdram[20] &= ~0x08;//A
        params.gdram[21] &= ~0x08;//H
        
        params.gdram[17] &= ~0x01;//M
        params.gdram[18] &= ~0x01;//K
        params.gdram[19] &= ~0x01;//var
        params.gdram[20] &= ~0x01;//H
        
        params.gdram[18] &= ~0x04;
        params.gdram[19] &= ~0x04;//x1000
        params.gdram[24] &= ~0x08;//Hz
        
        //符号处理
        params.gdram[32] &= ~0x02;
    }
	else if(channel == LCD_WINDOW_SUB)
	{
        //小数点处理
        params.gdram[11] &= ~0x10;
        params.gdram[10] &= ~0x10;
        params.gdram[8] &= ~0x10;
        params.gdram[31] &= ~0x10;
        params.gdram[6] &= ~0x10;
        params.gdram[4] &= ~0x10;
        params.gdram[2] &= ~0x10;
		
        //数字处理
        for(tubes=0; tubes<8; tubes++)
        {
            for(bits=0; bits<7; bits++)
            {
                params.gdram[matrix_sub[tubes][bits] / 8] &= ~(1 << (matrix_sub[tubes][bits] % 8));
            }
        }
	}
	
    if(!params.flush)
    {
        if(memcmp(gdram, params.gdram, sizeof(gdram)))
        {
            params.flush = 0xff;
        }
    }
#endif
    
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
    
#if defined (DEMO_STM32F091)
    uint8_t tubes, bits;
	uint8_t gdram[GDRAM_SIZE];
    
	if(channel > LCD_MAX_WINDOWS)
	{
		return;
	}
	
	params.global = LCD_SHOW_NORMAL;
	
	if(!params.flush)
	{
		memcpy(gdram, params.gdram, sizeof(gdram));
	}

    if(channel == LCD_WINDOW_MAIN)
    {
        //小数点处理
        params.gdram[28] |= 0x04;//冒号1
        params.gdram[26] |= 0x08;//冒号2
        
        params.gdram[15] |= 0x01;
        params.gdram[13] |= 0x01;
        params.gdram[11] |= 0x01;
        params.gdram[10] |= 0x01;
        params.gdram[8] |= 0x01;
        params.gdram[6] |= 0x01;
        params.gdram[4] |= 0x01;
        
        //数字处理
        for(tubes=0; tubes<8; tubes++)
        {
            for(bits=0; bits<7; bits++)
            {
                params.gdram[matrix_main[tubes][bits] / 8] |= (1 << (matrix_main[tubes][bits] % 8));
            }
        }
        
        //单位处理
        params.gdram[18] |= 0x02;//M
        params.gdram[19] |= 0x02;//K
        params.gdram[20] |= 0x06;//VV
        params.gdram[20] |= 0x08;//A
        params.gdram[21] |= 0x08;//H
        
        params.gdram[17] |= 0x01;//M
        params.gdram[18] |= 0x01;//K
        params.gdram[19] |= 0x01;//var
        params.gdram[20] |= 0x01;//H
        
        params.gdram[18] |= 0x04;
        params.gdram[19] |= 0x04;//x1000
        params.gdram[24] |= 0x08;//Hz
        
        //符号处理
        params.gdram[32] |= 0x02;
    }
	else if(channel == LCD_WINDOW_SUB)
	{
        //小数点处理
        params.gdram[11] |= 0x10;
        params.gdram[10] |= 0x10;
        params.gdram[8] |= 0x10;
        params.gdram[31] |= 0x10;
        params.gdram[6] |= 0x10;
        params.gdram[4] |= 0x10;
        params.gdram[2] |= 0x10;
		
        //数字处理
        for(tubes=0; tubes<8; tubes++)
        {
            for(bits=0; bits<7; bits++)
            {
                params.gdram[matrix_sub[tubes][bits] / 8] |= (1 << (matrix_sub[tubes][bits] % 8));
            }
        }
	}
	
    if(!params.flush)
    {
        if(memcmp(gdram, params.gdram, sizeof(gdram)))
        {
            params.flush = 0xff;
        }
    }
#endif
    
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
    
    params.global = LCD_SHOW_NORMAL;
    
    params.blink[channel / 8] &= ~(1 << (channel & 8));
    
	if(!params.flush)
	{
		memcpy(gdram, params.gdram, sizeof(gdram));
	}
    
    switch(channel)
    {
        case LCD_LABEL_U1:
        {
            params.gdram[16] |= 0x40;
            break;
        }
        case LCD_LABEL_U2:
        {
            params.gdram[17] |= 0x40;
            break;
        }
        case LCD_LABEL_U3:
        {
            params.gdram[17] |= 0x20;
            break;
        }
        case LCD_LABEL_I1:
        {
            if(state)
            {
                params.gdram[25] |= 0x60;
            }
            else
            {
                params.gdram[25] |= 0x20;
            }
            break;
        }
		case LCD_LABEL_I2:
        {
            if(state)
            {
                params.gdram[24] |= 0x60;
            }
            else
            {
                params.gdram[24] |= 0x20;
            }
            break;
        }
		case LCD_LABEL_I3:
        {
            if(state)
            {
                params.gdram[23] |= 0xc0;
            }
            else
            {
                params.gdram[23] |= 0x40;
            }
            break;
        }
		case LCD_LABEL_L1:
        {
            params.gdram[30] |= 0x08;
            break;
        }
		case LCD_LABEL_L2:
        {
            params.gdram[30] |= 0x04;
            break;
        }
		case LCD_LABEL_L3:
        {
            params.gdram[30] |= 0x02;
            break;
        }
		case LCD_LABEL_LN:
        {
            params.gdram[30] |= 0x01;
            break;
        }
		case LCD_LABEL_PF:
        {
            params.gdram[29] |= 0x01;
            break;
        }
		case LCD_LABEL_SIGNAL:
        {
            params.gdram[18] &= ~0x81;
            params.gdram[24] &= ~0x10;
            params.gdram[25] &= ~0x81;
            
            if(state < 20)
            {
                params.gdram[18] |= 0x10;
            }
            else if(state < 40)
            {
                params.gdram[18] |= 0x10;
                params.gdram[18] |= 0x08;
            }
            else if(state < 60)
            {
                params.gdram[18] |= 0x10;
                params.gdram[18] |= 0x08;
                params.gdram[25] |= 0x08;
            }
            else if(state < 80)
            {
                params.gdram[18] |= 0x10;
                params.gdram[18] |= 0x08;
                params.gdram[25] |= 0x08;
                params.gdram[25] |= 0x10;
            }
            else if(state < 100)
            {
                params.gdram[18] |= 0x10;
                params.gdram[18] |= 0x08;
                params.gdram[25] |= 0x08;
                params.gdram[25] |= 0x10;
                params.gdram[24] |= 0x10;
            }
            break;
        }
		case LCD_LABEL_COMM:
        {
            params.gdram[23] |= 0x30;
            break;
        }
		case LCD_LABEL_QUAD:
        {
            params.gdram[21] &= ~0xe0;
            params.gdram[22] &= ~0xc0;
            params.gdram[21] |= 0x40;
            switch(state)
            {
                case 1:
                {
                    params.gdram[21] |= 0x80;
                    break;
                }
                case 2:
                {
                    params.gdram[22] |= 0x80;
                    break;
                }
                case 3:
                {
                    params.gdram[22] |= 0x40;
                    break;
                }
                case 4:
                {
                    params.gdram[21] |= 0x20;
                    break;
                }
            }
            break;
        }
		case LCD_LABEL_RATE:
        {
            params.gdram[1] &= ~0x0c;
            params.gdram[2] &= ~0x0c;
            switch(state)
            {
                case 1:
                {
                    params.gdram[1] |= 0x08;
                    break;
                }
                case 2:
                {
                    params.gdram[2] |= 0x08;
                    break;
                }
                case 3:
                {
                    params.gdram[1] |= 0x04;
                    break;
                }
                case 4:
                {
                    params.gdram[2] |= 0x04;
                    break;
                }
            }
            break;
        }
		case LCD_LABEL_BATRTC:
        {
            params.gdram[16] |= 0x10;
            break;
        }
		case LCD_LABEL_BATBAK:
        {
            params.gdram[17] |= 0x10;
            break;
        }
		case LCD_LABEL_DATE:
        {
            params.gdram[26] |= 0x10;
            break;
        }
		case LCD_LABEL_TIM:
        {
            params.gdram[28] |= 0x08;
            break;
        }
		case LCD_LABEL_PRIM:
        {
            params.gdram[31] |= 0x08;
            break;
        }
		case LCD_LABEL_SECOND:
        {
            params.gdram[31] |= 0x04;
            break;
        }
		case LCD_LABEL_NET:
        {
            params.gdram[32] |= 0x08;
            break;
        }
		case LCD_LABEL_AUX:
        {
            params.gdram[32] |= 0x04;
            break;
        }
		case LCD_LABEL_PULSE:
        {
            params.gdram[22] |= 0x20;
            break;
        }
    }
    
    if(!params.flush)
    {
        if(memcmp(gdram, params.gdram, sizeof(gdram)))
        {
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
    
    params.global = LCD_SHOW_NORMAL;
    
    params.blink[channel / 8] &= ~(1 << (channel & 8));
    
	if(!params.flush)
	{
		memcpy(gdram, params.gdram, sizeof(gdram));
	}
    
    switch(channel)
    {
        case LCD_LABEL_U1:
        {
            params.gdram[16] &= ~0x40;
            break;
        }
        case LCD_LABEL_U2:
        {
            params.gdram[17] &= ~0x40;
            break;
        }
        case LCD_LABEL_U3:
        {
            params.gdram[17] &= ~0x20;
            break;
        }
        case LCD_LABEL_I1:
        {
            params.gdram[25] &= ~0x60;
            break;
        }
		case LCD_LABEL_I2:
        {
            params.gdram[24] &= ~0x60;
            break;
        }
		case LCD_LABEL_I3:
        {
            params.gdram[23] &= ~0xc0;
            break;
        }
		case LCD_LABEL_L1:
        {
            params.gdram[30] &= ~0x08;
            break;
        }
		case LCD_LABEL_L2:
        {
            params.gdram[30] &= ~0x04;
            break;
        }
		case LCD_LABEL_L3:
        {
            params.gdram[30] &= ~0x02;
            break;
        }
		case LCD_LABEL_LN:
        {
            params.gdram[30] &= ~0x01;
            break;
        }
		case LCD_LABEL_PF:
        {
            params.gdram[29] &= ~0x01;
            break;
        }
		case LCD_LABEL_SIGNAL:
        {
            params.gdram[18] &= ~0x81;
            params.gdram[24] &= ~0x10;
            params.gdram[25] &= ~0x81;
            break;
        }
		case LCD_LABEL_COMM:
        {
            params.gdram[23] &= ~0x30;
            break;
        }
		case LCD_LABEL_QUAD:
        {
            params.gdram[21] &= ~0xe0;
            params.gdram[22] &= ~0xc0;
            break;
        }
		case LCD_LABEL_RATE:
        {
            params.gdram[1] &= ~0x0c;
            params.gdram[2] &= ~0x0c;
            break;
        }
		case LCD_LABEL_BATRTC:
        {
            params.gdram[16] &= ~0x10;
            break;
        }
		case LCD_LABEL_BATBAK:
        {
            params.gdram[17] &= ~0x10;
            break;
        }
		case LCD_LABEL_DATE:
        {
            params.gdram[26] &= ~0x10;
            break;
        }
		case LCD_LABEL_TIM:
        {
            params.gdram[28] &= ~0x08;
            break;
        }
		case LCD_LABEL_PRIM:
        {
            params.gdram[31] &= ~0x08;
            break;
        }
		case LCD_LABEL_SECOND:
        {
            params.gdram[31] &= ~0x04;
            break;
        }
		case LCD_LABEL_NET:
        {
            params.gdram[32] &= ~0x08;
            break;
        }
		case LCD_LABEL_AUX:
        {
            params.gdram[32] &= ~0x04;
            break;
        }
		case LCD_LABEL_PULSE:
        {
            params.gdram[22] &= ~0x20;
            break;
        }
    }
    
    if(!params.flush)
    {
        if(memcmp(gdram, params.gdram, sizeof(gdram)))
        {
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
    
    params.global = LCD_SHOW_NORMAL;
    
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
