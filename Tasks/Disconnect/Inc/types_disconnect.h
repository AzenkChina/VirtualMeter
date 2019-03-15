/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TYPES_DISCONNECT_H__
#define __TYPES_DISCONNECT_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
#define RELAY_CONF_CONTENT              (8) //策略配置套数


/**
  * @brief 继电器动作
  *
  */
enum __relay_req_action
{
    RELAY_ACT_RELEASE = 0,//忽略，无操作
    RELAY_ACT_CLOSE = 1,//合闸
    RELAY_ACT_OPEN = 2,//拉闸
};

/**
  * @brief 继电器状态
  *
  */
enum __relay_req_status
{
    RELAY_STA_CLOSE = 0,//合闸
    RELAY_STA_READY = 1,//合闸允许
    RELAY_STA_OPEN = 2,//拉闸
};

/**
  * @brief 策略输出
  *
  */
enum __relay_req_policy
{
    RELAY_REQ_ACCEPT = 0,//接受
    RELAY_REQ_DROP = 1,//丢弃
    RELAY_REQ_MANGLE = 2,//修改
};

/**
  * @brief 继电器请求源，最多8个请求源
  *
  */
enum __relay_req_source
{
    RELAY_SRC_ACCOUNT = 0,//账户
	RELAY_SRC_COMMAND = 1,//命令
    RELAY_SRC_TEST = 2,//测试
    RELAY_SRC_OVERPOWER = 3,//超限
    RELAY_SRC_EVENTS = 4,//事件
    RELAY_SRC_TAMPER = 5,//窃电
    
	RELAY_SRC_RESERVE6 = 6,
	RELAY_SRC_RESERVE7 = 7,
};

/**
  * @brief 继电器请求源优先级
  *
  */
struct __relay_priority_entry
{
    uint8_t open:4;
    uint8_t close:4;
};

/**
  * @brief 继电器请求策略
  *
  */
struct __relay_policy_entry
{
    uint16_t open2ready:2;
    uint16_t open2close:2;
    uint16_t close2open:2;
    uint16_t close2ready:2;
    uint16_t ready2open:2;
    uint16_t ready2close:2;
};

/**
  * @brief 继电器配置
  *
  */
struct __relay_conf
{
    struct __relay_priority_entry   priority[8];
    struct __relay_policy_entry     policy;
};

/**
  * @brief 继电器数据
  *
  */
struct __relay_db
{
    uint8_t                         conf_select;
    enum __relay_req_status         status;
    uint16_t                        requests;
};



/**
  * @brief  disconnect task 的对外接口
  */
struct __disconnect
{
    uint8_t							(*request)(enum __relay_req_source source, enum __relay_req_action action);
};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif /* __TYPES_DISCONNECT_H__ */
