/**
 * @brief		
 * @details		
 * @date		azenk@2019-01-10
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "dlms_application.h"
#include "dlms_association.h"
#include "object_template.h"

/* Private typedef -----------------------------------------------------------*/
/**	
  * @brief Ӧ�ò�ְ�������
  */
struct __cosem_blocks
{
    uint32_t    counter;
    uint8_t     is_not_end;
};

/**	
  * @brief ����������Ŀ��¼
  */
struct __cosem_rows
{
    uint32_t    amount;
    uint32_t    counter;
};

/**	
  * @brief ���ݶ���
  */
struct __cosem_entry
{
    TypeObject              object;
    struct __cosem_blocks   block;
    struct __cosem_rows     row;
};



/**	
  * @brief get
  */
struct __type_get
{
    uint8_t *classid;
    uint8_t *obis;
    uint8_t *index;
    uint8_t *selector;
    uint32_t block;
    struct __type_get *next;
};

/**	
  * @brief set
  */
struct __type_set
{
    uint8_t *classid;
    uint8_t *obis;
    uint8_t *index;
    uint8_t *data;
    uint16_t length;
    uint32_t block;
    uint8_t *is_end;
    struct __type_set *next;
};

/**	
  * @brief action
  */
struct __type_action
{
    uint8_t *classid;
    uint8_t *obis;
    uint8_t *index;
    uint8_t *data;
    uint16_t length;
    uint32_t block;
    uint8_t *is_end;
    struct __type_action *next;
};

/**	
  * @brief ��������
  */
struct __dlms_request
{
    uint8_t *service;
    uint8_t *type;
    uint8_t *id;
    void *info; //struct __type_get || struct __type_set || struct __type_action
};

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static struct __cosem_entry *current_entry = (void *)0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief �������ݱ�������Ч����
  * 
  */
static void parse_dlms_frame(const uint8_t *info, uint32_t length, struct __dlms_request *request)
{
    //...
}

/**
  * @brief ���ɷ���ʵ��
  * 
  */
static uint8_t fork_cosem_instance(const struct __dlms_request *request, void **data)
{
    //...
    return(0);
}


/**	
  * @brief 
  */
void dlms_appl_entrance(void **data,
                        const uint8_t *info,
                        uint16_t length,
                        uint8_t *buffer,
                        uint16_t buffer_length,
                        uint16_t *filled_length)
{
    struct __dlms_request request;
    uint8_t entry_amount;
    uint8_t cnt;
    
    heap.set(&request, 0, sizeof(request));
    
    //��������
    parse_dlms_frame(info, length, &request);
    
    //���ɷ��ʶ�������
    entry_amount = fork_cosem_instance(&request, data);
    
    //���������������ɵķ��ʶ���
    for(cnt=0; cnt<entry_amount; cnt++)
    {
        current_entry = ((struct __cosem_entry *)(*data)) + cnt;
        
        //...����
        
        current_entry = (void *)0;
    }
    
    current_entry = (void *)0;
    
    
    //...�����������
    
    //...�ͷ� struct __dlms_request request;
    
    //...�ж϶������������Ƿ��Ѿ�����
    
}


static uint32_t dlms_closure_block_get(void)
{
    if(!current_entry)
    {
        return(0);
    }
    
    return(current_entry->block.counter);
}

static uint32_t dlms_closure_block_set(uint32_t val)
{
    if(!current_entry)
    {
        return(0);
    }
    
    current_entry->block.counter = val; 
    
    return(current_entry->block.counter);
}

static void dlms_closure_block_end(void)
{
    if(!current_entry)
    {
        return;
    }
    
    current_entry->block.is_not_end = 0;
}

static uint8_t dlms_closure_block_is_end(void)
{
    if(!current_entry)
    {
        return(0xff);
    }
    
    return(!(current_entry->block.is_not_end));
}

static uint32_t dlms_closure_amount_get(void)
{
    if(!current_entry)
    {
        return(0);
    }
    
    return(current_entry->row.amount);
}

static uint32_t dlms_closure_amount_set(uint32_t val)
{
    if(!current_entry)
    {
        return(0);
    }
    
    current_entry->row.amount = val;
    
    return(current_entry->row.amount);
}

static uint32_t dlms_closure_counter_get(void)
{
    if(!current_entry)
    {
        return(0);
    }
    
    return(current_entry->row.counter);
}

static void dlms_closure_counter_increase(void)
{
    if(!current_entry)
    {
        return;
    }
    
    current_entry->row.counter += 1;
}

static void dlms_closure_counter_clear(void)
{
    if(!current_entry)
    {
        return;
    }
    
    current_entry->row.counter = 0;
}

const struct __dlms_serializer serializer = 
{
    .block                  = 
    {
        .get                = dlms_closure_block_get,
        .set                = dlms_closure_block_set,
        .end                = dlms_closure_block_end,
        .is_end             = dlms_closure_block_is_end,
    },
    
    .rows                   =
    {
        .amount             =
        {
            .get            = dlms_closure_amount_get,
            .set            = dlms_closure_amount_set,
        },
        
        .counter            =
        {
            .get            = dlms_closure_counter_get,
            .increase       = dlms_closure_counter_increase,
            .clear          = dlms_closure_counter_clear,
        },
    },
};






