/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TYPES_DISPLAY_H__
#define __TYPES_DISPLAY_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported macro ------------------------------------------------------------*/
//参照 config_display.h 中的显示列表配置
#define DISP_SCROLL				((uint8_t)0) //循显列表
#define DISP_ALTERNATE			((uint8_t)1) //键先列表
#define DISP_DEBUG				((uint8_t)2) //调试列表

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  display task 的对外接口
  */
struct __display
{
    uint8_t                         (*change)(uint8_t channel); //切换当前显示列表
    uint8_t                         (*channel)(void); //获取当前显示列表
    
    struct
    {
        struct
        {
			uint8_t					(*next)(void); //显示下一个显示项
			uint8_t					(*last)(void); //显示上一个显示项
			uint8_t					(*index)(uint8_t val); //跳转到索引号显示项
            
        }                           show;
        
		uint8_t						(*current)(void); //获取当前显示项索引号
        
    }                               list;
    
    struct
    {
        struct
        {
            struct
            {
                uint8_t             (*get)(void); //获取开机全屏时间
                uint8_t             (*set)(uint8_t second); //设置开机全屏时间
                
            }                       start;
            
            struct
            {
                uint8_t             (*get)(void); //获取滚屏时间
                uint8_t             (*set)(uint8_t second); //设置滚屏时间
                
            }                       scroll;
            
            struct
            {
                uint8_t             (*get)(void); //获取背光时间
                uint8_t             (*set)(uint8_t second); //设置背光时间
                
            }                       backlight;
            
        }                           time;
        
        struct
        {
            struct
            {
                uint8_t             (*get)(void); //获取功率小数点显示位数
                uint8_t             (*set)(uint8_t dot); //设置功率小数点显示位数
                
            }                       power;
            
            struct
            {
                uint8_t             (*get)(void); //获取电压小数点显示位数
                uint8_t             (*set)(uint8_t dot); //设置电压小数点显示位数
                
            }                       voltage;
            
            struct
            {
                uint8_t             (*get)(void); //获取电流小数点显示位数
                uint8_t             (*set)(uint8_t dot); //设置电流小数点显示位数
                
            }                       current;
            
            struct
            {
                uint8_t             (*get)(void); //获取电能小数点显示位数
                uint8_t             (*set)(uint8_t dot); //设置电能小数点显示位数
                
            }                       energy;
            
            struct
            {
                uint8_t             (*get)(void); //获取需量小数点显示位数
                uint8_t             (*set)(uint8_t dot); //设置需量小数点显示位数
                
            }                       demand;
            
            struct
            {
                uint8_t             (*get)(void); //获取相角计量数据小数点显示位数
                uint8_t             (*set)(uint8_t dot); //设置相角数据小数点显示位数
                
            }                       angle;
            
            struct
            {
                uint8_t             (*get)(void); //获取频率计量数据小数点显示位数
                uint8_t             (*set)(uint8_t dot); //设置频率数据小数点显示位数
                
            }                       freq;
            
            struct
            {
                uint8_t             (*get)(void); //获取功率因数计量数据小数点显示位数
                uint8_t             (*set)(uint8_t dot); //设置功率因数数据小数点显示位数
                
            }                       pf;
            
        }                           dot;
        
        struct
        {
            uint16_t                (*write)(uint8_t channel, void *id); //将一个数据项添加到channel显示列表中
            uint16_t                (*read)(uint8_t channel, uint16_t index, void *id); //读channel显示列表中index显示项ID
			uint8_t					(*amount)(uint8_t channel); //获取channel显示列表中显示项数目
			uint8_t					(*clean)(uint8_t channel); //清除channel显示列表中显示项
            
        }                           list;
        
    }                               config;
};

/* Exported constants --------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif /* __TYPES_DISPLAY_H__ */
