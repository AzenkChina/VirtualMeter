/**
 * @brief		
 * @details		
 * @date		2016-11-15
 **/

/* Includes ------------------------------------------------------------------*/
#include "info.h"
#include "string.h"
#include "allocator.h"

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief  存储结构
  */
struct __information
{
	uint64_t address; //通信地址
	uint8_t serial[32]; //序列号（ASCII）
	uint8_t type[32]; //型号（ASCII）
	uint16_t phase; //相数
	uint16_t wire; //线数
	uint32_t voltage; //额定电压（mV）
	uint32_t current_rated; //额定电流（mA）
	uint32_t current_max; //最大电流（mA）
	uint32_t frequency; //额定频率（mHz）
	uint32_t accuracy_active; //有功精度（accuracy_active/1000000）
	uint32_t accuracy_reactive; //无功精度（accuracy_reactive/1000000）
	uint64_t manufacture; //生产日期
};

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
//编译时间
const char *compile_time = __TIME__ " "__DATE__ ;

//版本书写规则
//“大版本” + “.” + “小版本” + “+” + “svn或者git等等” + “-数字”
//最长不超过32字节

//硬件版本
const char hardware_version[] = "2.01+svn-3";

//软件版本
const char software_version[] = "18.02+svn-796";

//任务列表版本
const char tasks_version[] = "4.00+svn-221";

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static uint8_t version_parse(enum  __ver_field field, const char *str, const char **start)
{
    const char *str_start = 0;
    const char *str_stop = 0;
    uint8_t str_len = (uint8_t)strlen(str);
    
    if(!str || !str_len || !start || str_len > 32)
    {
        return(0);
    }
    
    switch(field)
    {
        case VERSION_BIG:
        {
            str_start = str;
            str_stop = strchr(str, '.');
            if(!str_stop)
            {
                return(0);
            }
            
            *start = str_start;
            return((uint8_t)(str_stop - str_start));
            
            break;
        }
        case VERSION_LITTLE:
        {
            str_start = strchr(str, '.');
            if(!str_start)
            {
                return(0);
            }
            
            str_stop = strchr(str, '+');
            if(!str_stop)
            {
                return(0);
            }
            
            str_start += 1;
            
            *start = str_start;
            return((uint8_t)(str_stop - str_start));
            break;
        }
        case VERSION_BIGLITTLE:
        {
            str_start = str;
            str_stop = strchr(str, '+');
            if(!str_stop)
            {
                return(0);
            }
            
            *start = str_start;
            return((uint8_t)(str_stop - str_start));
            break;
        }
        case VERSION_SERVER:
        {
            str_start = strchr(str, '+');
            if(!str_start)
            {
                return(0);
            }
            str_start += 1;
            str_stop = &str[str_len];
            
            *start = str_start;
            return((uint8_t)(str_stop - str_start));
            break;
        }
        case VERSION_FULL:
        {
            str_start = str;
            str_stop = &str[str_len];
            
            *start = str_start;
            return((uint8_t)(str_stop - str_start));
            break;
        }
        default :
        {
            return(0);
            break;
        }
    }
}


/**
  * @brief  获取软件版本号
  */
uint8_t info_get_software_version(enum  __ver_field field, uint8_t buff_length, uint8_t *buff)
{
    const char *ver_start = 0;
    uint8_t ver_length = 0;
    
    ver_length = version_parse(field, software_version, &ver_start);
    
    if(ver_length > buff_length)
    {
        ver_length = buff_length;
    }
    
    if(!ver_start || !ver_length)
    {
        return(0);
    }
    
    memcpy((void *)buff, (const void *)ver_start, ver_length);
    
    return(ver_length);
}

/**
  * @brief  获取硬件版本号
  */
uint8_t info_get_hardware_version(enum  __ver_field field, uint8_t buff_length, uint8_t *buff)
{
    const char *ver_start = 0;
    uint8_t ver_length = 0;
    
    ver_length = version_parse(field, hardware_version, &ver_start);
    
    if(ver_length > buff_length)
    {
        ver_length = buff_length;
    }
    
    if(!ver_start || !ver_length)
    {
        return(0);
    }
    
    memcpy((void *)buff, (const void *)ver_start, ver_length);
    
    return(ver_length);
}

/**
  * @brief  获取应用版本号
  */
uint8_t info_get_tasks_version(enum  __ver_field field, uint8_t buff_length, uint8_t *buff)
{
    const char *ver_start = 0;
    uint8_t ver_length = 0;
    
    ver_length = version_parse(field, tasks_version, &ver_start);
    
    if(ver_length > buff_length)
    {
        ver_length = buff_length;
    }
    
    if(!ver_start || !ver_length)
    {
        return(0);
    }
    
    memcpy((void *)buff, (const void *)ver_start, ver_length);
    
    return(ver_length);
}


/**
  * @brief  获取编译时间
  */
uint8_t info_get_compile_time(uint8_t buff_length, uint8_t *buff)
{
    uint8_t len = (uint8_t)strlen(compile_time);
    
    if(len > buff_length)
    {
        len = buff_length;
    }
    
    memcpy((void *)buff, (const void *)compile_time, len);
    
    return(len);
}

/**
  * @brief  获取预置密码
  */
uint8_t info_get_management_passwd(uint8_t buff_length, uint8_t *buff)
{
    if(buff_length < 16)
    {
        return(0);
    }
    
    buff[5] = '>';
    memcpy(&buff[6], "management", 16);
    memcpy(&buff[0], "meter", 5);
    
    return(16);
}





/**
  * @brief  
  */
uint64_t info_get_address(void)
{
    uint64_t address = 0;
    
    if(file.read("information", \
					STRUCT_OFFSET(struct __information, address), \
					sizeof(address), \
					&address) != sizeof(address))
	{
		return(0);
	}
    
    return(address);
}

/**
  * @brief  
  */
uint64_t info_set_address(uint64_t val)
{
	uint64_t address = val;
	
    if(file.write("information", \
					STRUCT_OFFSET(struct __information, address), \
					sizeof(address), \
					&address) != sizeof(address))
	{
		return(0);
	}
    
    return(address);
}

/**
  * @brief  
  */
uint8_t info_get_serial(char *buff)
{
    if(file.read("information", \
					STRUCT_OFFSET(struct __information, serial), \
					STRUCT_SIZE(struct __information, serial), \
					buff) != STRUCT_SIZE(struct __information, serial))
	{
		return(0);
	}
	
	if(strlen(buff) > STRUCT_SIZE(struct __information, serial))
	{
		return(STRUCT_SIZE(struct __information, serial));
	}
	else
	{
		return(strlen(buff));
	}
}

/**
  * @brief  
  */
uint8_t info_set_serial(const char *buff)
{
	uint8_t length;
	
	if(strlen(buff) > STRUCT_SIZE(struct __information, serial))
	{
		length = STRUCT_SIZE(struct __information, serial);
	}
	else
	{
		length = strlen(buff);
	}
	
    if(file.write("information", \
					STRUCT_OFFSET(struct __information, serial), \
					length, \
					buff) != length)
	{
		return(0);
	}
    
    return(length);
}

/**
  * @brief  
  */
uint8_t info_get_type(char *buff)
{
    if(file.read("information", \
					STRUCT_OFFSET(struct __information, type), \
					STRUCT_SIZE(struct __information, type), \
					buff) != STRUCT_SIZE(struct __information, type))
	{
		return(0);
	}
	
	if(strlen(buff) > STRUCT_SIZE(struct __information, type))
	{
		return(STRUCT_SIZE(struct __information, type));
	}
	else
	{
		return(strlen(buff));
	}
}

/**
  * @brief  
  */
uint8_t info_set_type(const char *buff)
{
	uint8_t length;
	
	if(strlen(buff) > STRUCT_SIZE(struct __information, type))
	{
		length = STRUCT_SIZE(struct __information, type);
	}
	else
	{
		length = strlen(buff);
	}
	
    if(file.write("information", \
					STRUCT_OFFSET(struct __information, type), \
					length, \
					buff) != length)
	{
		return(0);
	}
    
    return(length);
}

/**
  * @brief  
  */
uint8_t info_get_phase(void)
{
    uint16_t phase = 0;
    
    if(file.read("information", \
					STRUCT_OFFSET(struct __information, phase), \
					sizeof(phase), \
					&phase) != sizeof(phase))
	{
		return(0);
	}
    
    return(phase);
}

/**
  * @brief  
  */
uint8_t info_set_phase(uint8_t val)
{
	uint16_t phase = val;
	
    if(file.write("information", \
					STRUCT_OFFSET(struct __information, phase), \
					sizeof(phase), \
					&phase) != sizeof(phase))
	{
		return(0);
	}
    
    return(phase);
}

/**
  * @brief  
  */
uint8_t info_get_wire(void)
{
    uint16_t wire = 0;
    
    if(file.read("information", \
					STRUCT_OFFSET(struct __information, wire), \
					sizeof(wire), \
					&wire) != sizeof(wire))
	{
		return(0);
	}
    
    return(wire);
}

/**
  * @brief  
  */
uint8_t info_set_wire(uint8_t val)
{
	uint16_t wire = val;
	
    if(file.write("information", \
					STRUCT_OFFSET(struct __information, wire), \
					sizeof(wire), \
					&wire) != sizeof(wire))
	{
		return(0);
	}
    
    return(wire);
}

/**
  * @brief  
  */
uint32_t info_get_voltage(void)
{
    uint32_t voltage = 0;
    
    if(file.read("information", \
					STRUCT_OFFSET(struct __information, voltage), \
					sizeof(voltage), \
					&voltage) != sizeof(voltage))
	{
		return(0);
	}
    
    return(voltage);
}

/**
  * @brief  
  */
uint32_t info_set_voltage(uint32_t val)
{
	uint32_t voltage = val;
	
    if(file.write("information", \
					STRUCT_OFFSET(struct __information, voltage), \
					sizeof(voltage), \
					&voltage) != sizeof(voltage))
	{
		return(0);
	}
    
    return(voltage);
}

/**
  * @brief  
  */
uint32_t info_get_current_rated(void)
{
    uint32_t current_rated = 0;
    
    if(file.read("information", \
					STRUCT_OFFSET(struct __information, current_rated), \
					sizeof(current_rated), \
					&current_rated) != sizeof(current_rated))
	{
		return(0);
	}
    
    return(current_rated);
}

/**
  * @brief  
  */
uint32_t info_set_current_rated(uint32_t val)
{
	uint32_t current_rated = val;
	
    if(file.write("information", \
					STRUCT_OFFSET(struct __information, current_rated), \
					sizeof(current_rated), \
					&current_rated) != sizeof(current_rated))
	{
		return(0);
	}
    
    return(current_rated);
}

/**
  * @brief  
  */
uint32_t info_get_current_max(void)
{
    uint32_t current_max = 0;
    
    if(file.read("information", \
					STRUCT_OFFSET(struct __information, current_max), \
					sizeof(current_max), \
					&current_max) != sizeof(current_max))
	{
		return(0);
	}
    
    return(current_max);
}

/**
  * @brief  
  */
uint32_t info_set_current_max(uint32_t val)
{
	uint32_t current_max = val;
	
    if(file.write("information", \
					STRUCT_OFFSET(struct __information, current_max), \
					sizeof(current_max), \
					&current_max) != sizeof(current_max))
	{
		return(0);
	}
    
    return(current_max);
}

/**
  * @brief  
  */
uint32_t info_get_frequency(void)
{
    uint32_t frequency = 0;
    
    if(file.read("information", \
					STRUCT_OFFSET(struct __information, frequency), \
					sizeof(frequency), \
					&frequency) != sizeof(frequency))
	{
		return(0);
	}
    
    return(frequency);
}

/**
  * @brief  
  */
uint32_t info_set_frequency(uint32_t val)
{
	uint32_t frequency = val;
	
    if(file.write("information", \
					STRUCT_OFFSET(struct __information, frequency), \
					sizeof(frequency), \
					&frequency) != sizeof(frequency))
	{
		return(0);
	}
    
    return(frequency);
}

/**
  * @brief  
  */
uint32_t info_get_accuracy_active(void)
{
    uint32_t accuracy_active = 0;
    
    if(file.read("information", \
					STRUCT_OFFSET(struct __information, accuracy_active), \
					sizeof(accuracy_active), \
					&accuracy_active) != sizeof(accuracy_active))
	{
		return(0);
	}
    
    return(accuracy_active);
}

/**
  * @brief  
  */
uint32_t info_set_accuracy_active(uint32_t val)
{
	uint32_t accuracy_active = val;
	
    if(file.write("information", \
					STRUCT_OFFSET(struct __information, accuracy_active), \
					sizeof(accuracy_active), \
					&accuracy_active) != sizeof(accuracy_active))
	{
		return(0);
	}
    
    return(accuracy_active);
}

/**
  * @brief  
  */
uint32_t info_get_accuracy_reactive(void)
{
    uint32_t accuracy_reactive = 0;
    
    if(file.read("information", \
					STRUCT_OFFSET(struct __information, accuracy_reactive), \
					sizeof(accuracy_reactive), \
					&accuracy_reactive) != sizeof(accuracy_reactive))
	{
		return(0);
	}
    
    return(accuracy_reactive);
}

/**
  * @brief  
  */
uint32_t info_set_accuracy_reactive(uint32_t val)
{
	uint32_t accuracy_reactive = val;
	
    if(file.write("information", \
					STRUCT_OFFSET(struct __information, accuracy_reactive), \
					sizeof(accuracy_reactive), \
					&accuracy_reactive) != sizeof(accuracy_reactive))
	{
		return(0);
	}
    
    return(accuracy_reactive);
}

/**
  * @brief  
  */
uint64_t info_get_manufacture(void)
{
    uint64_t manufacture = 0;
    
    if(file.read("information", \
					STRUCT_OFFSET(struct __information, manufacture), \
					sizeof(manufacture), \
					&manufacture) != sizeof(manufacture))
	{
		return(0);
	}
    
    return(manufacture);
}

/**
  * @brief  
  */
uint64_t info_set_manufacture(uint64_t val)
{
	uint64_t manufacture = val;
	
    if(file.write("information", \
					STRUCT_OFFSET(struct __information, manufacture), \
					sizeof(manufacture), \
					&manufacture) != sizeof(manufacture))
	{
		return(0);
	}
    
    return(manufacture);
}
