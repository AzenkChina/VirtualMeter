/**
 * @brief		
 * @details		
 * @date		azenk@2019-01-10
 **/


/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "dlms_utilities.h"

/* Private typedef -----------------------------------------------------------*/
/**	
  * @brief 
  */
struct __dlms_configs
{
    uint8_t akey[2+32];
    uint8_t ekey[2+32];
    uint8_t passwd[2+32];
    uint8_t title[2+8];
};

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**	
  * @brief 
  */
uint8_t dlms_util_load_akey(uint8_t *buffer)
{
    file.read("dlms", \
              STRUCT_OFFSET(struct __dlms_configs, akey), \
              STRUCT_SIZE(struct __dlms_configs, akey), \
              buffer);
    
    if((buffer[1] != 16) && (buffer[1] != 32))
    {
        buffer[1] = 16;
        heap.set(&buffer[2], 0x30, 16);
    }
    
    return(STRUCT_SIZE(struct __dlms_configs, akey));
}

/**	
  * @brief 
  */
uint8_t dlms_util_write_akey(uint8_t *buffer)
{
    if((buffer[1] != 16) && (buffer[1] != 32))
    {
        return(0);
    }
    
    file.write("dlms", \
              STRUCT_OFFSET(struct __dlms_configs, akey), \
              2+buffer[1], \
              buffer);
    
    return(2+buffer[1]);
}

/**	
  * @brief 
  */
uint8_t dlms_util_load_ekey(uint8_t *buffer)
{
    file.read("dlms", \
              STRUCT_OFFSET(struct __dlms_configs, ekey), \
              STRUCT_SIZE(struct __dlms_configs, ekey), \
              buffer);
    
    if((buffer[1] != 16) && (buffer[1] != 32))
    {
        buffer[1] = 16;
        heap.set(&buffer[2], 0x30, 16);
    }
    
    return(STRUCT_SIZE(struct __dlms_configs, ekey));
}

/**	
  * @brief 
  */
uint8_t dlms_util_write_ekey(uint8_t *buffer)
{
    if((buffer[1] != 16) && (buffer[1] != 32))
    {
        return(0);
    }
    
    file.write("dlms", \
              STRUCT_OFFSET(struct __dlms_configs, ekey), \
              2+buffer[1], \
              buffer);
    
    return(2+buffer[1]);
}

/**	
  * @brief 
  */
uint8_t dlms_util_load_passwd(uint8_t *buffer)
{
    file.read("dlms", \
              STRUCT_OFFSET(struct __dlms_configs, passwd), \
              STRUCT_SIZE(struct __dlms_configs, passwd), \
              buffer);
    
    if((buffer[1] != 16) && (buffer[1] != 32))
    {
        buffer[1] = 16;
        heap.set(&buffer[2], 0x30, 16);
    }
    
    return(STRUCT_SIZE(struct __dlms_configs, passwd));
}

/**	
  * @brief 
  */
uint8_t dlms_util_write_passwd(uint8_t *buffer)
{
    if((buffer[1] != 16) && (buffer[1] != 32))
    {
        return(0);
    }
    
    file.write("dlms", \
              STRUCT_OFFSET(struct __dlms_configs, passwd), \
              2+buffer[1], \
              buffer);
    
    return(2+buffer[1]);
}

/**	
  * @brief 
  */
uint8_t dlms_util_load_title(uint8_t *buffer)
{
    file.read("dlms", \
              STRUCT_OFFSET(struct __dlms_configs, title), \
              STRUCT_SIZE(struct __dlms_configs, title), \
              buffer);
    
    if(buffer[1] != 8)
    {
        buffer[1] = 8;
        heap.set(&buffer[2], 0x30, 8);
    }
    
    return(STRUCT_SIZE(struct __dlms_configs, title));
}

/**	
  * @brief 
  */
uint8_t dlms_util_write_title(uint8_t *buffer)
{
    if(buffer[1] != 8)
    {
        return(0);
    }
    
    file.write("dlms", \
              STRUCT_OFFSET(struct __dlms_configs, passwd), \
              2+buffer[1], \
              buffer);
    
    return(2+buffer[1]);
}
