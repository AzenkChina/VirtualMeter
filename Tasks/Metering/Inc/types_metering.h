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

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  计量数据 类型
  */
enum __metering_item
{
    M_NULL = 0,//未定义
    M_P_ENERGY = 1,//有功电能（mWh）
    M_Q_ENERGY = 2,//无功电能（mVarh）
    M_S_ENERGY = 3,//视在电能（mVAh）
    M_P_POWER = 4,//有功功率（mW）
    M_Q_POWER = 5,//无功功率（mVar）
    M_S_POWER = 6,//视在功率（mVA）
    M_VOLTAGE = 7,//电压（mV）
    M_CURRENT = 8,//电流（mA）
    M_POWER_FACTOR = 9,//功率因数（1/1000）
    M_ANGLE = 10,//相角（1/1000度）
    M_FREQUENCY = 11,//频率（1/1000Hz）
    /** 以上数据的格式是已知的，不需要指定 */
    
/**
  * @brief  当显示列表中添加非计量数据时，显示任务不清楚获取到的数据
  *         需要以怎样的格式来显示，默认显示为十进制，当需要显示为非
  *         十进制格式，则需要在这里显式地指定
  */
    FMT_BIN = 24,//二进制格式
    FMT_HEX = 25,//十六进制格式
    FMT_DATE = 26,//日期格式
    FMT_TIME = 27,//时间格式
    FMT_DTIME = 28,//一年内时间格式
    FMT_BCD = 29,//BCD格式
    FMT_ASCII = 30,//ASCII格式
    FMT_STR = 31,//字符串格式
};

/**
  * @brief  计量数据 象限标识
  */
enum __metering_quad
{
    M_QUAD_N = 0,//无
	M_QUAD_I = 1,//1象限
	M_QUAD_II = 2,//2象限
	M_QUAD_III = 3,//3象限
    M_QUAD_V = 4,//4象限
    
    M_PACT = 5,//正向有功
    M_NACT = 6,//反向有功
    M_PREACT = 7,//正向无功
    M_NREACT = 8,//反向无功
    
    M_DPACT = 9,//正向有功-反向有功
    M_DNACT = 10,//反向有功-正向有功
    M_DPREACT = 11,//正向无功-反向无功
    M_DNREACT = 12,//反向无功-正向无功
    
    M_TACT = 13,//总有功
    M_TREACT = 14,//总无功
};

/**
  * @brief  计量数据 分相标识
  */
enum __metering_phase
{
    M_PHASE_N = 0,//N相
	M_PHASE_A = 1,//A相
	M_PHASE_B = 2,//B相
	M_PHASE_C = 4,//C相
	M_PHASE_AB = 3,//AB
	M_PHASE_AC = 5,//AC
	M_PHASE_BC = 6,//BC
    M_PHASE_T = 7,//总
};

/**
  * @brief  计量数据 标识符
  */
struct __metering_identifier
{
    uint32_t item       :5;// enum __metering_item
    uint32_t quad       :4;// enum __metering_quad
    uint32_t phase      :3;// enum __metering_phase
    uint32_t rate       :4;//费率0 ~ 15
	uint32_t offset		:16;//自定义字段
};




/**
  * @brief  电能计量状态
  */
enum __metering_status
{
    M_ENERGY_STOP = 0,//电能不计量
    M_ENERGY_START,//电能计量
};

/**
  * @brief  metering task 的对外接口
  */
struct __metering
{
    //读取一个当前的数据项
    enum __metering_item                    (*instant)(struct __metering_identifier id, int64_t *val);
    
    //读取最近整分钟冻结的数据项
    enum __metering_item                    (*recent)(struct __metering_identifier id, int64_t *val);
    
    //将一个数据项转换为一次侧值
    //当宏定义 __USE_PRIMARY_ENERGY__ 打开时，这些函数将不对输入的数据做任何处理
    int64_t                                 (*primary)(struct __metering_identifier id, int64_t val);
	
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
            void                            (*enter)(void *args); //开始校准
            void                            (*exit)(void); //退出校准
            
        }                                   calibration;
        
    }                                       config;
    
};


/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
//将一个计量数据标识转换成U32 ID
#define M_ID2UINT(i,q,p,r,o)		((uint32_t)(((i&0x1f)<<27)|((q&0x0f)<<23)|((p&0x07)<<20)|((r&0x0f)<<16))|((o&0xffff)<<0))

//将一个U32 ID转换成计量数据标识
#define M_UINT2ID(val, i)           i.item = ((val>>27)&0x1f);\
                                    i.quad = ((val>>23)&0x0f);\
                                    i.phase = ((val>>20)&0x07);\
                                    i.rate = ((val>>16)&0x0f);\
									i.offset = ((val>>0)&0xffff)


//判断一个U32 ID是否为显示格式
#define M_UINTISFMT(val)            ((((val>>27)&0x1f) >= FMT_BIN) && (((val>>27)&0x1f) <= FMT_STR))

//判断一个U32 ID是否为计量数据标识
#define M_UINTISID(val)             ((((val>>27)&0x1f) >= M_P_ENERGY) && (((val>>27)&0x1f) <= M_FREQUENCY))

//判断一个U32 ID是否为电能
#define M_UINTISENERGY(val)         ((((val>>27)&0x1f) >= M_P_ENERGY) && (((val>>27)&0x1f) <= M_S_ENERGY))
//判断一个U32 ID是否为功率
#define M_UINTISPOWER(val)          ((((val>>27)&0x1f) >= M_P_POWER) && (((val>>27)&0x1f) <= M_S_POWER))
//判断一个U32 ID是否为电压
#define M_UINTISVOLTAGE(val)        (((val>>27)&0x1f) == M_VOLTAGE)
//判断一个U32 ID是否为电流
#define M_UINTISCURRENT(val)        (((val>>27)&0x1f) == M_CURRENT)


/* Exported function prototypes ----------------------------------------------*/

#endif /* __TYPES_METERING_H__ */
