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
struct __dlms_sym_key
{
    uint8_t length;//密钥长度 < 32
    uint8_t val[63];//密钥，使用base64编码
    uint32_t check;
};


/**	
  * @brief 
  */
struct __dlms_configs
{
    struct __dlms_sym_key passwd; //Password for LLS
    struct __dlms_sym_key akey; //Authentication key
    struct __dlms_sym_key bekey; //Broadcast encryption key
    struct __dlms_sym_key uekey; //Unicast encryption key
    struct __dlms_sym_key mkey; //Master key (KEK)
    uint8_t title[10]; //System title
};

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**	
  * @brief 
  */
uint8_t dlms_util_load_passwd(uint8_t *buffer)
{
    struct __dlms_sym_key key;
    
    file.read("dlms", \
              STRUCT_OFFSET(struct __dlms_configs, passwd), \
              STRUCT_SIZE(struct __dlms_configs, passwd), \
              (void *)&key);
    
    if((key.length < 8) || (key.length > 32))
    {
        buffer[1] = 16;
        heap.set(&buffer[2], 0x30, 16);
        return(18);
    }
    else
    {
        buffer[1] = key.length;
        heap.copy(&buffer[2], key.val, key.length);
        return(key.length + 2);
    }
}

/**	
  * @brief 
  */
uint8_t dlms_util_write_passwd(uint8_t *buffer)
{
    struct __dlms_sym_key key;
    
    if((buffer[1] < 8) || (buffer[1] > 32))
    {
        return(0);
    }
    
    key.length = buffer[1];
    heap.copy(key.val, &buffer[2], key.length);
    
    file.write("dlms", \
              STRUCT_OFFSET(struct __dlms_configs, passwd), \
              sizeof(key), \
              (void *)&key);
    
    return(key.length + 2);
}

/**	
  * @brief 
  */
uint8_t dlms_util_load_akey(uint8_t *buffer)
{
    struct __dlms_sym_key key;
    
    file.read("dlms", \
              STRUCT_OFFSET(struct __dlms_configs, akey), \
              STRUCT_SIZE(struct __dlms_configs, akey), \
              (void *)&key);
    
    if((key.length != 16) && (key.length != 32))
    {
        buffer[1] = 16;
        heap.set(&buffer[2], 0x30, 16);
        return(18);
    }
    else
    {
        buffer[1] = key.length;
        heap.copy(&buffer[2], key.val, key.length);
        return(key.length + 2);
    }
}

/**	
  * @brief 
  */
uint8_t dlms_util_write_akey(uint8_t *buffer)
{
    struct __dlms_sym_key key;
    
    if((buffer[1] != 16) && (buffer[1] != 32))
    {
        return(0);
    }
    
    key.length = buffer[1];
    heap.copy(key.val, &buffer[2], key.length);
    
    file.write("dlms", \
              STRUCT_OFFSET(struct __dlms_configs, akey), \
              sizeof(key), \
              (void *)&key);
    
    return(key.length + 2);
}

/**	
  * @brief 
  */
uint8_t dlms_util_load_bekey(uint8_t *buffer)
{
    struct __dlms_sym_key key;
    
    file.read("dlms", \
              STRUCT_OFFSET(struct __dlms_configs, bekey), \
              STRUCT_SIZE(struct __dlms_configs, bekey), \
              (void *)&key);
    
    if((key.length != 16) && (key.length != 32))
    {
        buffer[1] = 16;
        heap.set(&buffer[2], 0x30, 16);
        return(18);
    }
    else
    {
        buffer[1] = key.length;
        heap.copy(&buffer[2], key.val, key.length);
        return(key.length + 2);
    }
}

/**	
  * @brief 
  */
uint8_t dlms_util_write_bekey(uint8_t *buffer)
{
    struct __dlms_sym_key key;
    
    if((buffer[1] != 16) && (buffer[1] != 32))
    {
        return(0);
    }
    
    key.length = buffer[1];
    heap.copy(key.val, &buffer[2], key.length);
    
    file.write("dlms", \
              STRUCT_OFFSET(struct __dlms_configs, bekey), \
              sizeof(key), \
              (void *)&key);
    
    return(key.length + 2);
}

/**	
  * @brief 
  */
uint8_t dlms_util_load_uekey(uint8_t *buffer)
{
    struct __dlms_sym_key key;
    
    file.read("dlms", \
              STRUCT_OFFSET(struct __dlms_configs, uekey), \
              STRUCT_SIZE(struct __dlms_configs, uekey), \
              (void *)&key);
    
    if((key.length != 16) && (key.length != 32))
    {
        buffer[1] = 16;
        heap.set(&buffer[2], 0x30, 16);
        return(18);
    }
    else
    {
        buffer[1] = key.length;
        heap.copy(&buffer[2], key.val, key.length);
        return(key.length + 2);
    }
}

/**	
  * @brief 
  */
uint8_t dlms_util_write_uekey(uint8_t *buffer)
{
    struct __dlms_sym_key key;
    
    if((buffer[1] != 16) && (buffer[1] != 32))
    {
        return(0);
    }
    
    key.length = buffer[1];
    heap.copy(key.val, &buffer[2], key.length);
    
    file.write("dlms", \
              STRUCT_OFFSET(struct __dlms_configs, uekey), \
              sizeof(key), \
              (void *)&key);
    
    return(key.length + 2);
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
