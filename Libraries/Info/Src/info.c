/**
 * @brief		
 * @details		
 * @date		2016-11-15
 **/

/* Includes ------------------------------------------------------------------*/
#include "info.h"
#include "string.h" 

/* Private typedef -----------------------------------------------------------*/
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
uint8_t get_software_version(enum  __ver_field field, uint8_t buff_length, uint8_t *buff)
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
uint8_t get_hardware_version(enum  __ver_field field, uint8_t buff_length, uint8_t *buff)
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
uint8_t get_tasks_version(enum  __ver_field field, uint8_t buff_length, uint8_t *buff)
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
uint8_t get_compile_time(uint8_t buff_length, uint8_t *buff)
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
uint8_t get_management_passwd(uint8_t buff_length, uint8_t *buff)
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
