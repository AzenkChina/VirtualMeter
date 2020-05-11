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
struct __hdlc_params
{
    uint16_t address;//本地地址
    uint16_t interval;//链路超时时间
    uint32_t check;
};

/**	
  * @brief AES密钥
  */
struct __dlms_sym_key
{
    uint8_t length;//密钥长度 <= 48
    uint8_t val[59];//密钥
    uint32_t check;
};

/**	
  * @brief 私钥
  */
struct __dlms_asym_pri_key
{
    uint8_t length;//密钥长度 <= 48
    uint8_t val[59];//密钥
    uint32_t check;
};

/**	
  * @brief 公钥
  */
struct __dlms_asym_pub_key
{
    uint8_t length;//密钥长度 <= 96
    uint8_t val[123];//密钥
    uint32_t check;
};


/**	
  * @brief 
  */
struct __dlms_params
{
    struct __hdlc_params hdlc; //HDLC参数
    uint8_t title[16]; //System title
    struct __dlms_sym_key passwd; //Password for LLS
    struct __dlms_sym_key akey; //Authentication key
    struct __dlms_sym_key bekey; //Broadcast encryption key
    struct __dlms_sym_key uekey; //Unicast encryption key
    struct __dlms_sym_key mkey; //Master key (KEK)
	struct __dlms_asym_pri_key saprikey; //Server Agreement Private Key
	struct __dlms_asym_pub_key sapubkey; //Server Agreement Public Key
	struct __dlms_asym_pri_key ssprikey; //Server Signing Private Key
	struct __dlms_asym_pub_key sspubkey; //Server Signing Public Key
	struct __dlms_asym_pub_key capubkey; //Client Agreement Public Key
	struct __dlms_asym_pub_key cspubkey; //Client Signing Public Key
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
uint16_t dlms_util_load_hdlc_address(void)
{
    struct __hdlc_params hdlc;
    
    heap.set(&hdlc, 0, sizeof(hdlc));
    
    if(file.read("dlms", \
                 STRUCT_OFFSET(struct __dlms_params, hdlc), \
                 sizeof(hdlc), \
                 (void *)&hdlc) != sizeof(hdlc))
    {
        return(0x10);
    }
    
    if(crc32(&hdlc, (sizeof(hdlc) - sizeof(hdlc.check))) != hdlc.check)
    {
        return(0x10);
    }
    else
    {
        return(hdlc.address);
    }
}

/**	
  * @brief 
  */
uint16_t dlms_util_write_hdlc_address(uint16_t val)
{
    struct __hdlc_params hdlc;
    
    if((val < 0x10) || (val > 0x3FFD))
    {
        return(~val);
    }
    
    heap.set(&hdlc, 0, sizeof(hdlc));
    
    if(file.read("dlms", \
                 STRUCT_OFFSET(struct __dlms_params, hdlc), \
                 sizeof(hdlc), \
                 (void *)&hdlc) != sizeof(hdlc))
    {
        return(0x10);
    }
    
    hdlc.address = val;
    hdlc.check = crc32(&hdlc, (sizeof(hdlc) - sizeof(hdlc.check)));
    
    if(file.write("dlms", \
                  STRUCT_OFFSET(struct __dlms_params, hdlc), \
                  sizeof(hdlc), \
                  (void *)&hdlc) != sizeof(hdlc))
    {
        return(0x10);
    }
    
    return(hdlc.address);
}

/**	
  * @brief 
  */
uint16_t dlms_util_load_hdlc_interval(void)
{
    struct __hdlc_params hdlc;
    
    heap.set(&hdlc, 0, sizeof(hdlc));
    
    if(file.read("dlms", \
                 STRUCT_OFFSET(struct __dlms_params, hdlc), \
                 sizeof(hdlc), \
                 (void *)&hdlc) != sizeof(hdlc))
    {
        return(10);
    }
    
    if(crc32(&hdlc, (sizeof(hdlc) - sizeof(hdlc.check))) != hdlc.check)
    {
        return(10);
    }
    else
    {
        return(hdlc.interval);
    }
}

/**	
  * @brief 
  */
uint16_t dlms_util_write_hdlc_interval(uint16_t val)
{
    struct __hdlc_params hdlc;
    
    if(val < 10)
    {
        val = 10;
    }
    
    heap.set(&hdlc, 0, sizeof(hdlc));
    
    if(file.read("dlms", \
                 STRUCT_OFFSET(struct __dlms_params, hdlc), \
                 sizeof(hdlc), \
                 (void *)&hdlc) != sizeof(hdlc))
    {
        return(10);
    }
    
    hdlc.interval = val;
    hdlc.check = crc32(&hdlc, (sizeof(hdlc) - sizeof(hdlc.check)));
    
    if(file.write("dlms", \
                  STRUCT_OFFSET(struct __dlms_params, hdlc), \
                  sizeof(hdlc), \
                  (void *)&hdlc) != sizeof(hdlc))
    {
        return(10);
    }
    
    return(hdlc.interval);
}

/**	
  * @brief 
  */
uint8_t dlms_util_load_title(uint8_t *buffer)
{
    uint8_t title[16];
    
    if(file.read("dlms", \
              STRUCT_OFFSET(struct __dlms_params, title), \
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
              STRUCT_OFFSET(struct __dlms_params, passwd), \
              2+buffer[1], \
              buffer);
    
    return(2+buffer[1]);
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
uint8_t dlms_util_load_passwd(uint8_t *buffer)
{
    struct __dlms_sym_key key;
    
    heap.set(&key, 0, sizeof(key));
    
    if(file.read("dlms", \
                 STRUCT_OFFSET(struct __dlms_params, passwd), \
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
                  STRUCT_OFFSET(struct __dlms_params, passwd), \
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
uint8_t dlms_util_load_akey(uint8_t *buffer)
{
    struct __dlms_sym_key key;
    
    heap.set(&key, 0, sizeof(key));
    
    if(file.read("dlms", \
                 STRUCT_OFFSET(struct __dlms_params, akey), \
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
                  STRUCT_OFFSET(struct __dlms_params, akey), \
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
                 STRUCT_OFFSET(struct __dlms_params, bekey), \
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
                  STRUCT_OFFSET(struct __dlms_params, bekey), \
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
                 STRUCT_OFFSET(struct __dlms_params, uekey), \
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
                  STRUCT_OFFSET(struct __dlms_params, uekey), \
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
uint8_t dlms_util_load_mkey(uint8_t *buffer)
{
    struct __dlms_sym_key key;
    
    heap.set(&key, 0, sizeof(key));
    
    if(file.read("dlms", \
                 STRUCT_OFFSET(struct __dlms_params, mkey), \
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
uint8_t dlms_util_write_mkey(uint8_t *buffer)
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
                  STRUCT_OFFSET(struct __dlms_params, mkey), \
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
uint8_t dlms_util_load_saprikey(uint8_t *buffer)
{
    struct __dlms_asym_pri_key key;
    
    heap.set(&key, 0, sizeof(key));
    
    if(file.read("dlms", \
                 STRUCT_OFFSET(struct __dlms_params, saprikey), \
                 sizeof(key), \
                 (void *)&key) != sizeof(key))
    {
        buffer[1] = 32;
        heap.set(&buffer[2], 0x30, 32);
        return(34);
    }
    
    if((crc32(key.val, sizeof(key.val)) != key.check) || \
        (key.length != 32) && (key.length != 48))
    {
        buffer[1] = 32;
        heap.set(&buffer[2], 0x30, 32);
        return(34);
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
uint8_t dlms_util_write_saprikey(uint8_t *buffer)
{
    struct __dlms_asym_pri_key key;
    
    heap.set(&key, 0, sizeof(key));
    
    if((buffer[1] != 32) && (buffer[1] != 48))
    {
        return(0);
    }
    
    key.length = buffer[1];
    heap.copy(key.val, &buffer[2], key.length);
    confuse(key.val, sizeof(key.val));
    key.check = crc32(key.val, sizeof(key.val));
    
    if(file.write("dlms", \
                  STRUCT_OFFSET(struct __dlms_params, saprikey), \
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
uint8_t dlms_util_load_sapubkey(uint8_t *buffer)
{
    struct __dlms_asym_pub_key key;
    
    heap.set(&key, 0, sizeof(key));
    
    if(file.read("dlms", \
                 STRUCT_OFFSET(struct __dlms_params, sapubkey), \
                 sizeof(key), \
                 (void *)&key) != sizeof(key))
    {
        buffer[1] = 64;
        heap.set(&buffer[2], 0x30, 64);
        return(66);
    }
    
    if((crc32(key.val, sizeof(key.val)) != key.check) || \
        (key.length != 64) && (key.length != 96))
    {
        buffer[1] = 64;
        heap.set(&buffer[2], 0x30, 64);
        return(66);
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
uint8_t dlms_util_write_sapubkey(uint8_t *buffer)
{
    struct __dlms_asym_pub_key key;
    
    heap.set(&key, 0, sizeof(key));
    
    if((buffer[1] != 64) && (buffer[1] != 96))
    {
        return(0);
    }
    
    key.length = buffer[1];
    heap.copy(key.val, &buffer[2], key.length);
    confuse(key.val, sizeof(key.val));
    key.check = crc32(key.val, sizeof(key.val));
    
    if(file.write("dlms", \
                  STRUCT_OFFSET(struct __dlms_params, sapubkey), \
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
uint8_t dlms_util_load_ssprikey(uint8_t *buffer)
{
    struct __dlms_asym_pri_key key;
    
    heap.set(&key, 0, sizeof(key));
    
    if(file.read("dlms", \
                 STRUCT_OFFSET(struct __dlms_params, ssprikey), \
                 sizeof(key), \
                 (void *)&key) != sizeof(key))
    {
        buffer[1] = 32;
        heap.set(&buffer[2], 0x30, 32);
        return(34);
    }
    
    if((crc32(key.val, sizeof(key.val)) != key.check) || \
        (key.length != 32) && (key.length != 48))
    {
        buffer[1] = 32;
        heap.set(&buffer[2], 0x30, 32);
        return(34);
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
uint8_t dlms_util_write_ssprikey(uint8_t *buffer)
{
    struct __dlms_asym_pri_key key;
    
    heap.set(&key, 0, sizeof(key));
    
    if((buffer[1] != 32) && (buffer[1] != 48))
    {
        return(0);
    }
    
    key.length = buffer[1];
    heap.copy(key.val, &buffer[2], key.length);
    confuse(key.val, sizeof(key.val));
    key.check = crc32(key.val, sizeof(key.val));
    
    if(file.write("dlms", \
                  STRUCT_OFFSET(struct __dlms_params, ssprikey), \
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
uint8_t dlms_util_load_sspubkey(uint8_t *buffer)
{
    struct __dlms_asym_pub_key key;
    
    heap.set(&key, 0, sizeof(key));
    
    if(file.read("dlms", \
                 STRUCT_OFFSET(struct __dlms_params, sspubkey), \
                 sizeof(key), \
                 (void *)&key) != sizeof(key))
    {
        buffer[1] = 64;
        heap.set(&buffer[2], 0x30, 64);
        return(66);
    }
    
    if((crc32(key.val, sizeof(key.val)) != key.check) || \
        (key.length != 64) && (key.length != 96))
    {
        buffer[1] = 64;
        heap.set(&buffer[2], 0x30, 64);
        return(66);
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
uint8_t dlms_util_write_sspubkey(uint8_t *buffer)
{
    struct __dlms_asym_pub_key key;
    
    heap.set(&key, 0, sizeof(key));
    
    if((buffer[1] != 64) && (buffer[1] != 96))
    {
        return(0);
    }
    
    key.length = buffer[1];
    heap.copy(key.val, &buffer[2], key.length);
    confuse(key.val, sizeof(key.val));
    key.check = crc32(key.val, sizeof(key.val));
    
    if(file.write("dlms", \
                  STRUCT_OFFSET(struct __dlms_params, sspubkey), \
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
uint8_t dlms_util_load_capubkey(uint8_t *buffer)
{
    struct __dlms_asym_pub_key key;
    
    heap.set(&key, 0, sizeof(key));
    
    if(file.read("dlms", \
                 STRUCT_OFFSET(struct __dlms_params, capubkey), \
                 sizeof(key), \
                 (void *)&key) != sizeof(key))
    {
        buffer[1] = 64;
        heap.set(&buffer[2], 0x30, 64);
        return(66);
    }
    
    if((crc32(key.val, sizeof(key.val)) != key.check) || \
        (key.length != 64) && (key.length != 96))
    {
        buffer[1] = 64;
        heap.set(&buffer[2], 0x30, 64);
        return(66);
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
uint8_t dlms_util_write_capubkey(uint8_t *buffer)
{
    struct __dlms_asym_pub_key key;
    
    heap.set(&key, 0, sizeof(key));
    
    if((buffer[1] != 64) && (buffer[1] != 96))
    {
        return(0);
    }
    
    key.length = buffer[1];
    heap.copy(key.val, &buffer[2], key.length);
    confuse(key.val, sizeof(key.val));
    key.check = crc32(key.val, sizeof(key.val));
    
    if(file.write("dlms", \
                  STRUCT_OFFSET(struct __dlms_params, capubkey), \
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
uint8_t dlms_util_load_cspubkey(uint8_t *buffer)
{
    struct __dlms_asym_pub_key key;
    
    heap.set(&key, 0, sizeof(key));
    
    if(file.read("dlms", \
                 STRUCT_OFFSET(struct __dlms_params, cspubkey), \
                 sizeof(key), \
                 (void *)&key) != sizeof(key))
    {
        buffer[1] = 64;
        heap.set(&buffer[2], 0x30, 64);
        return(66);
    }
    
    if((crc32(key.val, sizeof(key.val)) != key.check) || \
        (key.length != 64) && (key.length != 96))
    {
        buffer[1] = 64;
        heap.set(&buffer[2], 0x30, 64);
        return(66);
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
uint8_t dlms_util_write_cspubkey(uint8_t *buffer)
{
    struct __dlms_asym_pub_key key;
    
    heap.set(&key, 0, sizeof(key));
    
    if((buffer[1] != 64) && (buffer[1] != 96))
    {
        return(0);
    }
    
    key.length = buffer[1];
    heap.copy(key.val, &buffer[2], key.length);
    confuse(key.val, sizeof(key.val));
    key.check = crc32(key.val, sizeof(key.val));
    
    if(file.write("dlms", \
                  STRUCT_OFFSET(struct __dlms_params, cspubkey), \
                  sizeof(key), \
                  (void *)&key) != sizeof(key))
    {
        return(0);
    }
    
    return(key.length + 2);
}
