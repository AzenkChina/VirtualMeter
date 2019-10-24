/**
  * @file    	
  * @author  	
  * @version	
  * @date		2019-08-18
  * @brief 		meta data identification system
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MIDS_H__
#define __MIDS_H__

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Exported types ------------------------------------------------------------*/
/**
  * @brief  类型
  */
enum __meta_item
{
    M_NULL = 0,//未定义
    M_P_ENERGY = 1,//有功电能（mWh）
    M_Q_ENERGY = 2,//无功电能（mvarh）
    M_S_ENERGY = 3,//视在电能（mVAh）
    M_P_POWER = 4,//有功功率（mW）
    M_Q_POWER = 5,//无功功率（mvar）
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
    FMT_ASCII = 30,//ASCII字符流格式
    FMT_STR = 31,//二进制字符流格式
};

/**
  * @brief  分相标识
  */
enum __meta_phase
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
  * @brief  缩放
  */
enum __meta_scale
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
  * @brief  象限标识
  */
enum __metering_quad
{
    M_QUAD_N = 0x00,//无
	M_QUAD_I = 0x01,//1象限
	M_QUAD_II = 0x02,//2象限
	M_QUAD_III = 0x04,//3象限
    M_QUAD_IV = 0x08,//4象限
    
	M_QUAD_NI = 0x10,//减1象限
	M_QUAD_NII = 0x20,//减2象限
	M_QUAD_NIII = 0x40,//减3象限
    M_QUAD_NIV = 0x80,//减4象限
	
	M_QUAD_DEMAND = 0x100,//标识该数据是需量而不是功率
};

/**
  * @brief  元数据 标识符
  */
struct __meta_identifier
{
    uint32_t item       :5;//分类 enum __meta_item
    uint32_t phase      :3;//分相 enum __meta_phase
    uint32_t rate       :4;//费率 0 ~ 15
    uint32_t scale      :5;//缩放 enum __meta_scale
	uint32_t type		:5;//enum __axdr_type
	uint32_t flex       :10;//当item 为电能或者功率时，解析为 enum __metering_quad 否则自由定义
};


/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
//将一个数据标识转换成U32   i=item p=phase r=rate s=scale t=type f=quad or flexable
#define M_ID2U(i,p,r,s,t,f)         ((uint32_t)\
                                    (((i)&0x1f)<<27)|\
                                    (((p)&0x07)<<24)|\
                                    (((r)&0x0f)<<20)|\
                                    (((s)&0x1f)<<15)|\
                                    (((t)&0x1f)<<10)|\
									(((f)&0x3ff)<<0))

//将一个数字转换成U32
#define M_NUMBER2U(n)               ((uint32_t)((n)&0x07ffffff))

//将一个U32转换成数据标识
#define M_U2ID(val, i)              i.item = (((val)>>27)&0x1f);\
                                    i.phase = (((val)>>24)&0x07);\
                                    i.rate = (((val)>>20)&0x0f);\
                                    i.scale = (((val)>>15)&0x1f);\
                                    i.type = (((val)>>10)&0x1f);\
									i.flex = (((val)>>0)&0x3ff)




//判断一个U32 ID是否为数据标识
#define M_UISID(val)                (((((val)>>27)&0x1f) >= M_P_ENERGY) && ((((val)>>27)&0x1f) <= M_FREQUENCY))

//判断一个U32 ID是否为显示格式
#define M_UISFMT(val)               (((((val)>>27)&0x1f) >= FMT_BIN) && ((((val)>>27)&0x1f) <= FMT_STR))

//判断一个U32 ID是否为一个数字
#define M_UISNUM(val)               (!((n)&(~0x07ffffff)))




//判断一个U32 ID是否为电能
#define M_UISENERGY(val)            (((((val)>>27)&0x1f) >= M_P_ENERGY) && ((((val)>>27)&0x1f) <= M_S_ENERGY))
//判断一个U32 ID是否为功率
#define M_UISPOWER(val)             (((((val)>>27)&0x1f) >= M_P_POWER) && ((((val)>>27)&0x1f) <= M_S_POWER) && (!(((val)&0x3ff) & M_QUAD_DEMAND)))
//判断一个U32 ID是否为需量
#define M_UISDEMAND(val)             (((((val)>>27)&0x1f) >= M_P_POWER) && ((((val)>>27)&0x1f) <= M_S_POWER) && (((val)&0x3ff) & M_QUAD_DEMAND))
//判断一个U32 ID是否为电压
#define M_UISVOLTAGE(val)           ((((val)>>27)&0x1f) == M_VOLTAGE)
//判断一个U32 ID是否为电流
#define M_UISCURRENT(val)           ((((val)>>27)&0x1f) == M_CURRENT)
//判断一个U32 ID是否为功率因数
#define M_UISPF(val)                ((((val)>>27)&0x1f) == M_POWER_FACTOR)
//判断一个U32 ID是否为相角
#define M_UISANGLE(val)             ((((val)>>27)&0x1f) == M_ANGLE)
//判断一个U32 ID是否为频率
#define M_UISFREQ(val)              ((((val)>>27)&0x1f) == M_FREQUENCY)




//缩放数据
//extern int64_t __mids_scaling(int64_t val, enum __meta_scale scale);
//#define M_SCALING(val, scale)       (val = __mids_scaling(val, scale))

#define M_SCALING(val, scale)       switch((enum __meta_scale)((scale)&0x1f)) { \
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
