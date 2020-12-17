/**
 * @brief		
 * @details		
 * @date		azenk@2017-01-09
 **/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TYPES_METERING_H__
#define __TYPES_METERING_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "mids.h"

/* Exported types ------------------------------------------------------------*/

/**
  * @brief  电能计量状态
  */
enum __metering_status
{
    M_ENERGY_STOP = 0,//电能不计量
    M_ENERGY_START = 0xff,//电能计量
};

/**
  * @brief  metering task 的对外接口
  */
struct __metering
{
    //读取一个当前的数据项
    enum __meta_item                        (*instant)(struct __meta_identifier id, int64_t *val);
    
    //读取最近整分钟冻结的数据项
    enum __meta_item                        (*recent)(struct __meta_identifier id, int64_t *val);
    
    //将一个数据项转换为一次侧值
    //当宏定义 __USE_PRIMARY_ENERGY__ 打开时，这些函数将不对输入的数据做任何处理
    int64_t                                 (*primary)(struct __meta_identifier id, int64_t val);
	
    struct
    {
        struct
        {
            uint8_t                         (*read)(void); //读取当前运行费率
            uint8_t                         (*change)(uint8_t rate); //设置当前运行费率
            uint8_t                         (*max)(void); //最大支持的费率数 
            
        }                                   rate;
        
        struct
        {
	        struct
	        {
	            uint32_t					(*get)(void); //读取电流变比（除分母之后的）
	            uint32_t					(*set)(uint32_t val); //设置电流变比（分母置1）
	            uint32_t					(*get_num)(void); //读取电流变比分子
	            uint32_t					(*set_num)(uint32_t val); //设置电流变比分子
	            uint32_t					(*get_denum)(void); //读取电流变比分母
	            uint32_t					(*set_denum)(uint32_t val); //设置电流变比分母
	            
	        }								current;
	        
	        struct
	        {
	            uint32_t					(*get)(void); //读取电压变比（除分母之后的）
	            uint32_t					(*set)(uint32_t val); //设置电压变比（分母置1）
	            uint32_t					(*get_num)(void); //读取电压变比分子
	            uint32_t					(*set_num)(uint32_t val); //设置电压变比分子
	            uint32_t					(*get_denum)(void); //读取电压变比分母
	            uint32_t					(*set_denum)(uint32_t val); //设置电压变比分母
	            
	        }								voltage;
	        
        }                                   ratio;
        
        struct
        {
            uint8_t                         (*get_period)(void); //读取滑差时间（分钟）
            uint8_t                         (*set_period)(uint8_t minute); //设置滑差时间（分钟）
            uint8_t                         (*get_multiple)(void); //读取滑差时间倍数（为1时为block demand，否则为sliding demand）
            uint8_t                         (*set_multiple)(uint8_t val); //设置滑差时间倍数
            
			void                            (*clear_current)(void); //清空当前需量
            void                            (*clear_max)(void); //清空最大需量
            
        }                                   demand;
        
        struct
        {
            enum __metering_status          (*start)(void); //开始电能计量
            enum __metering_status          (*stop)(void); //停止电能计量
            enum __metering_status          (*status)(void); //电能计量状态
            void                            (*clear)(void); //电能清零
            
        }                                   energy;
        
        struct
        {
            struct
            {
                uint32_t                    (*get)(void); //读取有功脉冲常数
                uint32_t                    (*set)(uint32_t val); //设置有功脉冲常数
                
            }                               active;
            
            struct
            {
                uint32_t                    (*get)(void); //读取无功脉冲常数
                uint32_t                    (*set)(uint32_t val); //设置无功脉冲常数
                
            }                               reactive;
            
        }                                   div;
        
        struct
        {
            struct
            {
                uint32_t                    (*get)(void); //读取校准电压
                uint32_t                    (*set)(uint32_t val); //设置校准电压
                
            }                               voltage;
            
            struct
            {
                uint32_t                    (*get)(void); //读取校准电流
                uint32_t                    (*set)(uint32_t val); //设置校准电流
                
            }                               current;
			
            void                            (*enter)(void *args); //开始校准
            void                            (*exit)(void); //退出校准
            
        }                                   calibration;
        
    }                                       config;
    
};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported function prototypes ----------------------------------------------*/

#endif /* __TYPES_METERING_H__ */
