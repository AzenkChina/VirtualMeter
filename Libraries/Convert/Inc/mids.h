/**
  * @file    	
  * @author  	
  * @version	
  * @date		
  * @brief 		metering data identification system
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MIDS_H__
#define __MIDS_H__

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
  * @brief  计量数据 象限标识
  */
enum __metering_quad
{
    M_QUAD_N = 0x00,//无
	M_QUAD_I = 0x01,//1象限
	M_QUAD_II = 0x02,//2象限
	M_QUAD_III = 0x04,//3象限
    M_QUAD_V = 0x08,//4象限
    
	M_QUAD_NI = 0x10,//减1象限
	M_QUAD_NII = 0x20,//减2象限
	M_QUAD_NIII = 0x40,//减3象限
    M_QUAD_NV = 0x80,//减4象限
};

/**
  * @brief  计量数据 缩放
  */
enum __metering_scale
{
    M_SCALE_N8 = 23,    //x 100000000 -> x100000
	M_SCALE_N7 = 25,    //x 10000000 -> x10000
	M_SCALE_N6 = 25,    //x 1000000 -> x1000
	M_SCALE_N5 = 26,    //x 100000 -> x100
    M_SCALE_N4 = 27,    //x 10000 -> x10
	M_SCALE_N3 = 28,    //x 1000 -> x1
	M_SCALE_N2 = 29,    //x 100 -> x1/10
	M_SCALE_N1 = 30,    //x 10 -> x1/100
    M_SCALE_ZN = 31,    //x 1 -> x1000
    M_SCALE_ZP = 0,     //x 1 -> x1/1000
    M_SCALE_P1 = 1,     //x 1/10 -> x1/10000
    M_SCALE_P2 = 2,     //x 1/100 -> x1/100000
    M_SCALE_P3 = 3,     //x 1/1000 -> x1/1000000
    M_SCALE_P4 = 4,     //x 1/10000 -> x1/10000000
    M_SCALE_P5 = 5,     //x 1/100000 -> x1/100000000
    M_SCALE_P6 = 6,     //x 1/1000000 -> x1/1000000000
    M_SCALE_P7 = 7,     //x 1/10000000 -> x1/10000000000
    M_SCALE_P8 = 8,     //x 1/100000000 -> x1/100000000000
};

/**
  * @brief  计量数据 标识符
  */
struct __metering_identifier
{
    uint32_t item       :5;// enum __metering_item
    uint32_t phase      :3;// enum __metering_phase
    uint32_t quad       :8;// enum __metering_quad
    uint32_t rate       :4;//费率0 ~ 15
    uint32_t scale      :5;//enum __metering_scale
	uint32_t type		:7;//enum __axdr_type
};


/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
//将一个计量数据标识转换成U32 ID
#define M_ID2UINT(i,p,q,r,s,t)      ((uint32_t)\
                                    ((((i)&0x1f)<<27)|\
                                    (((p)&0x07)<<24)|\
                                    (((q)&0xff)<<16)|\
                                    (((r)&0x0f)<<12)|\
                                    (((s)&0x1f)<<7)|\
                                    (((t)&0x7f)<<0)))

//将一个U32 ID转换成计量数据标识
#define M_UINT2ID(val, i)           i.item = (((val)>>27)&0x1f);\
                                    i.phase = (((val)>>24)&0x07);\
                                    i.quad = (((val)>>16)&0xff);\
                                    i.rate = (((val)>>12)&0x0f);\
                                    i.scale = (((val)>>7)&0x1f);\
                                    i.type = (((val)>>0)&0x7f)

//将一个数字写入到U32 ID
#define M_2NUMBER(n)                ((uint32_t)((n)&0x07ffffff))




//判断一个U32 ID是否为计量数据标识
#define M_UINTISID(val)             (((((val)>>27)&0x1f) >= M_P_ENERGY) && ((((val)>>27)&0x1f) <= M_FREQUENCY))

//判断一个U32 ID是否为显示格式
#define M_UINTISFMT(val)            (((((val)>>27)&0x1f) >= FMT_BIN) && ((((val)>>27)&0x1f) <= FMT_STR))

//判断一个U32 ID是否为一个数字
#define M_UINTISNUM(val)            (!((n)&(~0x07ffffff)))




//判断一个U32 ID是否为电能
#define M_UINTISENERGY(val)         (((((val)>>27)&0x1f) >= M_P_ENERGY) && ((((val)>>27)&0x1f) <= M_S_ENERGY))
//判断一个U32 ID是否为功率
#define M_UINTISPOWER(val)          (((((val)>>27)&0x1f) >= M_P_POWER) && ((((val)>>27)&0x1f) <= M_S_POWER))
//判断一个U32 ID是否为电压
#define M_UINTISVOLTAGE(val)        ((((val)>>27)&0x1f) == M_VOLTAGE)
//判断一个U32 ID是否为电流
#define M_UINTISCURRENT(val)        ((((val)>>27)&0x1f) == M_CURRENT)
//判断一个U32 ID是否为功率因数
#define M_UINTISPF(val)             ((((val)>>27)&0x1f) == M_POWER_FACTOR)
//判断一个U32 ID是否为相角
#define M_UINTISANGLE(val)          ((((val)>>27)&0x1f) == M_ANGLE)
//判断一个U32 ID是否为频率
#define M_UINTISFREQ(val)           ((((val)>>27)&0x1f) == M_FREQUENCY)




//缩放数据
#define M_SCALING(val, scale)       switch((enum __metering_scale)((scale)&0x1f)) { \
                                    case M_SCALE_ZP: (val) = (val)/1000;break; \
                                    case M_SCALE_N1: val = (val)/100;break; \
                                    case M_SCALE_N2: (val) = (val)/10;break; \
                                    case M_SCALE_N3: break; \
                                    case M_SCALE_ZN: (val) = (val)*1000;break; \
									default: \
									{if((((scale)&0x1f) >= M_SCALE_P1) && (((scale)&0x1f) <= M_SCALE_P8))\
									{uint8_t i;(val)=(val)/1000;for(i=0; i<((scale)&0x1f); i++){(val)=(val)/10;}}\
									else if((((scale)&0x1f) >= M_SCALE_N8) && (((scale)&0x1f) <= M_SCALE_N4))\
									{uint8_t i; for(i=3; i<((~((scale)&0x1f))&0x1f); i++){(val)=(val)*10;}}}}



/* Exported function prototypes ----------------------------------------------*/


#endif /* __MIDS_H__ */
