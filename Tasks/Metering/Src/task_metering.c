/**
 * @brief       
 * @details     
 * @date        2017-01-09
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "task_metering.h"
#include "types_metering.h"
#include "config_metering.h"
#include "crc.h"

#include "meter.h"

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief  计量参数
  */
struct __metering_parameter
{
	uint32_t voltage; //校准电压（mV）
	uint32_t current; //校准电流（mA）
	
	uint32_t active_div; //有功脉冲常数
	uint32_t reactive_div; //无功脉冲常数
	
	uint32_t current_num; //电流变比分子
	uint32_t current_denum; //电流变比分母
	
	uint32_t voltage_num; //电压变比分子
	uint32_t voltage_denum; //电压变比分母
	
	uint8_t demand_period; //滑差时间（min）
	uint8_t demand_multiple;//滑差时间倍数
	
	uint8_t energy_switch;//电能计量开关
	uint8_t rate;//当前费率
	
	uint32_t check;
};

/**
  * @brief  计量数据元
  */
struct __metering_base
{
	struct
	{
		//总 A B C
		uint64_t value[MAX_PHASE];
		uint64_t check;
		
	} group[3];//有功/无功/视在
};

/**
  * @brief  计量数据
  */
struct __metering_data
{
	//费率 相限
	struct __metering_base energy[MAX_RATE][4];
	struct __metering_base demand[MAX_RATE][4];
};

/* Private define ------------------------------------------------------------*/
#define DEV_M			meter

/* Private macro -------------------------------------------------------------*/
#if defined ( __USE_PRIMARY_ENERGY__ ) && !defined ( __USE_64BIT_ENERGY__ )
#define __USE_64BIT_ENERGY__        //当选择使用一次侧数据，则必须选择使用64位数据类型来存储电能
#endif

/* Private variables ---------------------------------------------------------*/
static enum __task_status status = TASK_NOTINIT;

static struct __metering_parameter mp;
static struct __metering_base mc[4];

static uint8_t step = 0;
static uint8_t quad = 0;
static uint8_t group = 0;
static uint16_t period = 0;
static uint16_t flush[4][3] = {0};

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  
  */
static void config_check(void)
{
	uint32_t check = crc32(&mp, (sizeof(mp) - sizeof(mp.check)), 0);
	
	if(check != mp.check)
	{
		if(file.parameter.read("calibration", \
								STRUCT_SIZE(struct __calibrates, data), \
								sizeof(mp), \
								(void *)&mp) != sizeof(mp))
		{
			TRACE(TRACE_ERR, "Task metering config_check 1.");
			
			mp.voltage = 220000;
			mp.current = 1500;
			mp.active_div = 6400;
			mp.reactive_div = 6400;
			mp.current_num = 1;
			mp.current_denum = 1;
			mp.voltage_num = 1;
			mp.voltage_denum = 1;
			mp.demand_period = 5;
			mp.demand_multiple = 3;
			mp.energy_switch = 0xff;
			mp.rate = 0;
			mp.check = crc32(&mp, (sizeof(mp) - sizeof(mp.check)), 0);
			
			file.parameter.write("calibration", STRUCT_SIZE(struct __calibrates, data), sizeof(mp), (void *)&mp);
		}
		else
		{
			check = crc32(&mp, (sizeof(mp) - sizeof(mp.check)), 0);
			if(check != mp.check)
			{
				TRACE(TRACE_ERR, "Task metering config_check 2.");
				
				mp.voltage = 220000;
				mp.current = 1500;
				mp.active_div = 6400;
				mp.reactive_div = 6400;
				mp.current_num = 1;
				mp.current_denum = 1;
				mp.voltage_num = 1;
				mp.voltage_denum = 1;
				mp.demand_period = 5;
				mp.demand_multiple = 3;
				mp.energy_switch = 0xff;
				mp.rate = 0;
				mp.check = crc32(&mp, (sizeof(mp) - sizeof(mp.check)), 0);
				
				file.parameter.write("calibration", STRUCT_SIZE(struct __calibrates, data), sizeof(mp), (void *)&mp);
			}
		}
	}
	
	if(!mp.voltage || !mp.current || !mp.active_div || !mp.reactive_div)
	{
		TRACE(TRACE_ERR, "Task metering config_check 3.");
		
		mp.voltage = 220000;
		mp.current = 1500;
		mp.active_div = 6400;
		mp.reactive_div = 6400;
		mp.check = crc32(&mp, (sizeof(mp) - sizeof(mp.check)), 0);
		
		file.parameter.write("calibration", STRUCT_SIZE(struct __calibrates, data), sizeof(mp), (void *)&mp);
	}
}




/**
  * @brief  
  */
static enum __meta_item metering_instant(struct __meta_identifier id, int64_t *val)
{
    int64_t read = 0;
	uint8_t phase;
	uint8_t group;
	
	config_check();
    
	if(id.item == M_VOLTAGE)//电压
	{
		if(id.phase == M_PHASE_A)
		{
			read = DEV_M.read(R_UA);
			*val = read;
		}
		else if(id.phase == M_PHASE_B)
		{
			read = DEV_M.read(R_UB);
			*val = read;
		}
		else if(id.phase == M_PHASE_C)
		{
			read = DEV_M.read(R_UC);
			*val = read;
		}
		else
		{
			*val = 0;
		}
		
		return(M_VOLTAGE);
	}
	else if(id.item == M_CURRENT)//电流
	{
		if(id.phase == M_PHASE_A)
		{
			read = DEV_M.read(R_IA);
			*val = read;
		}
		else if(id.phase == M_PHASE_B)
		{
			read = DEV_M.read(R_IB);
			*val = read;
		}
		else if(id.phase == M_PHASE_C)
		{
			read = DEV_M.read(R_IC);
			*val = read;
		}
		else if(id.phase == M_PHASE_N)
		{
			read = DEV_M.read(R_I0);
			*val = read;
		}
		else if(id.phase == M_PHASE_T)
		{
			read = DEV_M.read(R_IT);
			*val = read;
		}
		else
		{
			*val = 0;
		}
		
		return(M_CURRENT);
	}
	else if(id.item == M_POWER_FACTOR)//功率因数
	{
		if(id.phase == M_PHASE_A)
		{
			read = DEV_M.read(R_PFA);
			*val = read;
		}
		else if(id.phase == M_PHASE_B)
		{
			read = DEV_M.read(R_PFB);
			*val = read;
		}
		else if(id.phase == M_PHASE_C)
		{
			read = DEV_M.read(R_PFC);
			*val = read;
		}
		else if(id.phase == M_PHASE_T)
		{
			read = DEV_M.read(R_PFT);
			*val = read;
		}
		else
		{
			*val = 0;
		}
		
		return(M_POWER_FACTOR);
	}
	else if(id.item == M_ANGLE)//相角
	{
		if(id.phase == M_PHASE_A)
		{
			read = DEV_M.read(R_YIA);
			*val = read;
		}
		else if(id.phase == M_PHASE_B)
		{
			read = DEV_M.read(R_YIB);
			*val = read;
		}
		else if(id.phase == M_PHASE_C)
		{
			read = DEV_M.read(R_YIC);
			*val = read;
		}
		else if(id.phase == M_PHASE_AB)
		{
			read = DEV_M.read(R_YUAUB);
			*val = read;
		}
		else if(id.phase == M_PHASE_AC)
		{
			read = DEV_M.read(R_YUAUC);
			*val = read;
		}
		else if(id.phase == M_PHASE_BC)
		{
			read = DEV_M.read(R_YUBUC);
			*val = read;
		}
		else
		{
			*val = 0;
		}
		
		return(M_ANGLE);
	}
	else if(id.item == M_FREQUENCY)//频率
	{
		read = DEV_M.read(R_FREQ);
		*val = read;
		
		return(M_FREQUENCY);
	}
	else if((id.item == M_P_POWER) && !(id.flex & M_QUAD_DEMAND))//有功功率
	{
		int32_t reactive;
		
		if(id.phase == M_PHASE_A)
		{
			read = DEV_M.read(R_PA);
		}
		else if(id.phase == M_PHASE_B)
		{
			read = DEV_M.read(R_PB);
		}
		else if(id.phase == M_PHASE_C)
		{
			read = DEV_M.read(R_PC);
		}
		else if(id.phase == M_PHASE_T)
		{
			read = DEV_M.read(R_PT);
		}
		else
		{
			read = 0;
		}
		
		if(read)
		{
			if(id.phase == M_PHASE_A)
			{
				reactive = DEV_M.read(R_QA);
			}
			else if(id.phase == M_PHASE_B)
			{
				reactive = DEV_M.read(R_QB);
			}
			else if(id.phase == M_PHASE_C)
			{
				reactive = DEV_M.read(R_QC);
			}
			else if(id.phase == M_PHASE_T)
			{
				reactive = DEV_M.read(R_QT);
			}
			else
			{
				reactive = 0;
			}
			
			if(read >= 0)
			{
				if(reactive >= 0)
				{
					//第一相限
					if(id.flex & M_QUAD_I)
					{
						*val = read;
					}
				}
				else
				{
					//第四相限
					if(id.flex & M_QUAD_IV)
					{
						*val = read;
					}
				}
			}
			else
			{
				if(reactive >= 0)
				{
					//第二相限
					if(id.flex & M_QUAD_II)
					{
						*val = read;
					}
				}
				else
				{
					//第三相限
					if(id.flex & M_QUAD_III)
					{
						*val = read;
					}
				}
			}
		}
		else
		{
			*val = 0;
		}
		
		return(M_P_POWER);
	}
	else if((id.item == M_Q_POWER) && !(id.flex & M_QUAD_DEMAND))//无功功率
	{
		int32_t active;
		
		if(id.phase == M_PHASE_A)
		{
			read = DEV_M.read(R_QA);
		}
		else if(id.phase == M_PHASE_B)
		{
			read = DEV_M.read(R_QB);
		}
		else if(id.phase == M_PHASE_C)
		{
			read = DEV_M.read(R_QC);
		}
		else if(id.phase == M_PHASE_T)
		{
			read = DEV_M.read(R_QT);
		}
		else
		{
			read = 0;
		}
		
		if(read)
		{
			if(id.phase == M_PHASE_A)
			{
				active = DEV_M.read(R_PA);
			}
			else if(id.phase == M_PHASE_B)
			{
				active = DEV_M.read(R_PB);
			}
			else if(id.phase == M_PHASE_C)
			{
				active = DEV_M.read(R_PC);
			}
			else if(id.phase == M_PHASE_T)
			{
				active = DEV_M.read(R_PT);
			}
			else
			{
				active = 0;
			}
			
			if(active >= 0)
			{
				if(read >= 0)
				{
					//第一相限
					if(id.flex & M_QUAD_I)
					{
						*val = read;
					}
				}
				else
				{
					//第四相限
					if(id.flex & M_QUAD_IV)
					{
						*val = read;
					}
				}
			}
			else
			{
				if(read >= 0)
				{
					//第二相限
					if(id.flex & M_QUAD_II)
					{
						*val = read;
					}
				}
				else
				{
					//第三相限
					if(id.flex & M_QUAD_III)
					{
						*val = read;
					}
				}
			}
		}
		else
		{
			*val = 0;
		}
		
		return(M_Q_POWER);
	}
	else if((id.item == M_S_POWER) && !(id.flex & M_QUAD_DEMAND))//视在功率
	{
		int32_t active;
		int32_t reactive;
		
		if(id.phase == M_PHASE_A)
		{
			read = DEV_M.read(R_SA);
		}
		else if(id.phase == M_PHASE_B)
		{
			read = DEV_M.read(R_SB);
		}
		else if(id.phase == M_PHASE_C)
		{
			read = DEV_M.read(R_SC);
		}
		else if(id.phase == M_PHASE_T)
		{
			read = DEV_M.read(R_ST);
		}
		else
		{
			read = 0;
		}
		
		if(read)
		{
			if(id.phase == M_PHASE_A)
			{
				active = DEV_M.read(R_PA);
				reactive = DEV_M.read(R_QA);
			}
			else if(id.phase == M_PHASE_B)
			{
				active = DEV_M.read(R_PB);
				reactive = DEV_M.read(R_QB);
			}
			else if(id.phase == M_PHASE_C)
			{
				active = DEV_M.read(R_PC);
				reactive = DEV_M.read(R_QC);
			}
			else if(id.phase == M_PHASE_T)
			{
				active = DEV_M.read(R_PT);
				reactive = DEV_M.read(R_QT);
			}
			else
			{
				active = 0;
				reactive = 0;
			}
			
			if(active >= 0)
			{
				if(reactive >= 0)
				{
					//第一相限
					if(id.flex & M_QUAD_I)
					{
						*val = read;
					}
				}
				else
				{
					//第四相限
					if(id.flex & M_QUAD_IV)
					{
						*val = read;
					}
				}
			}
			else
			{
				if(reactive >= 0)
				{
					//第二相限
					if(id.flex & M_QUAD_II)
					{
						*val = read;
					}
				}
				else
				{
					//第三相限
					if(id.flex & M_QUAD_III)
					{
						*val = read;
					}
				}
			}
		}
		else
		{
			*val = 0;
		}
		
		return(M_S_POWER);
	}
	else if((id.item == M_P_POWER) && (id.flex & M_QUAD_DEMAND))//有功需量
	{
		//...TODO
	}
	else if((id.item == M_Q_POWER) && (id.flex & M_QUAD_DEMAND))//无功需量
	{
		//...TODO
	}
	else if((id.item == M_S_POWER) && (id.flex & M_QUAD_DEMAND))//视在需量
	{
		//...TODO
	}
	else if((id.item == M_P_ENERGY) || (id.item == M_Q_ENERGY) || (id.item == M_S_ENERGY))//电能
	{
		struct __metering_base tmc[4];
		struct __metering_base *pmc;
		
		if(id.phase == M_PHASE_T)
		{
			phase = 0;
		}
		else if(id.phase == M_PHASE_A)
		{
			phase = 1;
		}
		else if(id.phase == M_PHASE_B)
		{
			phase = 2;
		}
		else if(id.phase == M_PHASE_C)
		{
			phase = 3;
		}
		else
		{
			*val = 0;
			return((enum __meta_item)(id.item));
		}
		
		if(id.item == M_P_ENERGY)
		{
			group = 0;
		}
		else if(id.item == M_Q_ENERGY)
		{
			group = 1;
		}
		else if(id.item == M_S_ENERGY)
		{
			group = 2;
		}
		else
		{
			*val = 0;
			return((enum __meta_item)(id.item));
		}
		
		if(id.rate == mp.rate)
		{
			for(uint8_t n=0; n<4; n++)
			{
				if(id.flex & M_QUAD_I)
				{
					read += mc[0].group[group].value[phase];
				}
				if(id.flex & M_QUAD_II)
				{
					read += mc[1].group[group].value[phase];
				}
				if(id.flex & M_QUAD_III)
				{
					read += mc[2].group[group].value[phase];
				}
				if(id.flex & M_QUAD_IV)
				{
					read += mc[3].group[group].value[phase];
				}
				
				if(id.flex & M_QUAD_NI)
				{
					read -= mc[0].group[group].value[phase];
				}
				if(id.flex & M_QUAD_NII)
				{
					read -= mc[1].group[group].value[phase];
				}
				if(id.flex & M_QUAD_NIII)
				{
					read -= mc[2].group[group].value[phase];
				}
				if(id.flex & M_QUAD_NIV)
				{
					read -= mc[3].group[group].value[phase];
				}
			}
		}
		else if(id.rate < MAX_RATE)
		{
			if(file.parameter.read("measurements", \
									STRUCT_OFFSET(struct __metering_data, energy[id.rate]), \
									sizeof(tmc), \
									(void *)tmc) != sizeof(tmc))
			{
				TRACE(TRACE_ERR, "Task metering metering_instant.");
				*val = 0;
				return((enum __meta_item)(id.item));
			}
			
			for(uint8_t n=0; n<4; n++)
			{
				if(id.flex & M_QUAD_I)
				{
					read += tmc[0].group[group].value[phase];
				}
				if(id.flex & M_QUAD_II)
				{
					read += tmc[1].group[group].value[phase];
				}
				if(id.flex & M_QUAD_III)
				{
					read += tmc[2].group[group].value[phase];
				}
				if(id.flex & M_QUAD_IV)
				{
					read += tmc[3].group[group].value[phase];
				}
				
				if(id.flex & M_QUAD_NI)
				{
					read -= tmc[0].group[group].value[phase];
				}
				if(id.flex & M_QUAD_NII)
				{
					read -= tmc[1].group[group].value[phase];
				}
				if(id.flex & M_QUAD_NIII)
				{
					read -= tmc[2].group[group].value[phase];
				}
				if(id.flex & M_QUAD_NIV)
				{
					read -= tmc[3].group[group].value[phase];
				}
			}
		}
		else
		{
			for(uint8_t r=0; r<MAX_RATE; r++)
			{
				if(r != mp.rate)
				{
					if(file.parameter.read("measurements", \
											STRUCT_OFFSET(struct __metering_data, energy[r]), \
											sizeof(tmc), \
											(void *)tmc) != sizeof(tmc))
					{
						TRACE(TRACE_ERR, "Task metering metering_instant.");
						memset(tmc, 0, sizeof(tmc));
					}
					
					pmc = tmc;
				}
				else
				{
					pmc = mc;
				}
				
				for(uint8_t n=0; n<4; n++)
				{
					if(id.flex & M_QUAD_I)
					{
						read += pmc[0].group[group].value[phase];
					}
					if(id.flex & M_QUAD_II)
					{
						read += pmc[1].group[group].value[phase];
					}
					if(id.flex & M_QUAD_III)
					{
						read += pmc[2].group[group].value[phase];
					}
					if(id.flex & M_QUAD_IV)
					{
						read += pmc[3].group[group].value[phase];
					}
					
					if(id.flex & M_QUAD_NI)
					{
						read -= pmc[0].group[group].value[phase];
					}
					if(id.flex & M_QUAD_NII)
					{
						read -= pmc[1].group[group].value[phase];
					}
					if(id.flex & M_QUAD_NIII)
					{
						read -= pmc[2].group[group].value[phase];
					}
					if(id.flex & M_QUAD_NIV)
					{
						read -= pmc[3].group[group].value[phase];
					}
				}
			}
		}
		
		if(id.item == M_Q_ENERGY)
		{
			read = read * 1000 / mp.reactive_div;
		}
		else
		{
			read = read * 1000 / mp.active_div;
		}
		
		*val = read;
		return((enum __meta_item)(id.item));
	}
	
	return(M_NULL);
}

/**
  * @brief  
  */
static enum __meta_item metering_recent(struct __meta_identifier id, int64_t *val)
{
	return(M_NULL);
}

/**
  * @brief  
  */
static double metering_primary(struct __meta_identifier id, int64_t val)
{
	double result;

	config_check();
	
	if(id.item == M_VOLTAGE)
	{
		result = (double)val * mp.voltage_denum / mp.voltage_num;
	}
	else if(id.item == M_CURRENT)
	{
		result = (double)val * mp.current_denum / mp.current_num;
	}
	else if((id.item >= M_P_ENERGY) && (id.item <= M_S_POWER))
	{
		result = (double)val * mp.voltage_denum / mp.voltage_num;
		result = result * mp.current_denum / mp.current_num;
	}

	return(result);
}





/**
  * @brief  
  */
static uint8_t config_rate_read(void)
{
	config_check();
	return(mp.rate);
}

/**
  * @brief  
  */
static uint8_t config_rate_change(uint8_t rate)
{
	uint8_t previous;
	config_check();
	previous = mp.rate;
	mp.rate = rate % (MAX_RATE + 1);
	mp.check = crc32(&mp, (sizeof(mp) - sizeof(mp.check)), 0);
	
	if(file.parameter.write("calibration", \
							STRUCT_SIZE(struct __calibrates, data) + STRUCT_OFFSET(struct __metering_parameter, rate), \
							sizeof(mp.rate), \
							(void *)&(mp.rate)) != sizeof(mp.rate))
	{
		TRACE(TRACE_ERR, "Task metering config_rate_change.");
	}
	
	if(mp.rate == previous)
	{
		return(mp.rate);
	}
	
	//写回当前费率的计量数据
	for(uint8_t n=0; n<4; n++)
	{
		for(uint8_t g=0; g<3; g++)
		{
			if(!flush[n][g])
			{
				continue;
			}
			
			if(file.parameter.write("measurements", \
									STRUCT_OFFSET(struct __metering_data, energy[mp.rate][n].group[g]), \
									sizeof(mc[n].group[g]), \
									(void *)&(mc[n].group[g])) != sizeof(mc[n].group[g]))
			{
				TRACE(TRACE_ERR, "Task metering metering_loop.");
			}
			
			flush[n][g] = 0;
		}
	}
	
	//读出新费率的计量数据
	if(file.parameter.read("measurements", \
							STRUCT_OFFSET(struct __metering_data, energy[mp.rate]), \
							sizeof(mc), \
							(void *)mc) != sizeof(mc))
	{
		TRACE(TRACE_ERR, "Task metering config_rate_change 2.");
	}
	
	return(mp.rate);
}

/**
  * @brief  
  */
static uint8_t config_rate_max(void)
{
	config_check();
	return(MAX_RATE);
}

/**
  * @brief  
  */
static uint32_t config_current_ratio_get(void)
{
	config_check();
	if(!mp.current_denum)
	{
		mp.current_denum = 1;
		mp.check = crc32(&mp, (sizeof(mp) - sizeof(mp.check)), 0);
	}
	return(mp.current_num / mp.current_denum);
}

/**
  * @brief  
  */
static uint32_t config_current_ratio_set(uint32_t val)
{
	if(val > 10000)
	{
		return(mp.current_num);
	}
	config_check();
	mp.current_num = val;
	mp.current_denum = 1;
	mp.check = crc32(&mp, (sizeof(mp) - sizeof(mp.check)), 0);
	
	if(file.parameter.write("calibration", \
							STRUCT_SIZE(struct __calibrates, data) + STRUCT_OFFSET(struct __metering_parameter, current_num), \
							sizeof(mp.current_num), \
							(void *)&(mp.current_num)) != sizeof(mp.current_num))
	{
		TRACE(TRACE_ERR, "Task metering config_current_ratio_set 1.");
	}
	
	if(file.parameter.write("calibration", \
							STRUCT_SIZE(struct __calibrates, data) + STRUCT_OFFSET(struct __metering_parameter, current_denum), \
							sizeof(mp.current_denum), \
							(void *)&(mp.current_denum)) != sizeof(mp.current_denum))
	{
		TRACE(TRACE_ERR, "Task metering config_current_ratio_set 2.");
	}
	
	return(mp.current_num);
}

/**
  * @brief  
  */
static uint32_t config_current_ratio_get_num(void)
{
	config_check();
	return(mp.current_num);
}

/**
  * @brief  
  */
static uint32_t config_current_ratio_set_num(uint32_t val)
{
	config_check();
	mp.current_num = val;
	
	if(!mp.current_denum)
	{
		mp.current_denum = 1;
	}
	
	mp.check = crc32(&mp, (sizeof(mp) - sizeof(mp.check)), 0);
	
	if(file.parameter.write("calibration", \
							STRUCT_SIZE(struct __calibrates, data) + STRUCT_OFFSET(struct __metering_parameter, current_num), \
							sizeof(mp.current_num), \
							(void *)&(mp.current_num)) != sizeof(mp.current_num))
	{
		TRACE(TRACE_ERR, "Task metering config_current_ratio_set_num.");
	}
	return(mp.current_num);
}

/**
  * @brief  
  */
static uint32_t config_current_ratio_get_denum(void)
{
	config_check();
	if(!mp.current_denum)
	{
		mp.current_denum = 1;
		mp.check = crc32(&mp, (sizeof(mp) - sizeof(mp.check)), 0);
	}
	return(mp.current_denum);
}

/**
  * @brief  
  */
static uint32_t config_current_ratio_set_denum(uint32_t val)
{
	config_check();
	if(!val)
	{
		val = 1;
	}
	mp.current_denum = val;
	
	mp.check = crc32(&mp, (sizeof(mp) - sizeof(mp.check)), 0);
	
	if(file.parameter.write("calibration", \
							STRUCT_SIZE(struct __calibrates, data) + STRUCT_OFFSET(struct __metering_parameter, current_denum), \
							sizeof(mp.current_denum), \
							(void *)&(mp.current_denum)) != sizeof(mp.current_denum))
	{
		TRACE(TRACE_ERR, "Task metering config_current_ratio_set_denum.");
	}
	return(mp.current_denum);
}


/**
  * @brief  
  */
static uint32_t config_voltage_ratio_get(void)
{
	config_check();
	if(!mp.voltage_denum)
	{
		mp.voltage_denum = 1;
		mp.check = crc32(&mp, (sizeof(mp) - sizeof(mp.check)), 0);
	}
	return(mp.voltage_num / mp.voltage_denum);
}

/**
  * @brief  
  */
static uint32_t config_voltage_ratio_set(uint32_t val)
{
	if(val > 10000)
	{
		return(mp.voltage_num);
	}
	config_check();
	mp.voltage_num = val;
	mp.voltage_denum = 1;
	
	mp.check = crc32(&mp, (sizeof(mp) - sizeof(mp.check)), 0);
	
	if(file.parameter.write("calibration", \
							STRUCT_SIZE(struct __calibrates, data) + STRUCT_OFFSET(struct __metering_parameter, voltage_num), \
							sizeof(mp.voltage_num), \
							(void *)&(mp.voltage_num)) != sizeof(mp.voltage_num))
	{
		TRACE(TRACE_ERR, "Task metering config_voltage_ratio_set 1.");
	}
	
	if(file.parameter.write("calibration", \
							STRUCT_SIZE(struct __calibrates, data) + STRUCT_OFFSET(struct __metering_parameter, voltage_denum), \
							sizeof(mp.voltage_denum), \
							(void *)&(mp.voltage_denum)) != sizeof(mp.voltage_denum))
	{
		TRACE(TRACE_ERR, "Task metering config_voltage_ratio_set 2.");
	}
	
	return(mp.voltage_num);
}

/**
  * @brief  
  */
static uint32_t config_voltage_ratio_get_num(void)
{
	config_check();
	return(mp.voltage_num);
}

/**
  * @brief  
  */
static uint32_t config_voltage_ratio_set_num(uint32_t val)
{
	config_check();
	mp.voltage_num = val;
	
	if(!mp.voltage_denum)
	{
		mp.voltage_denum = 1;
	}
	
	mp.check = crc32(&mp, (sizeof(mp) - sizeof(mp.check)), 0);
	
	if(file.parameter.write("calibration", \
							STRUCT_SIZE(struct __calibrates, data) + STRUCT_OFFSET(struct __metering_parameter, voltage_num), \
							sizeof(mp.voltage_num), \
							(void *)&(mp.voltage_num)) != sizeof(mp.voltage_num))
	{
		TRACE(TRACE_ERR, "Task metering config_voltage_ratio_set_num.");
	}
	return(mp.voltage_num);
}

/**
  * @brief  
  */
static uint32_t config_voltage_ratio_get_denum(void)
{
	config_check();
	if(!mp.voltage_denum)
	{
		mp.voltage_denum = 1;
		mp.check = crc32(&mp, (sizeof(mp) - sizeof(mp.check)), 0);
	}
	return(mp.voltage_denum);
}

/**
  * @brief  
  */
static uint32_t config_voltage_ratio_set_denum(uint32_t val)
{
	config_check();
	if(!val)
	{
		val = 1;
	}
	mp.voltage_denum = val;
	
	mp.check = crc32(&mp, (sizeof(mp) - sizeof(mp.check)), 0);
	
	if(file.parameter.write("calibration", \
							STRUCT_SIZE(struct __calibrates, data) + STRUCT_OFFSET(struct __metering_parameter, voltage_denum), \
							sizeof(mp.voltage_denum), \
							(void *)&(mp.voltage_denum)) != sizeof(mp.voltage_denum))
	{
		TRACE(TRACE_ERR, "Task metering config_voltage_ratio_set_denum.");
	}
	return(mp.voltage_denum);
}

/**
  * @brief  
  */
static enum __metering_status config_energy_start(void)
{
	config_check();
	mp.energy_switch = 0xff;
	mp.check = crc32(&mp, (sizeof(mp) - sizeof(mp.check)), 0);
	
	if(file.parameter.write("calibration", \
							STRUCT_SIZE(struct __calibrates, data) + STRUCT_OFFSET(struct __metering_parameter, energy_switch), \
							sizeof(mp.energy_switch), \
							(void *)&(mp.energy_switch)) != sizeof(mp.energy_switch))
	{
		TRACE(TRACE_ERR, "Task metering config_energy_start.");
	}
	return(M_ENERGY_START);
}

/**
  * @brief  
  */
static enum __metering_status config_energy_stop(void)
{
	config_check();
	mp.energy_switch = 0;
	mp.check = crc32(&mp, (sizeof(mp) - sizeof(mp.check)), 0);
	
	if(file.parameter.write("calibration", \
							STRUCT_SIZE(struct __calibrates, data) + STRUCT_OFFSET(struct __metering_parameter, energy_switch), \
							sizeof(mp.energy_switch), \
							(void *)&(mp.energy_switch)) != sizeof(mp.energy_switch))
	{
		TRACE(TRACE_ERR, "Task metering config_energy_stop.");
	}
	return(M_ENERGY_STOP);
}

/**
  * @brief  
  */
static enum __metering_status config_energy_status(void)
{
	config_check();
	if(mp.energy_switch)
	{
		return(M_ENERGY_START);
	}
	else
	{
		return(M_ENERGY_STOP);
	}
}

/**
  * @brief  
  */
static void config_energy_clear(void)
{
	heap.set(flush, 0, sizeof(flush));
	heap.set(mc, 0, sizeof(mc));
	
	for(uint8_t n=0; n<4; n++)
	{
		for(uint8_t g=0; g<3; g++)
		{
			mc[n].group[g].check = crc32(&mc[n], (sizeof(mc[n].group[g]) - sizeof(mc[n].group[g].check)), 0);
		}
	}
	
	for(uint8_t n=0; n<MAX_RATE; n++)
	{
		if(file.parameter.write("measurements", \
								STRUCT_OFFSET(struct __metering_data, energy[n]), \
								sizeof(mc), \
								(void *)mc) != sizeof(mc))
		{
			TRACE(TRACE_ERR, "Task metering metering_loop.");
		}
	}
}


/**
  * @brief  
  */
static uint8_t config_demand_get_period(void)
{
	config_check();
	if(!mp.demand_period)
	{
		mp.demand_period = 1;
		mp.check = crc32(&mp, (sizeof(mp) - sizeof(mp.check)), 0);
	}
	return(mp.demand_period);
}

static uint8_t config_demand_set_period(uint8_t minute)
{
	config_check();
	mp.demand_period = minute;
	if(!mp.demand_period)
	{
		mp.demand_period = 1;
	}
	
	mp.check = crc32(&mp, (sizeof(mp) - sizeof(mp.check)), 0);
	
	if(file.parameter.write("calibration", \
							STRUCT_SIZE(struct __calibrates, data) + STRUCT_OFFSET(struct __metering_parameter, demand_period), \
							sizeof(mp.demand_period), \
							(void *)&(mp.demand_period)) != sizeof(mp.demand_period))
	{
		TRACE(TRACE_ERR, "Task metering config_demand_set_period.");
	}
	return(mp.demand_period);
}

/**
  * @brief  
  */
static uint8_t config_demand_get_multiple(void)
{
	config_check();
	if(!mp.demand_multiple)
	{
		mp.demand_multiple = 5;
		mp.check = crc32(&mp, (sizeof(mp) - sizeof(mp.check)), 0);
	}
	return(mp.demand_multiple);
}

/**
  * @brief  
  */
static uint8_t config_demand_set_multiple(uint8_t val)
{
	config_check();
	mp.demand_multiple = val;
	if(!mp.demand_multiple)
	{
		mp.demand_multiple = 5;
	}
	
	mp.check = crc32(&mp, (sizeof(mp) - sizeof(mp.check)), 0);
	
	if(file.parameter.write("calibration", \
							STRUCT_SIZE(struct __calibrates, data) + STRUCT_OFFSET(struct __metering_parameter, demand_multiple), \
							sizeof(mp.demand_multiple), \
							(void *)&(mp.demand_multiple)) != sizeof(mp.demand_multiple))
	{
		TRACE(TRACE_ERR, "Task metering config_demand_set_multiple.");
	}
	return(mp.demand_multiple);
}

/**
  * @brief  
  */
static void config_demand_clear_current(void)
{
	
}

/**
  * @brief  
  */
static void config_demand_clear_max(void)
{
	
}


/**
  * @brief  
  */
static uint32_t config_active_div_get(void)
{
	config_check();
	return(mp.active_div);
}

/**
  * @brief  
  */
static uint32_t config_active_div_set(uint32_t val)
{
	config_check();
	mp.active_div = val;
	mp.check = crc32(&mp, (sizeof(mp) - sizeof(mp.check)), 0);
	
	if(file.parameter.write("calibration", \
							STRUCT_SIZE(struct __calibrates, data) + STRUCT_OFFSET(struct __metering_parameter, active_div), \
							sizeof(mp.active_div), \
							(void *)&(mp.active_div)) != sizeof(mp.active_div))
	{
		TRACE(TRACE_ERR, "Task metering config_active_div_set.");
	}
	return(mp.active_div);
}


/**
  * @brief  
  */
static uint32_t config_reactive_div_get(void)
{
	config_check();
	return(mp.reactive_div);
}

static uint32_t config_reactive_div_set(uint32_t val)
{
	config_check();
	mp.reactive_div = val;
	mp.check = crc32(&mp, (sizeof(mp) - sizeof(mp.check)), 0);
	
	if(file.parameter.write("calibration", \
							STRUCT_SIZE(struct __calibrates, data) + STRUCT_OFFSET(struct __metering_parameter, reactive_div), \
							sizeof(mp.reactive_div), \
							(void *)&(mp.reactive_div)) != sizeof(mp.reactive_div))
	{
		TRACE(TRACE_ERR, "Task metering config_reactive_div_set.");
	}
	return(mp.reactive_div);
}

/**
  * @brief  
  */
static uint32_t config_calibration_voltage_get(void)
{
	config_check();
	return(mp.voltage);
}

/**
  * @brief  
  */
static uint32_t config_calibration_voltage_set(uint32_t val)
{
	config_check();
	mp.voltage = val;
	mp.check = crc32(&mp, (sizeof(mp) - sizeof(mp.check)), 0);
	
	if(file.parameter.write("calibration", \
							STRUCT_SIZE(struct __calibrates, data) + STRUCT_OFFSET(struct __metering_parameter, voltage), \
							sizeof(mp.voltage), \
							(void *)&(mp.voltage)) != sizeof(mp.voltage))
	{
		TRACE(TRACE_ERR, "Task metering config_calibration_voltage_set.");
	}
	return(mp.voltage);
}

/**
  * @brief  
  */
static uint32_t config_calibration_current_get(void)
{
	config_check();
	return(mp.current);
}

/**
  * @brief  
  */
static uint32_t config_calibration_current_set(uint32_t val)
{
	config_check();
	mp.current = val;
	mp.check = crc32(&mp, (sizeof(mp) - sizeof(mp.check)), 0);
	
	if(file.parameter.write("calibration", \
							STRUCT_SIZE(struct __calibrates, data) + STRUCT_OFFSET(struct __metering_parameter, current), \
							sizeof(mp.current), \
							(void *)&(mp.current)) != sizeof(mp.current))
	{
		TRACE(TRACE_ERR, "Task metering config_calibration_current_set.");
	}
	return(mp.current);
}

/**
  * @brief  
  */
static void config_calibration_enter(void *args)
{
	uint8_t *step = args;
    struct __calibrates *calibrates = heap.dzalloc(sizeof(struct __calibrates));
    
    if(!calibrates)
    {
        return;
    }
	
	config_check();
    
	//读出校准数据
	file.parameter.read("calibration", 0, sizeof(calibrates->data), (void *)&(calibrates->data));
    
	//更新校准步骤与信息
	calibrates->param.step = *step;
    calibrates->param.voltage = mp.voltage;
    calibrates->param.current = mp.current;
    calibrates->param.ppulse = mp.active_div;
    calibrates->param.qpulse = mp.reactive_div;
	
	//开始校准
    DEV_M.calibrate.enter(sizeof(struct __calibrates), (void *)calibrates);
	
	//写回校准数据
    file.parameter.write("calibration", 0, sizeof(calibrates->data), (void *)&(calibrates->data));
    
    heap.free(calibrates);
}

/**
  * @brief  
  */
static void config_calibration_exit(void)
{
    DEV_M.calibrate.exit();
}




/**
  * @brief  
  */
static const struct __metering metering = 
{
	.instant								= metering_instant,
	.recent									= metering_recent,
	.primary                                = metering_primary,
	
    .config									=
    {
        .rate								=
        {
			.read							= config_rate_read,
			.change							= config_rate_change,
			.max							= config_rate_max,
        },
        
        .ratio								=
        {
	        .current						=
	        {
				.get						= config_current_ratio_get,
				.set						= config_current_ratio_set,
				.get_num					= config_current_ratio_get_num,
				.set_num					= config_current_ratio_set_num,
				.get_denum					= config_current_ratio_get_denum,
				.set_denum					= config_current_ratio_set_denum,
	        },
	        
	        .voltage						=
	        {
				.get						= config_voltage_ratio_get,
				.set						= config_voltage_ratio_set,
				.get_num					= config_voltage_ratio_get_num,
				.set_num					= config_voltage_ratio_set_num,
				.get_denum					= config_voltage_ratio_get_denum,
				.set_denum					= config_voltage_ratio_set_denum,
	        },
        },
        
        .demand								=
        {
			.get_period						= config_demand_get_period,
			.set_period						= config_demand_set_period,
			.get_multiple					= config_demand_get_multiple,
			.set_multiple					= config_demand_set_multiple,
			.clear_current					= config_demand_clear_current,
			.clear_max						= config_demand_clear_max,
        },
        
        .energy								=
        {
            .start							= config_energy_start,
            .stop							= config_energy_stop,
			.status							= config_energy_status,
            .clear							= config_energy_clear,
        },
        
        .div								=
        {
            .active                         =
            {
                .get                        = config_active_div_get,
                .set                        = config_active_div_set,
            },
            
            .reactive                       =
            {
                .get                        = config_reactive_div_get,
                .set                        = config_reactive_div_set,
            },
        },
        
        .calibration                        = 
        {
            .voltage						=
            {
                .get                        = config_calibration_voltage_get,
                .set                        = config_calibration_voltage_set,
            },
            
            .current						=
            {
                .get                        = config_calibration_current_get,
                .set                        = config_calibration_current_set,
            },
			
            .enter                          = config_calibration_enter,
            .exit                           = config_calibration_exit,
        },
    },
};



/**
  * @brief  
  */
static void metering_init(void)
{
    struct __calibrate_data *calibrates;
	
    if((system_status() == SYSTEM_RUN) || (system_status() == SYSTEM_WAKEUP))
    {
		//读取配置信息
		if(file.parameter.read("calibration", \
								STRUCT_SIZE(struct __calibrates, data), \
								sizeof(mp), \
								(void *)&mp) != sizeof(mp))
		{
			TRACE(TRACE_ERR, "Calibrate information read faild.");
		}
		
		config_check();
		
		//读出当前费率的计量数据
		if(file.parameter.read("measurements", \
								STRUCT_OFFSET(struct __metering_data, energy[mp.rate]), \
								sizeof(mc), \
								(void *)mc) != sizeof(mc))
		{
			TRACE(TRACE_ERR, "Task metering metering_init.");
		}
		
		//清空刷新标记
		heap.set(flush, 0, sizeof(flush));
		
		step = 0;
		quad = 0;
		group = 0;
		period = 0;
	}
	
	//只有正常上电状态下才运行，其它状态下不运行
    if(system_status() == SYSTEM_RUN)
    {
        DEV_M.control.init(DEVICE_NORMAL);
        
        calibrates = heap.dalloc(sizeof(struct __calibrate_data));
        
        if(calibrates)
        {
			//加载校准信息
            if(file.parameter.read("calibration", 0, sizeof(struct __calibrate_data), (void *)calibrates) != sizeof(struct __calibrate_data))
            {
                TRACE(TRACE_ERR, "Calibrate information read faild.");
            }
            else
            {
				//输入校准信息到计量芯片
                if(DEV_M.calibrate.load(sizeof(struct __calibrate_data), (void *)calibrates) != true)
                {
                    TRACE(TRACE_ERR, "Calibrate faild.");
                }
            }
            
            heap.free(calibrates);
        }
	}
	else
	{
		//计量芯片的低功耗初始化类似于挂起，对正常电表来说并没有意义
		//这样操作是为了使模拟表正常运行 
		DEV_M.control.init(DEVICE_LOWPOWER); 
	}
    
    status = TASK_INIT;
	TRACE(TRACE_INFO, "Task metering initialized.");
}

/**
  * @brief  
  */
static void metering_loop(void)
{
	int32_t val;
	int32_t active;
	int32_t reactive;
	
	//只有正常上电状态下才运行，其它状态下不运行
    if(system_status() == SYSTEM_RUN)
    {
		DEV_M.runner(KERNEL_PERIOD);
		
		period += KERNEL_PERIOD;
		
		//校验当前内存中的数据
		for(uint8_t n=0; n<4; n++)
		{
			for(uint8_t g=0; g<3; g++)
			{
				if(mc[n].group[g].check != crc32(&mc[n].group[g], (sizeof(mc[n].group[g]) - sizeof(mc[n].group[g].check)), 0))
				{
					config_check();
					
					if(file.parameter.read("measurements", \
											STRUCT_OFFSET(struct __metering_data, energy[mp.rate][n].group[g]), \
											sizeof(mc[n].group[g]), \
											(void *)&(mc[n].group[g])) != sizeof(mc[n].group[g]))
					{
						TRACE(TRACE_ERR, "Task metering metering_init.");
					}
				}
			}
		}
		
		step += 1;
		
		//从计量芯片中读计量数据
		switch(step)
		{
			case 1://有功总
			{
				val = DEV_M.read(R_EPT);
				if(val && mp.energy_switch)
				{
					active = DEV_M.read(R_PT);
					reactive = DEV_M.read(R_QT);
					
					if(active >= 0)
					{
						if(reactive >= 0)
						{
							//第一相限
							mc[0].group[0].value[0] += val;
							flush[0][0] += 1;
						}
						else
						{
							//第四相限
							mc[3].group[0].value[0] += val;
							flush[3][0] += 1;
						}
					}
					else
					{
						if(reactive >= 0)
						{
							//第二相限
							mc[1].group[0].value[0] += val;
							flush[1][0] += 1;
						}
						else
						{
							//第三相限
							mc[2].group[0].value[0] += val;
							flush[2][0] += 1;
						}
					}
				}
				break;
			}
			case 2://有功A
			{
				val = DEV_M.read(R_EPA);
				if(val && mp.energy_switch)
				{
					active = DEV_M.read(R_PA);
					reactive = DEV_M.read(R_QA);
					if(active >= 0)
					{
						if(reactive >= 0)
						{
							//第一相限
							mc[0].group[0].value[1] += val;
							flush[0][0] += 1;
						}
						else
						{
							//第四相限
							mc[3].group[0].value[1] += val;
							flush[3][0] += 1;
						}
					}
					else
					{
						if(reactive >= 0)
						{
							//第二相限
							mc[1].group[0].value[1] += val;
							flush[1][0] += 1;
						}
						else
						{
							//第三相限
							mc[2].group[0].value[1] += val;
							flush[2][0] += 1;
						}
					}
				}
				break;
			}
			case 3://有功B
			{
				val = DEV_M.read(R_EPB);
				if(val && mp.energy_switch)
				{
					active = DEV_M.read(R_PB);
					reactive = DEV_M.read(R_QB);
					if(active >= 0)
					{
						if(reactive >= 0)
						{
							//第一相限
							mc[0].group[0].value[2] += val;
							flush[0][0] += 1;
						}
						else
						{
							//第四相限
							mc[3].group[0].value[2] += val;
							flush[3][0] += 1;
						}
					}
					else
					{
						if(reactive >= 0)
						{
							//第二相限
							mc[1].group[0].value[2] += val;
							flush[1][0] += 1;
						}
						else
						{
							//第三相限
							mc[2].group[0].value[2] += val;
							flush[2][0] += 1;
						}
					}
				}
				break;
			}
			case 4://有功C
			{
				val = DEV_M.read(R_EPC);
				if(val && mp.energy_switch)
				{
					active = DEV_M.read(R_PC);
					reactive = DEV_M.read(R_QC);
					if(active >= 0)
					{
						if(reactive >= 0)
						{
							//第一相限
							mc[0].group[0].value[3] += val;
							flush[0][0] += 1;
						}
						else
						{
							//第四相限
							mc[3].group[0].value[3] += val;
							flush[3][0] += 1;
						}
					}
					else
					{
						if(reactive >= 0)
						{
							//第二相限
							mc[1].group[0].value[3] += val;
							flush[1][0] += 1;
						}
						else
						{
							//第三相限
							mc[2].group[0].value[3] += val;
							flush[2][0] += 1;
						}
					}
				}
				break;
			}
			case 5://无功总
			{
				val = DEV_M.read(R_EQT);
				if(val && mp.energy_switch)
				{
					active = DEV_M.read(R_PT);
					reactive = DEV_M.read(R_QT);
					
					if(active >= 0)
					{
						if(reactive >= 0)
						{
							//第一相限
							mc[0].group[1].value[0] += val;
							flush[0][1] += 1;
						}
						else
						{
							//第四相限
							mc[3].group[1].value[0] += val;
							flush[3][1] += 1;
						}
					}
					else
					{
						if(reactive >= 0)
						{
							//第二相限
							mc[1].group[1].value[0] += val;
							flush[1][1] += 1;
						}
						else
						{
							//第三相限
							mc[2].group[1].value[0] += val;
							flush[2][1] += 1;
						}
					}
				}
				break;
			}
			case 6://无功A
			{
				val = DEV_M.read(R_EQA);
				if(val && mp.energy_switch)
				{
					active = DEV_M.read(R_PA);
					reactive = DEV_M.read(R_QA);
					
					if(active >= 0)
					{
						if(reactive >= 0)
						{
							//第一相限
							mc[0].group[1].value[1] += val;
							flush[0][1] += 1;
						}
						else
						{
							//第四相限
							mc[3].group[1].value[1] += val;
							flush[3][1] += 1;
						}
					}
					else
					{
						if(reactive >= 0)
						{
							//第二相限
							mc[1].group[1].value[1] += val;
							flush[1][1] += 1;
						}
						else
						{
							//第三相限
							mc[2].group[1].value[1] += val;
							flush[2][1] += 1;
						}
					}
				}
				break;
			}
			case 7://无功B
			{
				val = DEV_M.read(R_EQB);
				if(val && mp.energy_switch)
				{
					active = DEV_M.read(R_PB);
					reactive = DEV_M.read(R_QB);
					
					if(active >= 0)
					{
						if(reactive >= 0)
						{
							//第一相限
							mc[0].group[1].value[2] += val;
							flush[0][1] += 1;
						}
						else
						{
							//第四相限
							mc[3].group[1].value[2] += val;
							flush[3][1] += 1;
						}
					}
					else
					{
						if(reactive >= 0)
						{
							//第二相限
							mc[1].group[1].value[2] += val;
							flush[1][1] += 1;
						}
						else
						{
							//第三相限
							mc[2].group[1].value[2] += val;
							flush[2][1] += 1;
						}
					}
				}
				break;
			}
			case 8://无功C
			{
				val = DEV_M.read(R_EQC);
				if(val && mp.energy_switch)
				{
					active = DEV_M.read(R_PC);
					reactive = DEV_M.read(R_QC);
					
					if(active >= 0)
					{
						if(reactive >= 0)
						{
							//第一相限
							mc[0].group[1].value[3] += val;
							flush[0][1] += 1;
						}
						else
						{
							//第四相限
							mc[3].group[1].value[3] += val;
							flush[3][1] += 1;
						}
					}
					else
					{
						if(reactive >= 0)
						{
							//第二相限
							mc[1].group[1].value[3] += val;
							flush[1][1] += 1;
						}
						else
						{
							//第三相限
							mc[2].group[1].value[3] += val;
							flush[2][1] += 1;
						}
					}
				}
				break;
			}
			case 9://视在总
			{
				val = DEV_M.read(R_EST);
				if(val && mp.energy_switch)
				{
					active = DEV_M.read(R_PT);
					if(active >= 0)
					{
						//正向
						mc[0].group[2].value[0] += val;
						flush[0][2] += 1;
					}
					else
					{
						//反向
						mc[2].group[2].value[0] += val;
						flush[2][2] += 1;
					}
				}
				break;
			}
			case 10://视在A
			{
				val = DEV_M.read(R_ESA);
				if(val && mp.energy_switch)
				{
					active = DEV_M.read(R_PA);
					if(active >= 0)
					{
						//正向
						mc[0].group[2].value[1] += val;
						flush[0][2] += 1;
					}
					else
					{
						//反向
						mc[2].group[2].value[1] += val;
						flush[2][2] += 1;
					}
				}
				break;
			}
			case 11://视在B
			{
				val = DEV_M.read(R_ESB);
				if(val && mp.energy_switch)
				{
					active = DEV_M.read(R_PB);
					if(active >= 0)
					{
						//正向
						mc[0].group[2].value[2] += val;
						flush[0][2] += 1;
					}
					else
					{
						//反向
						mc[2].group[2].value[2] += val;
						flush[2][2] += 1;
					}
				}
				break;
			}
			case 12://视在C
			{
				val = DEV_M.read(R_ESC);
				if(val && mp.energy_switch)
				{
					active = DEV_M.read(R_PC);
					if(active >= 0)
					{
						//正向
						mc[0].group[2].value[3] += val;
						flush[0][2] += 1;
					}
					else
					{
						//反向
						mc[2].group[2].value[3] += val;
						flush[2][2] += 1;
					}
				}
				break;
			}
			default:
			{
				step = 0;
			}
		}
		
		//检查数据是否有变更，更新校验
		for(uint8_t n=0; n<4; n++)
		{
			for(uint8_t g=0; g<3; g++)
			{
				if(!flush[n][g])
				{
					continue;
				}
				
				mc[n].group[g].check = crc32(&mc[n].group[g], (sizeof(mc[n].group[g]) - sizeof(mc[n].group[g].check)), 0);
			}
		}
		
		//每12*5秒写完一次循环，磨损平衡由文件系统提供支持
		if(period >= 5*1000)
		{
			period = 0;
			
			config_check();
			
			quad = quad % 4;
			group = group % 3;
			
			if(flush[quad][group] > 5)
			{
				if(file.parameter.write("measurements", \
										STRUCT_OFFSET(struct __metering_data, energy[mp.rate][quad].group[group]), \
										sizeof(mc[quad].group[group]), \
										(void *)&(mc[quad].group[group])) != sizeof(mc[quad].group[group]))
				{
					TRACE(TRACE_ERR, "Task metering metering_loop.");
				}
				
				flush[quad][group] = 0;
			}
			
			quad += 1;
			group += 1;
		}
    	
        status = TASK_RUN;
	}
}

/**
  * @brief  
  */
static void metering_exit(void)
{
    DEV_M.control.suspend();
	
	//判断是否有还没写入的电能
	config_check();
	
	for(uint8_t n=0; n<4; n++)
	{
		for(uint8_t g=0; g<3; g++)
		{
			if(!flush[n][g])
			{
				continue;
			}
			
			if(file.parameter.write("measurements", \
									STRUCT_OFFSET(struct __metering_data, energy[mp.rate][n].group[g]), \
									sizeof(mc[n].group[g]), \
									(void *)&(mc[n].group[g])) != sizeof(mc[n].group[g]))
			{
				TRACE(TRACE_ERR, "Task metering metering_loop.");
			}
			
			flush[n][g] = 0;
		}
	}
    
    status = TASK_SUSPEND;
    
    TRACE(TRACE_INFO, "Task metering exited.");
}

/**
  * @brief  
  */
static void metering_reset(void)
{
    DEV_M.control.suspend(); 
    
    status = TASK_NOTINIT;
    
#if defined ( MAKE_RUN_FOR_DEBUG )
	heap.set(&mp, 0, sizeof(mp));
	mp.voltage = 220000;
	mp.current = 1500;
	mp.active_div = 6400;
	mp.reactive_div = 6400;
	mp.current_num = 1;
	mp.current_denum = 1;
	mp.voltage_num = 1;
	mp.voltage_denum = 1;
	mp.demand_period = 5;
	mp.demand_multiple = 3;
	mp.energy_switch = 0xff;
	mp.rate = 0;
	mp.check = crc32(&mp, (sizeof(mp) - sizeof(mp.check)), 0);
	
	file.parameter.write("calibration", STRUCT_SIZE(struct __calibrates, data), sizeof(mp), (void *)&mp);
	
	
	
	struct __calibrates *calibrates = heap.dzalloc(sizeof(struct __calibrates));
	
	if(!calibrates)
	{
		return;
	}
	
	calibrates->param.step = 1;
	calibrates->param.voltage = mp.voltage;
	calibrates->param.current = mp.current;
	calibrates->param.ppulse = mp.active_div;
	calibrates->param.qpulse = mp.reactive_div;
	DEV_M.calibrate.enter(sizeof(struct __calibrates), (void *)calibrates);
	file.parameter.write("calibration", 0, sizeof(calibrates->data), (void *)&(calibrates->data));
	
	DEV_M.calibrate.exit();
	
	heap.free(calibrates);
	
	config_energy_clear();
	
	config_demand_clear_current();
	
	config_demand_clear_max();
#endif

	TRACE(TRACE_INFO, "Task metering reset.");
}

/**
  * @brief  
  */
static enum __task_status metering_status(void)
{
    return(status);
}


/**
  * @brief  
  */
const struct __task_sched task_metering = 
{
    .name               = NAME_METERING,
    .init               = metering_init,
    .loop               = metering_loop,
    .exit               = metering_exit,
    .reset              = metering_reset,
    .status             = metering_status,
    .api                = (void *)&metering,
};
