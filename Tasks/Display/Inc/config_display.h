/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CONFIG_DISPLAY_H__
#define __CONFIG_DISPLAY_H__

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define DISP_CONF_CHANNEL_AMOUNT			((uint8_t)3) //显示列表个数

#define DISP_CONF_CHANNEL_SCROLL			((uint8_t)0) //循显列表
#define DISP_CONF_CHANNEL_ALTERNATE			((uint8_t)1) //键先列表
#define DISP_CONF_CHANNEL_DEBUG				((uint8_t)2) //调试列表

#define DISP_CONF_AMOUNT_IN_LIST			((uint8_t)(128)) //每个显示列表最大支持数据项数目

/* Exported macro ------------------------------------------------------------*/
#define NAME_DISPLAY   "task_display"

/* Exported function prototypes ----------------------------------------------*/

#endif /* __CONFIG_DISPLAY_H__ */
