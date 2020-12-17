/**
 * @brief		
 * @details		
 * @date		2017-01-09
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "task_display.h"
#include "types_display.h"
#include "config_display.h"

#include "lcd.h"

#include "mids.h"
#include "types_protocol.h"
#include "axdr.h"
#include "string.h"

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief  数据项标识
  */
struct __disp_entry
{
    //对于DLMS来说，一个显示项就是一个 __cosem_descriptor
    uint8_t                 descriptor[16];
};

/**
  * @brief  显示列表参数
  */
struct __disp_list
{
    //有效数据项个数
	uint8_t					amount;
    
    //每个显示列表最多支持128个数据项
	struct __disp_entry		entry[DISP_CONF_AMOUNT_IN_LIST];
};

/**
  * @brief  时间参数
  */
struct __disp_time
{
    //上电或唤醒时的屏显时间
    uint8_t                 start;
    //循显数据项切换时间
    uint8_t                 scroll;
    //背光时间
    uint8_t                 backlight;
};

/**
  * @brief  计量数据小数点位数
  */
struct __disp_dot
{
    //功率
    uint32_t                power:4;
    //电压
    uint32_t                voltage:4;
    //电流
    uint32_t                current:4;
    //电能
    uint32_t                energy:4;
    //需量
    uint32_t                demand:4;
    //相角
    uint32_t                angle:4;
    //频率
    uint32_t                freq:4;
    //功率因数
    uint32_t                pf:4;
    
    //备用1
    uint32_t                reserve1:4;
    //备用2
    uint32_t                reserve2:4;
    //备用3
    uint32_t                reserve3:4;
    //备用4
    uint32_t                reserve4:4;
    //备用5
    uint32_t                reserve5:4;
    //备用6
    uint32_t                reserve6:4;
    //备用7
    uint32_t                reserve7:4;
    //备用8
    uint32_t                reserve8:4;
};





/**
  * @brief  显示任务EEPROM参数
  */
struct __disp_param
{
    //时间参数
    struct __disp_time      time;
    //计量数据小数点位数参数
    struct __disp_dot       dots;
    //显示数据项列表
	struct __disp_list		list[DISP_CONF_CHANNEL_AMOUNT];
};

/**
  * @brief  显示任务运行参数
  */
struct __disp_runs
{
    //计量数据小数点位数参数
    struct __disp_dot       dots;
    //时间参数
    struct __disp_time      time;
    //时间计数器
    struct __disp_time      counter;
    //当前显示列表
    uint8_t                 channel;
    //之前的显示列表
    uint8_t                 last_channel;
    //当前显示列表数据项数量
    uint8_t                 amount;
    //当前显示数据项索引
    uint8_t                 index;
    //当前数据项
    struct __disp_entry     entry;
    //轮询秒定时
    uint16_t loop_record;
    //轮询主屏幕数据刷新
    uint16_t loop_flush;
    //插入显示定时（毫秒）
    uint16_t insert_millisecond;
	//插入显示是否为数据项
	uint8_t                 is_entry;
    //插入的数据项
    struct __disp_entry		insert_entry;
};

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static enum __task_status status = TASK_NOTINIT;
static struct __disp_runs disp_runs;

/* Private function prototypes -----------------------------------------------*/
static void clear_unit(void);
static uint32_t pow_uint(uint8_t base, uint8_t exp);
static enum __lcd_unit select_unit(struct __meta_identifier id);
static void select_data(uint32_t mid, uint8_t *data);
static void flush_data(void);
static uint8_t display_list_show_index(uint8_t val);

/* Private functions ---------------------------------------------------------*/
static void clear_unit(void)
{
    lcd.label.off(LCD_LABEL_L1);
    lcd.label.off(LCD_LABEL_L2);
    lcd.label.off(LCD_LABEL_L3);
    lcd.label.off(LCD_LABEL_LN);
    lcd.label.off(LCD_LABEL_PF);
    lcd.label.off(LCD_LABEL_DATE);
    lcd.label.off(LCD_LABEL_TIM);
}

static enum __lcd_unit select_unit(struct __meta_identifier id)
{
	switch(id.phase)
	{
		case M_PHASE_A: //A相
		case M_PHASE_AB: //AB
			lcd.label.on(LCD_LABEL_L1, 0);
			break;
		case M_PHASE_B: //B相
		case M_PHASE_AC: //AC
			lcd.label.on(LCD_LABEL_L2, 0);
			break;
		case M_PHASE_C: //C相
		case M_PHASE_BC: //BC
			lcd.label.on(LCD_LABEL_L3, 0);
			break;
		case M_PHASE_N: //零相
			lcd.label.on(LCD_LABEL_LN, 0);
			break;
	}
	
	switch(id.item)
	{
		case M_P_POWER: //有功功率（mW）
			return(LCD_UNIT_W);
		case M_Q_POWER: //无功功率（mVar）
			return(LCD_UNIT_VAR);
		case M_S_POWER: //视在功率（mVA）
			return(LCD_UNIT_VA);
		case M_P_ENERGY: //有功电能（mWh）
			return(LCD_UNIT_WH);
		case M_Q_ENERGY: //无功电能（mVarh）
			return(LCD_UNIT_VARH);
		case M_S_ENERGY: //视在电能（mVAh）
			return(LCD_UNIT_VARH);
		case M_VOLTAGE: //电压（mV）
			return(LCD_UNIT_V);
		case M_CURRENT: //电流（mA）
			return(LCD_UNIT_A);
		case M_FREQUENCY: //频率（1/1000Hz）
			return(LCD_UNIT_HZ);
		case M_POWER_FACTOR: //功率因数（1/1000）
			lcd.label.on(LCD_LABEL_PF, 0);
			return(LCD_UNIT_NONE);
	}
	
	return(LCD_UNIT_NONE);
}

static uint32_t pow_uint(uint8_t base, uint8_t exp)
{
    uint8_t count;
    uint32_t result = 1;
    
    for(count=0; count<exp; count++)
    {
        result *= base;
    }
    
    return(result);
}

static void select_data(uint32_t mid, uint8_t *data)
{
    enum __axdr_type type;
    union __axdr_container container;
    struct __meta_identifier id;
    
    type = axdr.type.decode(data);
    
    if(AXDR_CONTAINABLE(type))
    {
        axdr.decode(data, 0, &container);
    }
    else
    {
        return;
    }
    
    M_U2ID(mid, id);
    
    if(M_UISID(mid))
    {
        if(M_UISVOLTAGE(mid))
        {
            if(disp_runs.dots.voltage > 3)
            {
                container.vi64 *= pow_uint(10, (disp_runs.dots.voltage - 3));
            }
            else
            {
                container.vi64 /= pow_uint(10, (3 - disp_runs.dots.voltage));
            }
            lcd.window.show.dec(LCD_WINDOW_MAIN, (int32_t)container.vi64, (enum __lcd_dot)disp_runs.dots.voltage, select_unit(id));
        }
        else if(M_UISCURRENT(mid))
        {
            if(disp_runs.dots.current > 3)
            {
                container.vi64 *= pow_uint(10, (disp_runs.dots.current - 3));
            }
            else
            {
                container.vi64 /= pow_uint(10, (3 - disp_runs.dots.current));
            }
            lcd.window.show.dec(LCD_WINDOW_MAIN, (int32_t)container.vi64, (enum __lcd_dot)disp_runs.dots.current, select_unit(id));
        }
        else if(M_UISPOWER(mid))
        {
            if(disp_runs.dots.power > 3)
            {
                container.vi64 *= pow_uint(10, (disp_runs.dots.power - 3));
            }
            else
            {
                container.vi64 /= pow_uint(10, (3 - disp_runs.dots.power));
            }
            lcd.window.show.dec(LCD_WINDOW_MAIN, (int32_t)container.vi64, (enum __lcd_dot)disp_runs.dots.power, select_unit(id));
        }
        else if(M_UISENERGY(mid))
        {
            if(disp_runs.dots.energy > 3)
            {
                container.vi64 *= pow_uint(10, (disp_runs.dots.energy - 3));
            }
            else
            {
                container.vi64 /= pow_uint(10, (3 - disp_runs.dots.energy));
            }
            lcd.window.show.dec(LCD_WINDOW_MAIN, (int32_t)container.vi64, (enum __lcd_dot)disp_runs.dots.energy, select_unit(id));
        }
        else if(M_UISPF(mid))
        {
            if(disp_runs.dots.energy > 3)
            {
                container.vi64 *= pow_uint(10, (disp_runs.dots.energy - 3));
            }
            else
            {
                container.vi64 /= pow_uint(10, (3 - disp_runs.dots.energy));
            }
            lcd.window.show.dec(LCD_WINDOW_MAIN, (int32_t)container.vi64, (enum __lcd_dot)disp_runs.dots.pf, select_unit(id));
        }
        else if(M_UISANGLE(mid))
        {
            if(disp_runs.dots.energy > 3)
            {
                container.vi64 *= pow_uint(10, (disp_runs.dots.energy - 3));
            }
            else
            {
                container.vi64 /= pow_uint(10, (3 - disp_runs.dots.energy));
            }
            lcd.window.show.dec(LCD_WINDOW_MAIN, (int32_t)container.vi64, (enum __lcd_dot)disp_runs.dots.angle, select_unit(id));
        }
        else if(M_UISFREQ(mid))
        {
            if(disp_runs.dots.energy > 3)
            {
                container.vi64 *= pow_uint(10, (disp_runs.dots.energy - 3));
            }
            else
            {
                container.vi64 /= pow_uint(10, (3 - disp_runs.dots.energy));
            }
            lcd.window.show.dec(LCD_WINDOW_MAIN, (int32_t)container.vi64, (enum __lcd_dot)disp_runs.dots.freq, select_unit(id));
        }
        else
        {
            lcd.window.show.dec(LCD_WINDOW_MAIN, (int32_t)container.vi64, LCD_DOT_NONE, select_unit(id));
        }
    }
    else if(M_UISFMT(mid))
    {
        switch(id.item)
        {
            case FMT_BIN: //二进制格式
                lcd.window.show.bin(LCD_WINDOW_MAIN, container.vu16, LCD_DOT_NONE, LCD_UNIT_NONE);
                break;
            case FMT_BCD: //...BCD格式
            case FMT_HEX: //十六进制格式
                lcd.window.show.hex(LCD_WINDOW_MAIN, container.vu32, LCD_DOT_NONE, LCD_UNIT_NONE);
                break;
            case FMT_DATE: //日期格式
                lcd.window.show.date(LCD_WINDOW_MAIN, container.vu64, LCD_DATE_YYMMDD);
                break;
            case FMT_TIME: //时间格式
                lcd.window.show.date(LCD_WINDOW_MAIN, container.vu64, LCD_DATE_hhmmss);
                break;
            case FMT_DTIME: //日期时间格式
                lcd.window.show.date(LCD_WINDOW_MAIN, container.vu64, LCD_DATE_MMDDhhmm);
                break;
            case FMT_ASCII: //字符串格式
            case FMT_STR:
                lcd.window.show.msg(LCD_WINDOW_MAIN, 7, (const char *)(data+2));
                break;
            default:
                lcd.window.show.dec(LCD_WINDOW_MAIN, container.vi32, LCD_DOT_NONE, LCD_UNIT_NONE);
                break;
        }
    }
    else
    {
        lcd.window.show.dec(LCD_WINDOW_MAIN, container.vi32, LCD_DOT_NONE, LCD_UNIT_NONE);
    }
}

static void flush_data(void)
{
    uint32_t mid;
    uint8_t buff[32];
    struct __protocol *api_stream;
    
	if(disp_runs.index >= disp_runs.amount)
	{
		disp_runs.index = 0;
	}
    
    clear_unit();
    
    api_stream = (struct __protocol *)api("task_protocol");
    
    if(!api_stream)
    {
        return;
    }
    
    if(!api_stream->read(PF_DLMS, disp_runs.entry.descriptor, buff, sizeof(buff), &mid))
    {
        return;
    }
    
    select_data(mid, buff);
    
    return;
}

static uint8_t display_change(uint8_t channel)
{
	uint32_t offset;
    
    if(channel >= DISP_CONF_CHANNEL_AMOUNT)
    {
        return(disp_runs.channel);
    }
    
    if(disp_runs.channel != DISP_CONF_CHANNEL_ALTERNATE)
    {
		disp_runs.last_channel = disp_runs.channel;
	}
	
    disp_runs.channel = channel;
    
    offset = STRUCT_OFFSET(struct __disp_param, list[disp_runs.channel].amount);
    file.parameter.read("display", offset, sizeof(disp_runs.amount), &disp_runs.amount);
    
    disp_runs.counter.scroll = 0;
    disp_runs.index = 0;
    display_list_show_index(0);
    
    return(disp_runs.channel);
}

static uint8_t display_channel(void)
{
    return(disp_runs.channel);
}

static uint8_t display_list_show_next(void)
{
    uint32_t offset;
    uint32_t mid;
    uint8_t buff[32];
    struct __protocol *api_stream;
    
    //显示项索引加一
    disp_runs.index += 1;
    
	if(disp_runs.index >= disp_runs.amount)
	{
		disp_runs.index = 0;
	}
    
    //滚屏计数清零
	disp_runs.counter.scroll = 0;
    
    //如果是键显并且当前系统状态为正常运行状态，打开背光
    if((disp_runs.channel == DISP_CONF_CHANNEL_ALTERNATE) && (system_status() == SYSTEM_RUN))
    {
        lcd.backlight.open();
        disp_runs.counter.backlight = disp_runs.time.backlight;
    }
    
    //清空当前的L1 L2 L3 LN PF 显示状态
    clear_unit();
    
    //获取显示项索引对应的数据项标识
    offset = STRUCT_OFFSET(struct __disp_param, list[disp_runs.channel].entry[disp_runs.index]);
    file.parameter.read("display", offset, sizeof(disp_runs.entry), &disp_runs.entry);
    
    api_stream = (struct __protocol *)api("task_protocol");
    if(!api_stream)
    {
        return(disp_runs.index);
    }
    
    if(!api_stream->read(PF_DLMS, disp_runs.entry.descriptor, buff, sizeof(buff), &mid))
    {
        return(disp_runs.index);
    }
    
    
    //根据获取到的数据和参数，刷新显示
    select_data(mid, buff);
    
    return(disp_runs.index);
}

static uint8_t display_list_show_last(void)
{
    uint32_t offset;
    uint32_t mid;
    uint8_t buff[32];
    struct __protocol *api_stream;
    
	if(disp_runs.index)
	{
        disp_runs.index -= 1;
	}
    else
    {
        disp_runs.index = disp_runs.amount - 1;
    }
    
	disp_runs.counter.scroll = 0;
    
    if((disp_runs.channel == DISP_CONF_CHANNEL_ALTERNATE) && (system_status() == SYSTEM_RUN))
    {
        lcd.backlight.open();
        disp_runs.counter.backlight = disp_runs.time.backlight;
    }
    
    clear_unit();
    
    offset = STRUCT_OFFSET(struct __disp_param, list[disp_runs.channel].entry[disp_runs.index]);
    file.parameter.read("display", offset, sizeof(disp_runs.entry), &disp_runs.entry);
    
    api_stream = (struct __protocol *)api("task_protocol");
    if(!api_stream)
    {
        return(disp_runs.index);
    }
	
    if(!api_stream->read(PF_DLMS, disp_runs.entry.descriptor, buff, sizeof(buff), &mid))
    {
        return(disp_runs.index);
    }
    
    select_data(mid, buff);
    
    return(disp_runs.index);
}

static uint8_t display_list_show_index(uint8_t val)
{
    uint32_t offset;
    uint32_t mid;
    uint8_t buff[32];
    struct __protocol *api_stream;
    
	if(val >= disp_runs.amount)
	{
        return(disp_runs.index);
	}
    
    disp_runs.index = val;
    
	disp_runs.counter.scroll = 0;
    
    if((disp_runs.channel == DISP_CONF_CHANNEL_ALTERNATE) && (system_status() == SYSTEM_RUN))
    {
        lcd.backlight.open();
        disp_runs.counter.backlight = disp_runs.time.backlight;
    }
    
    clear_unit();
    
    offset = STRUCT_OFFSET(struct __disp_param, list[disp_runs.channel].entry[disp_runs.index]);
    file.parameter.read("display", offset, sizeof(disp_runs.entry), &disp_runs.entry);
    
    api_stream = (struct __protocol *)api("task_protocol");
    if(!api_stream)
    {
        return(disp_runs.index);
    }
    
    if(!api_stream->read(PF_DLMS, disp_runs.entry.descriptor, buff, sizeof(buff), &mid))
    {
        return(disp_runs.index);
    }
    
    select_data(mid, buff);
    
    return(disp_runs.index);
}

static uint8_t display_list_current(void)
{
    return(disp_runs.index);
}

static uint8_t display_list_insert_message(uint8_t second, const char *msg)
{
	if(!second || !msg)
	{
		return(0);
	}
	
	if(second > 60)
	{
		second = 60;
	}
	
	disp_runs.insert_millisecond = second * 1000;
	
    if(system_status() == SYSTEM_RUN)
    {
        lcd.backlight.open();
    }
    
    clear_unit();
    
	lcd.show.none();
    lcd.window.show.msg(LCD_WINDOW_MAIN, 7, msg);
	
	return(second);
}

static uint8_t display_list_insert_instance(uint8_t second, void *id)
{
	if(!second || !id)
	{
		return(0);
	}
	
	if(second > 60)
	{
		second = 60;
	}
	
	disp_runs.insert_millisecond = second * 1000;
	heap.copy(&disp_runs.insert_entry, id, sizeof(disp_runs.insert_entry));
	
	return(second);
}



static uint8_t display_config_start_get(void)
{
    return(disp_runs.time.start);
}

static uint8_t display_config_start_set(uint8_t second)
{
	uint32_t offset;
    
    disp_runs.time.start = second;
	offset = STRUCT_OFFSET(struct __disp_param, time.start);
	file.parameter.write("display", offset, sizeof(disp_runs.time.start), &disp_runs.time.start);
    
    return(disp_runs.time.start);
}

static uint8_t display_config_scroll_get(void)
{
    return(disp_runs.time.scroll);
}

static uint8_t display_config_scroll_set(uint8_t second)
{
	uint32_t offset;
    
    disp_runs.time.scroll = second;
	offset = STRUCT_OFFSET(struct __disp_param, time.scroll);
	file.parameter.write("display", offset, sizeof(disp_runs.time.scroll), &disp_runs.time.scroll);
    
    return(disp_runs.time.scroll);
}

static uint8_t display_config_backlight_get(void)
{
    return(disp_runs.time.backlight);
}

static uint8_t display_config_backlight_set(uint8_t second)
{
	uint32_t offset;
    
    disp_runs.time.backlight = second;
	offset = STRUCT_OFFSET(struct __disp_param, time.backlight);
	file.parameter.write("display", offset, sizeof(disp_runs.time.backlight), &disp_runs.time.backlight);
    
    return(disp_runs.time.backlight);
}

static uint8_t display_config_power_dot_get(void)
{
    return((uint8_t)disp_runs.dots.power);
}

static uint8_t display_config_power_dot_set(uint8_t dot)
{
	uint32_t offset;
    struct __disp_dot dots;
    
	offset = STRUCT_OFFSET(struct __disp_param, dots);
    file.parameter.read("display", offset, sizeof(disp_runs.dots), &dots);
    disp_runs.dots.power = dot;
	file.parameter.write("display", offset, sizeof(disp_runs.dots), &dots);
    
    return((uint8_t)disp_runs.dots.power);
}


static uint8_t display_config_voltage_dot_get(void)
{
    return((uint8_t)disp_runs.dots.voltage);
}

static uint8_t display_config_voltage_dot_set(uint8_t dot)
{
	uint32_t offset;
    struct __disp_dot dots;
    
	offset = STRUCT_OFFSET(struct __disp_param, dots);
    file.parameter.read("display", offset, sizeof(disp_runs.dots), &dots);
    disp_runs.dots.voltage = dot;
	file.parameter.write("display", offset, sizeof(disp_runs.dots), &dots);
    
    return((uint8_t)disp_runs.dots.voltage);
}

static uint8_t display_config_current_dot_get(void)
{
    return((uint8_t)disp_runs.dots.current);
}

static uint8_t display_config_current_dot_set(uint8_t dot)
{
	uint32_t offset;
    struct __disp_dot dots;
    
	offset = STRUCT_OFFSET(struct __disp_param, dots);
    file.parameter.read("display", offset, sizeof(disp_runs.dots), &dots);
    disp_runs.dots.current = dot;
	file.parameter.write("display", offset, sizeof(disp_runs.dots), &dots);
    
    return((uint8_t)disp_runs.dots.current);
}

static uint8_t display_config_energy_dot_get(void)
{
    return((uint8_t)disp_runs.dots.energy);
}

static uint8_t display_config_energy_dot_set(uint8_t dot)
{
	uint32_t offset;
    struct __disp_dot dots;
    
	offset = STRUCT_OFFSET(struct __disp_param, dots);
    file.parameter.read("display", offset, sizeof(disp_runs.dots), &dots);
    disp_runs.dots.energy = dot;
	file.parameter.write("display", offset, sizeof(disp_runs.dots), &dots);
    
    return((uint8_t)disp_runs.dots.energy);
}

static uint8_t display_config_demand_dot_get(void)
{
    return((uint8_t)disp_runs.dots.demand);
}

static uint8_t display_config_demand_dot_set(uint8_t dot)
{
	uint32_t offset;
    struct __disp_dot dots;
    
	offset = STRUCT_OFFSET(struct __disp_param, dots);
    file.parameter.read("display", offset, sizeof(disp_runs.dots), &dots);
    disp_runs.dots.demand = dot;
	file.parameter.write("display", offset, sizeof(disp_runs.dots), &dots);
    
    return((uint8_t)disp_runs.dots.demand);
}

static uint8_t display_config_angle_dot_get(void)
{
    return((uint8_t)disp_runs.dots.angle);
}

static uint8_t display_config_angle_dot_set(uint8_t dot)
{
	uint32_t offset;
    struct __disp_dot dots;
    
	offset = STRUCT_OFFSET(struct __disp_param, dots);
    file.parameter.read("display", offset, sizeof(disp_runs.dots), &dots);
    disp_runs.dots.angle = dot;
	file.parameter.write("display", offset, sizeof(disp_runs.dots), &dots);
    
    return((uint8_t)disp_runs.dots.angle);
}

static uint8_t display_config_freq_dot_get(void)
{
    return((uint8_t)disp_runs.dots.freq);
}

static uint8_t display_config_freq_dot_set(uint8_t dot)
{
	uint32_t offset;
    struct __disp_dot dots;
    
	offset = STRUCT_OFFSET(struct __disp_param, dots);
    file.parameter.read("display", offset, sizeof(disp_runs.dots), &dots);
    disp_runs.dots.freq = dot;
	file.parameter.write("display", offset, sizeof(disp_runs.dots), &dots);
    
    return((uint8_t)disp_runs.dots.freq);
}

static uint8_t display_config_pf_dot_get(void)
{
    return((uint8_t)disp_runs.dots.pf);
}

static uint8_t display_config_pf_dot_set(uint8_t dot)
{
	uint32_t offset;
    struct __disp_dot dots;
    
	offset = STRUCT_OFFSET(struct __disp_param, dots);
    file.parameter.read("display", offset, sizeof(disp_runs.dots), &dots);
    disp_runs.dots.pf = dot;
	file.parameter.write("display", offset, sizeof(disp_runs.dots), &dots);
    
    return((uint8_t)disp_runs.dots.pf);
}

static uint16_t display_config_list_write(uint8_t channel, void *id)
{
	uint32_t offset;
    uint8_t amount = 0;
    
    if(channel >= DISP_CONF_CHANNEL_AMOUNT)
    {
        return(0);
    }
    
    if(!id)
    {
        return(0);
    }
    
	offset = STRUCT_OFFSET(struct __disp_param, list[channel].amount);
	file.parameter.read("display", offset, sizeof(amount), &amount);
    
    if(amount >= DISP_CONF_AMOUNT_IN_LIST)
    {
        return(16);
    }
    
	offset = STRUCT_OFFSET(struct __disp_param, list[channel].entry[3]);
	file.parameter.write("display", offset, 16, id);
    
    amount += 1;
    
	offset = STRUCT_OFFSET(struct __disp_param, list[channel].amount);
	file.parameter.write("display", offset, sizeof(amount), &amount);
    
    if(channel == disp_runs.channel)
    {
        disp_runs.amount = amount;
    }
    
    return(16);
}

static uint16_t display_config_list_read(uint8_t channel, uint16_t index, void *id)
{
	uint32_t offset;
    uint8_t amount = 0;
    
    if(channel >= DISP_CONF_CHANNEL_AMOUNT)
    {
        return(0);
    }
    
    if(!id)
    {
        return(0);
    }
    
	offset = STRUCT_OFFSET(struct __disp_param, list[channel].amount);
	file.parameter.read("display", offset, sizeof(amount), &amount);
    
    if(index >= amount)
    {
        return(0);
    }
    
	offset = STRUCT_OFFSET(struct __disp_param, list[channel].entry[index]);
	file.parameter.read("display", offset, 16, id);
    
    return(16);
}

static uint8_t display_config_list_amount(uint8_t channel)
{
	uint32_t offset;
    uint8_t amount = 0;
    
    if(channel >= DISP_CONF_CHANNEL_AMOUNT)
    {
        return(0);
    }
    
	offset = STRUCT_OFFSET(struct __disp_param, list[channel].amount);
	file.parameter.read("display", offset, sizeof(amount), &amount);
    
    return(amount);
}

static uint8_t display_config_list_clean(uint8_t channel)
{
	uint32_t offset;
    uint8_t amount = 0;
    
    if(channel >= DISP_CONF_CHANNEL_AMOUNT)
    {
        return(0);
    }
    
	offset = STRUCT_OFFSET(struct __disp_param, list[channel].amount);
	file.parameter.write("display", offset, sizeof(amount), &amount);
    
    if(channel == disp_runs.channel)
    {
        disp_runs.amount = 0;
    }
    
    return(0);
}


static const struct __display display = 
{
    .change                     = display_change,
    .channel                    = display_channel,
    
    .list                       =
    {
        .show                   =
        {
            .next               = display_list_show_next,
            .last               = display_list_show_last,
            .index              = display_list_show_index,
        },
        
        .current                = display_list_current,
		
		.insert					=
		{
			.message			= display_list_insert_message,
			.instance			= display_list_insert_instance,
		},
    },
    
    .config                     =
    {
        .time                   =
        {
            .start              = 
            {
                .get            = display_config_start_get,
                .set            = display_config_start_set,
            },
            
            .scroll             = 
            {
                .get            = display_config_scroll_get,
                .set            = display_config_scroll_set,
            },
            
            .backlight          = 
            {
                .get            = display_config_backlight_get,
                .set            = display_config_backlight_set,
            },
        },
        
        .dot                    =
        {
            .power              = 
            {
                .get            = display_config_power_dot_get,
                .set            = display_config_power_dot_set,
            },
            
            .voltage            = 
            {
                .get            = display_config_voltage_dot_get,
                .set            = display_config_voltage_dot_set,
            },
            
            .current            = 
            {
                .get            = display_config_current_dot_get,
                .set            = display_config_current_dot_set,
            },
            
            .energy             = 
            {
                .get            = display_config_energy_dot_get,
                .set            = display_config_energy_dot_set,
            },
            
            .demand             = 
            {
                .get            = display_config_demand_dot_get,
                .set            = display_config_demand_dot_set,
            },
            
            .angle              = 
            {
                .get            = display_config_angle_dot_get,
                .set            = display_config_angle_dot_set,
            },
            
            .freq               = 
            {
                .get            = display_config_freq_dot_get,
                .set            = display_config_freq_dot_set,
            },
            
            .pf                 = 
            {
                .get            = display_config_pf_dot_get,
                .set            = display_config_pf_dot_set,
            },
        },
        
        .list                   = 
        {
            .write              = display_config_list_write,
            .read               = display_config_list_read,
            .amount             = display_config_list_amount,
            .clean              = display_config_list_clean,
        }
    },
};




/**
  * @brief  
  */
static void display_init(void)
{
	uint32_t offset;
	
    if((system_status() == SYSTEM_RUN) || (system_status() == SYSTEM_WAKEUP))
    {
        heap.set(&disp_runs, 0, sizeof(disp_runs));
        
        disp_runs.channel = DISP_CONF_CHANNEL_SCROLL;
        disp_runs.last_channel = DISP_CONF_CHANNEL_SCROLL;
        
        offset = STRUCT_OFFSET(struct __disp_param, dots);
        file.parameter.read("display", offset, sizeof(disp_runs.dots), &disp_runs.dots);
        
		offset = STRUCT_OFFSET(struct __disp_param, time);
		file.parameter.read("display", offset, sizeof(disp_runs.time), &disp_runs.time);
		
		offset = STRUCT_OFFSET(struct __disp_param, list[disp_runs.channel].amount);
		file.parameter.read("display", offset, sizeof(disp_runs.amount), &disp_runs.amount);
		
		offset = STRUCT_OFFSET(struct __disp_param, list[disp_runs.channel].entry[disp_runs.index]);
		file.parameter.read("display", offset, sizeof(disp_runs.entry), &disp_runs.entry);
        
        if(disp_runs.amount > DISP_CONF_AMOUNT_IN_LIST)
        {
            disp_runs.amount = 0;
        }
        
        if(system_status() == SYSTEM_RUN)
        {
            lcd.control.init(DEVICE_NORMAL);
            lcd.backlight.open();
            disp_runs.counter.backlight = disp_runs.time.backlight;
        }
        else
        {
            lcd.control.init(DEVICE_LOWPOWER);
        }
        
        lcd.show.all();
        
        status = TASK_INIT;
		TRACE(TRACE_INFO, "Task display initialized.");
    }
}

/**
  * @brief  
  */
static void display_loop(void)
{
	struct __disp_entry entry;
	
    if((system_status() == SYSTEM_RUN) || (system_status() == SYSTEM_WAKEUP))
    {
        status = TASK_RUN;
        lcd.runner(KERNEL_PERIOD);
        
        //全显完成之后才开始定时刷新
        if(disp_runs.counter.start >= disp_runs.time.start)
        {
            disp_runs.loop_flush += KERNEL_PERIOD;
        }
        
        //非正常运行状态下，直接关闭背光
        if(lcd.backlight.status() && (system_status() != SYSTEM_RUN))
        {
            disp_runs.counter.backlight = 0;
            
            lcd.backlight.close();
        }
        
		//处理插入显示
		if(disp_runs.insert_millisecond)
		{
			if(system_status() == SYSTEM_RUN)
			{
				lcd.backlight.open();
			}
			
			if(disp_runs.insert_millisecond > KERNEL_PERIOD)
			{
				disp_runs.insert_millisecond -= KERNEL_PERIOD;
			}
			else
			{
				disp_runs.insert_millisecond = 0;
				disp_runs.loop_record = 2000;
			}
			
			if(!disp_runs.is_entry)
			{
				return;
			}
			
			//499ms刷新主窗口的显示数据
			if(disp_runs.loop_flush > 499)
			{
				heap.copy(&entry, &disp_runs.entry.descriptor, sizeof(entry));
				heap.copy(&disp_runs.entry.descriptor, &disp_runs.insert_entry.descriptor, sizeof(disp_runs.entry.descriptor));
				
				disp_runs.loop_flush = 0;
				flush_data();
				
				heap.copy(&disp_runs.entry.descriptor, &entry, sizeof(entry));
			}
			
			return;
		}
		
		disp_runs.loop_record += KERNEL_PERIOD;
		
        //秒延时
		if(disp_runs.loop_record > 999)
		{
			disp_runs.loop_record = 0;
            disp_runs.loop_flush = 0;
            
            //背光延时处理
            if(disp_runs.counter.backlight)
            {
                disp_runs.counter.backlight -= 1;
                
                if(disp_runs.counter.backlight == 0)
                {
                    if(lcd.backlight.status())
                    {
                        lcd.backlight.close();
                    }
                }
            }
            
            //非键显下，自动滚屏
            if((disp_runs.channel == DISP_CONF_CHANNEL_SCROLL) || (disp_runs.channel == DISP_CONF_CHANNEL_DEBUG))
            {
                //如果当前在上电全显状态，则不再往下处理
                if(disp_runs.counter.start < disp_runs.time.start)
                {
                    disp_runs.counter.start += 1;
                    
                    if(disp_runs.counter.start >= disp_runs.time.start)
                    {
                        display.list.show.index(disp_runs.index);
                    }
                    
                    return;
                }
                
                //滚屏时间计时
                disp_runs.counter.scroll += 1;
                
                //滚屏时间计时大于配置的滚屏时间，开始滚屏
                if(disp_runs.counter.scroll >= disp_runs.time.scroll)
                {
                    display.list.show.next();
                }
            }
            //键显下，滚屏时间到达之后切回之前的显示列表
            else if(disp_runs.channel == DISP_CONF_CHANNEL_ALTERNATE)
            {
                disp_runs.counter.scroll += 1;
                
                if(disp_runs.counter.scroll >= disp_runs.time.scroll)
                {
                	display.change(disp_runs.last_channel);
                }
                
                //切换到键显后，失能上电全显
                disp_runs.counter.start = disp_runs.time.start + 1;
            }
		}
        
        //499ms刷新主窗口的显示数据
        if(disp_runs.loop_flush > 499)
        {
            disp_runs.loop_flush = 0;
            flush_data();
        }
    }
}

/**
  * @brief  
  */
static void display_exit(void)
{
    lcd.control.suspend();
    status = TASK_SUSPEND;
	TRACE(TRACE_INFO, "Task display exited.");
}

/**
  * @brief  
  */
static void display_reset(void)
{
    lcd.control.suspend();
    status = TASK_NOTINIT;
    
#if defined ( MAKE_RUN_FOR_DEBUG )
    {
        uint32_t offset;
        uint8_t val;
        uint8_t descriptor[16];
        struct __disp_dot dots;
        
        descriptor[0] = 0;
        descriptor[1] = 3;
        
        descriptor[2] = 1;
        descriptor[3] = 0;
        descriptor[4] = 0;
        descriptor[5] = 7;
        descriptor[6] = 0;
        descriptor[7] = 255;
        
        descriptor[8] = 2;
        
        descriptor[9] = 0;
        descriptor[10] = 0;
        
        
        heap.set((void *)&dots, 0, sizeof(dots));
        
        dots.power = 3;
        dots.voltage = 1;
        dots.current = 3;
        dots.energy = 3;
        dots.demand = 4;
        dots.angle = 3;
        dots.freq = 2;
        dots.pf = 3;
        
        offset = STRUCT_OFFSET(struct __disp_param, dots);
        file.parameter.write("display", offset, sizeof(dots), &dots);
        
        val = 6;
        offset = STRUCT_OFFSET(struct __disp_param, time.scroll);
        file.parameter.write("display", offset, sizeof(val), &val);
        
        val = 8;
        offset = STRUCT_OFFSET(struct __disp_param, time.backlight);
        file.parameter.write("display", offset, sizeof(val), &val);
        
        val = 3;
        offset = STRUCT_OFFSET(struct __disp_param, time.start);
        file.parameter.write("display", offset, sizeof(val), &val);
        
        val = 6;
        offset = STRUCT_OFFSET(struct __disp_param, list[DISP_CONF_CHANNEL_SCROLL].amount);
        file.parameter.write("display", offset, sizeof(val), &val);
        offset = STRUCT_OFFSET(struct __disp_param, list[DISP_CONF_CHANNEL_ALTERNATE].amount);
        file.parameter.write("display", offset, sizeof(val), &val);
        
        descriptor[4] = 32;
        offset = STRUCT_OFFSET(struct __disp_param, list[DISP_CONF_CHANNEL_SCROLL].entry[0]);
        file.parameter.write("display", offset, sizeof(descriptor), &descriptor);
        offset = STRUCT_OFFSET(struct __disp_param, list[DISP_CONF_CHANNEL_ALTERNATE].entry[0]);
        file.parameter.write("display", offset, sizeof(descriptor), &descriptor);
        
        descriptor[4] = 52;
        offset = STRUCT_OFFSET(struct __disp_param, list[DISP_CONF_CHANNEL_SCROLL].entry[1]);
        file.parameter.write("display", offset, sizeof(descriptor), &descriptor);
        offset = STRUCT_OFFSET(struct __disp_param, list[DISP_CONF_CHANNEL_ALTERNATE].entry[1]);
        file.parameter.write("display", offset, sizeof(descriptor), &descriptor);
        
        descriptor[4] = 72;
        offset = STRUCT_OFFSET(struct __disp_param, list[DISP_CONF_CHANNEL_SCROLL].entry[2]);
        file.parameter.write("display", offset, sizeof(descriptor), &descriptor);
        offset = STRUCT_OFFSET(struct __disp_param, list[DISP_CONF_CHANNEL_ALTERNATE].entry[2]);
        file.parameter.write("display", offset, sizeof(descriptor), &descriptor);
        
        descriptor[4] = 31;
        offset = STRUCT_OFFSET(struct __disp_param, list[DISP_CONF_CHANNEL_SCROLL].entry[3]);
        file.parameter.write("display", offset, sizeof(descriptor), &descriptor);
        offset = STRUCT_OFFSET(struct __disp_param, list[DISP_CONF_CHANNEL_ALTERNATE].entry[3]);
        file.parameter.write("display", offset, sizeof(descriptor), &descriptor);
        
        descriptor[4] = 51;
        offset = STRUCT_OFFSET(struct __disp_param, list[DISP_CONF_CHANNEL_SCROLL].entry[4]);
        file.parameter.write("display", offset, sizeof(descriptor), &descriptor);
        offset = STRUCT_OFFSET(struct __disp_param, list[DISP_CONF_CHANNEL_ALTERNATE].entry[4]);
        file.parameter.write("display", offset, sizeof(descriptor), &descriptor);
        
        descriptor[4] = 71;
        offset = STRUCT_OFFSET(struct __disp_param, list[DISP_CONF_CHANNEL_SCROLL].entry[5]);
        file.parameter.write("display", offset, sizeof(descriptor), &descriptor);
        offset = STRUCT_OFFSET(struct __disp_param, list[DISP_CONF_CHANNEL_ALTERNATE].entry[5]);
        file.parameter.write("display", offset, sizeof(descriptor), &descriptor);
    }
#endif // #if defined ( MAKE_RUN_FOR_DEBUG )
	
	TRACE(TRACE_INFO, "Task display reset.");
}

/**
  * @brief  
  */
static enum __task_status display_status(void)
{
    return(status);
}


/**
  * @brief  
  */
const struct __task_sched task_display = 
{
    .name               = NAME_DISPLAY,
    .init               = display_init,
    .loop               = display_loop,
    .exit               = display_exit,
    .reset              = display_reset,
    .status             = display_status,
    .api                = (void *)&display,
};

