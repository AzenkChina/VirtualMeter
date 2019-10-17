/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LCD_H__
#define __LCD_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "device.h"

/* Exported define -----------------------------------------------------------*/
#if defined ( _WIN32 ) || defined ( _WIN64 ) || defined ( __linux )

#define LCD_MAX_WINDOWS             (2)//windows数量

#define LCD_WINDOW_MAIN             (0)//主windows
#define LCD_WINDOW_SUB              (1)//副windows



#define LCD_MAX_LABELS              (18)//label数量

#define LCD_LABEL_U1                (0)//符号 U1
#define LCD_LABEL_U2                (1)//符号 U2
#define LCD_LABEL_U3                (2)//符号 U3
#define LCD_LABEL_I1                (3)//符号 I1
#define LCD_LABEL_I2                (4)//符号 I2
#define LCD_LABEL_I3                (5)//符号 I3
#define LCD_LABEL_L1                (6)//符号 L1
#define LCD_LABEL_L2                (7)//符号 L2
#define LCD_LABEL_L3                (8)//符号 L3
#define LCD_LABEL_LN                (9)//符号 LN
#define LCD_LABEL_PF                (10)//符号 PF
#define LCD_LABEL_RELAY             (11)//继电器状态
#define LCD_LABEL_COMM              (12)//通讯符号
#define LCD_LABEL_WARN              (13)//报警符号
#define LCD_LABEL_QUAD              (14)//功率象限
#define LCD_LABEL_RATE              (15)//费率
#define LCD_LABEL_DATE              (16)//日期
#define LCD_LABEL_TIM               (17)//时间

#else

#if defined (DEMO_STM32F091)
#define LCD_MAX_WINDOWS             (2)//windows数量

#define LCD_WINDOW_MAIN             (0)//主windows
#define LCD_WINDOW_SUB              (1)//副windows



#define LCD_MAX_LABELS              (37)//label数量

#define LCD_LABEL_U1                (0)//符号 U1
#define LCD_LABEL_U2                (1)//符号 U2
#define LCD_LABEL_U3                (2)//符号 U3
#define LCD_LABEL_I1                (3)//符号 I1
#define LCD_LABEL_I2                (4)//符号 I2
#define LCD_LABEL_I3                (5)//符号 I3
#define LCD_LABEL_L1                (6)//符号 L1
#define LCD_LABEL_L2                (7)//符号 L2
#define LCD_LABEL_L3                (8)//符号 L3
#define LCD_LABEL_LN                (9)//符号 LN
#define LCD_LABEL_PF                (10)//符号 PF
#define LCD_LABEL_RELAY             (11)//继电器状态
#define LCD_LABEL_COMM              (12)//通讯符号
#define LCD_LABEL_WARN              (13)//报警符号
#define LCD_LABEL_QUAD              (14)//功率象限
#define LCD_LABEL_RATE              (15)//费率
#define LCD_LABEL_DATE              (16)//日期
#define LCD_LABEL_TIM               (17)//时间

//以上 LABEL 为基础 LABEL

#define LCD_LABEL_SIGNAL            (18)//无线信号强度
#define LCD_LABEL_BATRTC            (19)//RTC电池状态
#define LCD_LABEL_BATBAK            (20)//抄表电池状态
#define LCD_LABEL_SECOND            (21)//二次侧标识
#define LCD_LABEL_PRIM				(22)//一次侧标识
#define LCD_LABEL_NET				(23)//电网电源符号
#define LCD_LABEL_AUX				(24)//辅助电源符号
#define LCD_LABEL_PULSE             (25)//心跳符号
#define LCD_LABEL_HOUSE             (26)//室内符号
#define LCD_LABEL_MON               (27)//月份符号
#define LCD_LABEL_NOW               (28)//当月符号
#define LCD_LABEL_LAST              (29)//上月符号
#define LCD_LABEL_TOTAL             (30)//组合符号
#define LCD_LABEL_NEG               (31)//无功符号
#define LCD_LABEL_POS               (32)//有功符号
#define LCD_LABEL_TARIF             (33)//TARIF符号
#define LCD_LABEL_CREDIT            (34)//CREDIT符号
#define LCD_LABEL_ENERGY            (35)//ENERGY符号
#define LCD_LABEL_NUMBER            (36)//NUMBER符号
#endif

#endif




/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/
extern const struct __lcd lcd;

#endif /* __LCD_H__ */
