/**
 * @brief		
 * @details		
 * @date		azenk@2019-01-10
 **/


/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "dlms_utilities.h"
#include "crc.h"
#include "info.h"

/* Private typedef -----------------------------------------------------------*/
/**	
  * @brief 
  */
struct __dlms_sym_key
{
    uint8_t length;//√‹‘ø≥§∂» <= 48
    uint8_t val[59];//√‹‘ø
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
    uint8_t title[16]; //System title
};

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**	
  * @brief 
  */
void confuse(uint8_t *buffer, uint8_t size)
{
    uint8_t cnt;
    
    for(cnt=0; cnt<size; cnt++)
    {
        buffer[cnt] ^= 0xaa;
    }
}

/**	
  * @brief 
  */
uint8_t dlms_util_load_passwd(uint8_t *buffer)
{
    struct __dlms_sym_key key;
    
    heap.set(&key, 0, sizeof(key));
    
    if(file.read("dlms", \
                 STRUCT_OFFSET(struct __dlms_configs, passwd), \
                 sizeof(key), \
                 (void *)&key) != sizeof(key))
    {
        buffer[1] = 16;
        heap.set(&buffer[2], 0x30, 16);
        return(18);
    }
    
    if((crc32(key.val, sizeof(key.val)) != key.check) || \
        (key.length < 8) || (key.length > 48))
    {
        buffer[1] = 16;
        heap.set(&buffer[2], 0x30, 16);
        return(18);
    }
    else
    {
        confuse(key.val, sizeof(key.val));
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
    
    heap.set(&key, 0, sizeof(key));
    
    if((buffer[1] < 8) || (buffer[1] > 48))
    {
        return(0);
    }
    
    key.length = buffer[1];
    heap.copy(key.val, &buffer[2], key.length);
    confuse(key.val, sizeof(key.val));
    key.check = crc32(key.val, sizeof(key.val));
    
    if(file.write("dlms", \
                  STRUCT_OFFSET(struct __dlms_configs, passwd), \
                  sizeof(key), \
                  (void *)&key) != sizeof(key))
    {
        return(0);
    }
    
    return(key.length + 2);
}

/**	
  * @brief 
  */
uint8_t dlms_util_load_management_passwd(uint8_t *buffer)
{
    heap.copy(buffer, "\x00\x10", 2);
    info_get_management_passwd(16, &buffer[2]);
    return(18);
}

/**	
  * @brief 
  */
uint8_t dlms_util_load_akey(uint8_t *buffer)
{
    struct __dlms_sym_key key;
    
    heap.set(&key, 0, sizeof(key));
    
    if(file.read("dlms", \
                 STRUCT_OFFSET(struct __dlms_configs, akey), \
                 sizeof(key), \
                 (void *)&key) != sizeof(key))
    {
        buffer[1] = 16;
        heap.set(&buffer[2], 0x30, 16);
        return(18);
    }
    
    if((crc32(key.val, sizeof(key.val)) != key.check) || \
        (key.length != 16) && (key.length != 32))
    {
        buffer[1] = 16;
        heap.set(&buffer[2], 0x30, 16);
        return(18);
    }
    else
    {
        confuse(key.val, sizeof(key.val));
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
    
    heap.set(&key, 0, sizeof(key));
    
    if((buffer[1] != 16) && (buffer[1] != 32))
    {
        return(0);
    }
    
    key.length = buffer[1];
    heap.copy(key.val, &buffer[2], key.length);
    confuse(key.val, sizeof(key.val));
    key.check = crc32(key.val, sizeof(key.val));
    
    if(file.write("dlms", \
                  STRUCT_OFFSET(struct __dlms_configs, akey), \
                  sizeof(key), \
                  (void *)&key) != sizeof(key))
    {
        return(0);
    }
    
    return(key.length + 2);
}

/**	
  * @brief 
  */
uint8_t dlms_util_load_bekey(uint8_t *buffer)
{
    struct __dlms_sym_key key;
    
    heap.set(&key, 0, sizeof(key));
    
    if(file.read("dlms", \
                 STRUCT_OFFSET(struct __dlms_configs, bekey), \
                 sizeof(key), \
                 (void *)&key) != sizeof(key))
    {
        buffer[1] = 16;
        heap.set(&buffer[2], 0x30, 16);
        return(18);
    }
    
    if((crc32(key.val, sizeof(key.val)) != key.check) || \
        (key.length != 16) && (key.length != 32))
    {
        buffer[1] = 16;
        heap.set(&buffer[2], 0x30, 16);
        return(18);
    }
    else
    {
        confuse(key.val, sizeof(key.val));
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
    
    heap.set(&key, 0, sizeof(key));
    
    if((buffer[1] != 16) && (buffer[1] != 32))
    {
        return(0);
    }
    
    key.length = buffer[1];
    heap.copy(key.val, &buffer[2], key.length);
    confuse(key.val, sizeof(key.val));
    key.check = crc32(key.val, sizeof(key.val));
    
    if(file.write("dlms", \
                  STRUCT_OFFSET(struct __dlms_configs, bekey), \
                  sizeof(key), \
                  (void *)&key) != sizeof(key))
    {
        return(0);
    }
    
    return(key.length + 2);
}

/**	
  * @brief 
  */
uint8_t dlms_util_load_uekey(uint8_t *buffer)
{
    struct __dlms_sym_key key;
    
    heap.set(&key, 0, sizeof(key));
    
    if(file.read("dlms", \
                 STRUCT_OFFSET(struct __dlms_configs, uekey), \
                 sizeof(key), \
                 (void *)&key) != sizeof(key))
    {
        buffer[1] = 16;
        heap.set(&buffer[2], 0x30, 16);
        return(18);
    }
    
    if((crc32(key.val, sizeof(key.val)) != key.check) || \
        (key.length != 16) && (key.length != 32))
    {
        buffer[1] = 16;
        heap.set(&buffer[2], 0x30, 16);
        return(18);
    }
    else
    {
        confuse(key.val, sizeof(key.val));
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
    
    heap.set(&key, 0, sizeof(key));
    
    if((buffer[1] != 16) && (buffer[1] != 32))
    {
        return(0);
    }
    
    key.length = buffer[1];
    heap.copy(key.val, &buffer[2], key.length);
    confuse(key.val, sizeof(key.val));
    key.check = crc32(key.val, sizeof(key.val));
    
    if(file.write("dlms", \
                  STRUCT_OFFSET(struct __dlms_configs, uekey), \
                  sizeof(key), \
                  (void *)&key) != sizeof(key))
    {
        return(0);
    }
    
    return(key.length + 2);
}

/**	
  * @brief 
  */
uint8_t dlms_util_load_title(uint8_t *buffer)
{
    uint8_t title[16];
    
    if(file.read("dlms", \
              STRUCT_OFFSET(struct __dlms_configs, title), \
              sizeof(title), \
              title) != sizeof(title))
    {
        buffer[1] = 8;
        heap.set(&buffer[2], 0x30, 8);
    }
    
    if(title[1] != 8)
    {
        buffer[1] = 8;
        heap.set(&buffer[2], 0x30, 8);
    }
    else
    {
        confuse(&title[2], 8);
        heap.copy(&buffer[2], &title[2], 8);
    }
    
    return(10);
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
    
    confuse(&buffer[2], 8);
    
    file.write("dlms", \
              STRUCT_OFFSET(struct __dlms_configs, passwd), \
              2+buffer[1], \
              buffer);
    
    return(2+buffer[1]);
}
