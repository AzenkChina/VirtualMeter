/**
 * @brief		
 * @details		
 * @date		
 **/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DLMS_TYPES_H__
#define __DLMS_TYPES_H__

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/**	
  * @brief 协商状态
  */
enum __asso_status
{
    NON_ASSOCIATED = 0,
    ASSOCIATION_PENDING,
    ASSOCIATED,
};

/**
  * DLMS的 访问等级
  *
  */
enum __dlms_access_level
{
    DLMS_ACCESS_NO = 0,
    DLMS_ACCESS_LOWEST,
    DLMS_ACCESS_LOW,
    DLMS_ACCESS_HIGH,
};


/**
  * DLMS 属性的访问权限
  *
  */
enum __dlms_attr_right
{
	ATTRIBUTE_NO_ACCESS = 0,
	ATTRIBUTE_READ_ONLY,
	ATTRIBUTE_WRITE_ONLY,
	ATTRIBUTE_READ_AND_WRITE,
	ATTRIBUTE_AUTEN_READ_ONLY,
	ATTRIBUTE_AUTEN_WRITE_ONLY,
	ATTRIBUTE_AUTEN_READ_AND_WRITE,
};

/**
  * DLMS 方法的访问权限
  *
  */
enum __dlms_method_right
{
	METHOD_NO_ACCESS = 0,
	METHOD_ACCESS,
	METHOD_AUTH_ACCESS,
};

/**
  * DLMS 对象访问权限
  *
  */
union __dlms_right
{
	enum __dlms_attr_right      attr;
    enum __dlms_method_right    method;
};

/**
  * DLMS 数据请求的结果 
  *
  */
enum __dlms_access_result
{
    DATA_SUCCESS = 0,
    DATA_HARDFAULT,
    DATA_TEMPORARY_FAILURE,
    DATA_REAS_WRITE_DENIED,
    DATA_OBJECT_UNDEFINED,
    
    DATA_OBJECT_CLASS_INCONSISTENT = 9,
    
    DATA_OBJECT_UNAVAILABLE = 11,
    DATA_TYPE_UNMATCHED,
    DATA_SCOPE_OF_ACCESS_VIOLATED,
    DATA_DATA_BLOCK_UNAVAILABLE,
    DATA_LONG_GET_ABORTED,
    DATA_NO_LONG_GET_IN_PROGRESS,
    DATA_LONG_SET_ABORTED,
    DATA_NO_LONG_SET_IN_PROGRESS,
    DATA_DATA_BLOCK_NUMBER_INVALID,
    
    DATA_OTHER_REASION = 250,
};

/**
  * DLMS 方法执行的结果 
  *
  */
enum __dlms_action_result
{
    ACTION_SUCCESS = 0,
    ACTION_HARDFAULT,
    ACTION_TEMPORARY_FAILURE,
    ACTION_REAS_WRITE_DENIED,
    ACTION_OBJECT_UNDEFINED,
    
    ACTION_OBJECT_CLASS_INCONSISTENT = 9,
    
    ACTION_OBJECT_UNAVAILABLE = 11,
    ACTION_TYPE_UNMATCHED,
    ACTION_SCOPE_OF_ACCESS_VIOLATED,
    ACTION_DATA_BLOCK_UNAVAILABLE,
    ACTION_LONG_ACTION_ABORTED,
    ACTION_NO_LONG_ACTION_IN_PROGRESS,
    
    ACTION_OTHER_REASION = 250,
};


/**
  * DLMS的 请求类型
  *
  */
enum __dlms_request_type
{
    GET_REQUEST = 192,
    SET_REQUEST,
    ACTION_REQUEST = 195,
    GLO_GET_REQUEST = 200,
    GLO_SET_REQUEST,
    GLO_EVENT_NOTIFICATION_REQUEST,
    GLO_ACTION_REQUEST,
    DED_GET_REQUEST = 208,
    DED_SET_REQUEST,
    DED_EVENT_NOTIFICATION_REQUEST,
    DED_ACTION_REQUEST,
};

/**
  * GET的 请求子类型
  *
  */
enum __dlms_get_request_type
{
    GET_NORMAL = 1,
    GET_NEXT,
    GET_WITH_LIST,
};

/**
  * SET的 请求子类型
  *
  */
enum __dlms_set_request_type
{
    SET_NORMAL = 1,
    SET_FIRST_BLOCK,
    SET_WITH_BLOCK,
    SET_WITH_LIST,
    SET_WITH_LIST_AND_FIRST_BLOCK,
};

/**
  * ACTION的 请求子类型
  *
  */
enum __dlms_action_request_type
{
    ACTION_NORMAL = 1,
    ACTION_NEXT_BLOCK,
    ACTION_WITH_LIST,
    ACTION_FIRST_BLOCK,
    ACTION_WITH_LIST_AND_FIRST_BLOCK,
    ACTION_WITH_BLOCK,
};

/**
  * DLMS的 响应类型
  *
  */
enum __dlms_response_type
{
    GET_RESPONSE = 196,
    SET_RESPONSE,
    ACTION_RESPONSE = 199,
    EXCEPTION_RESPONSE = 216,
    GLO_GET_RESPONSE = 204,
    GLO_SET_RESPONSE = 205,
    GLO_ACTION_RESPONSE = 207,
    DED_GET_RESPONSE = 212,
    DED_SET_RESPONSE = 213,
    DED_ACTION_RESPONSE = 215,
};

/**
  * DLMS的 GET 响应子类型
  *
  */
enum __dlms_get_response_type
{
    GET_RESPONSE_NORMAL = 1,
    GET_RESPONSE_WITH_BLOCK,
    GET_RESPONSE_WITH_LIST,
};

/**
  * DLMS的 SET响应子类型
  *
  */
enum __dlms_set_response_type
{
    SET_RESPONSE_NORMAL = 1,
    SET_RESPONSE_DATABLOCK,
    SET_RESPONSE_LAST_DATABLOCK,
    SET_RESPONSE_LAST_DATABLOCK_WITH_LIST,
    SET_RESPONSE_WITH_LIST,
};

/**
  * DLMS的 Action响应子类型
  *
  */
enum __dlms_action_response_type
{
    ACTION_RESPONSE_NORMAL = 1,
    ACTION_RESPONSE_WITH_BLOCK,
    ACTION_RESPONSE_WITH_LIST,
    ACTION_RESPONSE_NEXT_BLOCK,
};

/**
  * DLMS 多包最后一包标识
  *
  */
enum __dlms_last_block_flag
{
    NOT_LAST_BLOCK = 0,
    IS_LAST_BLOCK,
};

/**
  * DLMS 多包第一包标识
  *
  */
enum __dlms_first_block_flag
{
    IS_FIRST_BLOCK = 0,
    NOT_FIRST_BLOCK,
};

/**
  * DLMS的 Class
  *
  */
enum __dlms_class
{
    CLASS_DATA = 1,
    CLASS_REGISTER = 3,
    CLASS_EXTREGISTER,
    CLASS_DEMANDREGISTER,
    CLASS_PROFILE = 7,
    CLASS_CLOCK,
    CLASS_SCRIPT,
    CLASS_SCHEDULE,
    CLASS_SPECIALDAY = 11,
    CLASS_ASSOCIATION_LN = 15,
    CLASS_SAP = 17,
    CLASS_IMAGE_TRANSFER = 18,
    CLASS_ACTIVITYCALENDER = 20,
    CLASS_REGISTER_MONITOR,
    CLASS_SINGLE_ACTION,
    CLASS_HDLC_SETUP,
    CLASS_MAC_ADDRESS_SETUP = 43,
    CLASS_RELAY = 70,
    CLASS_LIMITER,
};


/**
  * DLMS的物理单位
  *
  */
enum __dlms_unit
{
    DLMS_UNIT_MIN = 6,          // 6 minute
    DLMS_UNIT_S,                // 7 second
    DLMS_UNIT_DEG,              // 8 (phase) angle
    
    DLMS_UNIT_W = 27,           // 27 active power
    DLMS_UNIT_VA,               // 28 apparent power
    DLMS_UNIT_VAR,              // 29 reactive power
    DLMS_UNIT_WH,               // 30 active energy
    DLMS_UNIT_VAH,              // 31 apparent energy
    DLMS_UNIT_VARH,             // 32 reactive energy
    DLMS_UNIT_A,                // 33 current
    
    DLMS_UNIT_V = 35,           // 35 vlotage
    DLMS_UNIT_HZ = 44,          // 44 frequency
    DLMS_UNIT_COUNT = 255,      // 255 no unit,count
};

/**
  * @brief  cosem 对象描述符
  */
struct __cosem_descriptor
{
    uint16_t    classid;
    uint8_t     obis[6];
    uint8_t     index;
    uint16_t    selector;
};

/**
  * @brief  cosem 请求描述符
  */
struct __cosem_request_desc
{
    enum __dlms_access_level    level;
    enum __dlms_request_type    request;
    struct __cosem_descriptor   descriptor;
};

/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif /* __DLMS_TYPES_H__ */
