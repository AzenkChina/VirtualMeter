/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TYPES_DISCONNECT_H__
#define __TYPES_DISCONNECT_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/

/**
  * @brief 继电器动作
  *
  */
enum __relay_action
{
    RELAY_ACT_RELEASE = 0,//忽略，无操作
    RELAY_ACT_CLOSE = 1,//合闸
    RELAY_ACT_OPEN = 2,//拉闸
};

/**
  * @brief 继电器状态
  *
  */
enum __relay_status
{
    RELAY_STA_CLOSE = 0,//合闸
    RELAY_STA_OPEN = 1,//拉闸
};

/**
  * @brief 继电器请求源，共16个
  *
  */
enum __relay_requests
{
    RELAY_REQ_SLOT_01 = 0,
	RELAY_REQ_SLOT_02,
    RELAY_REQ_SLOT_03,
    RELAY_REQ_SLOT_04,
    RELAY_REQ_SLOT_05,
    RELAY_REQ_SLOT_06,
	RELAY_REQ_SLOT_07,
	RELAY_REQ_SLOT_08,
    RELAY_REQ_SLOT_09,
	RELAY_REQ_SLOT_10,
    RELAY_REQ_SLOT_11,
    RELAY_REQ_SLOT_12,
    RELAY_REQ_SLOT_13,
    RELAY_REQ_SLOT_14,
	RELAY_REQ_SLOT_15,
	RELAY_REQ_SLOT_16,
};

/**
  * @brief  disconnect task 的对外接口
  */
struct __disconnect
{
    enum __relay_action             (*request)(enum __relay_requests source, enum __relay_action action);
    enum __relay_status             (*status)(void);
    enum __relay_requests           (*reason)(void);
	struct
	{
		uint8_t						(*write)(uint8_t selection, const uint8_t *priority);
		uint8_t						(*read)(uint8_t selection, uint8_t *priority);
        
	}								priority;
	struct
	{
		uint8_t                     (*set)(uint8_t selection);
		uint8_t                     (*get)(void);
        
	}								selection;
	struct
	{
		uint32_t                    (*set)(uint32_t mask);
		uint32_t                    (*get)(void);
        
	}								mask;
};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define RELAY_ACCOUNT       RELAY_REQ_SLOT_01
#define RELAY_COMMAND       RELAY_REQ_SLOT_02
#define RELAY_TEST          RELAY_REQ_SLOT_03
#define RELAY_OVERPOWER     RELAY_REQ_SLOT_04
#define RELAY_EVENTS        RELAY_REQ_SLOT_05
#define RELAY_TAMPER        RELAY_REQ_SLOT_06
#define RELAY_TIMER         RELAY_REQ_SLOT_07

/* Exported function prototypes ----------------------------------------------*/

#endif /* __TYPES_DISCONNECT_H__ */
