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
	uint64_t active[MAX_PHASE];
	uint64_t reactive[MAX_PHASE];
	uint64_t apparent[MAX_PHASE];
	uint32_t check;
};

/**
  * @brief  计量数据
  */
struct __metering_data
{
	struct __metering_base energy[MAX_RATE][MAX_QUAD];
	struct __metering_base demand[MAX_RATE][MAX_QUAD];
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
static struct __metering_base mc[MAX_QUAD];

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
    int32_t read;

	config_check();
    
	if(id.item == M_VOLTAGE)
	{
		if(id.rate != 0)
		{
			*val = 0;
		}
		else
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
		}
		
		return(M_VOLTAGE);
	}
	else if(id.item == M_CURRENT)
	{
		if(id.rate != 0)
		{
			*val = 0;
		}
		else
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
                read = DEV_M.read(R_IT);
				*val = read;
			}
			else
			{
				*val = 0;
			}
		}
		
		return(M_CURRENT);
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
	config_check();
	mp.rate = rate % (MAX_RATE + 1);
	mp.check = crc32(&mp, (sizeof(mp) - sizeof(mp.check)), 0);
	
	if(file.parameter.write("calibration", \
							STRUCT_SIZE(struct __calibrates, data) + STRUCT_OFFSET(struct __metering_parameter, rate), \
							sizeof(mp.rate), \
							(void *)&(mp.rate)) != sizeof(mp.rate))
	{
		TRACE(TRACE_ERR, "Task metering config_rate_change.");
	}
	
	//...费率改变之后需要写回当前费率的计量数据
	
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
		if(file.parameter.read("calibration", \
								STRUCT_SIZE(struct __calibrates, data), \
								sizeof(mp), \
								(void *)&mp) != sizeof(mp))
		{
			TRACE(TRACE_ERR, "Calibrate information read faild.");
		}
		
		config_check();
	}
	
	//只有正常上电状态下才运行，其它状态下不运行
    if(system_status() == SYSTEM_RUN)
    {
        DEV_M.control.init(DEVICE_NORMAL);
        
        calibrates = heap.dalloc(sizeof(struct __calibrate_data));
        
        if(calibrates)
        {
            if(file.parameter.read("calibration", 0, sizeof(struct __calibrate_data), (void *)calibrates) != sizeof(struct __calibrate_data))
            {
                TRACE(TRACE_ERR, "Calibrate information read faild.");
            }
            else
            {
                if(DEV_M.calibrate.load(sizeof(struct __calibrate_data), (void *)calibrates) != true)
                {
                    TRACE(TRACE_ERR, "Calibrate faild.");
                }
            }
            
            heap.free(calibrates);
        }
		
		if(file.parameter.read("measurements", \
								STRUCT_OFFSET(struct __metering_data, energy[mp.rate]), \
								sizeof(mc), \
								(void *)&mc) != sizeof(mc))
		{
			TRACE(TRACE_ERR, "Task metering metering_init.");
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
	static uint8_t step = 0;
	static uint8_t flush = 0;
	static uint16_t period = 0;
	int32_t val;
	int32_t active;
	int32_t reactive;
	
	//只有正常上电状态下才运行，其它状态下不运行
    if(system_status() == SYSTEM_RUN)
    {
		DEV_M.runner(KERNEL_PERIOD);
		
		period += KERNEL_PERIOD;
		
		//校验当前内存中的数据
		for(uint8_t n=0; n<MAX_QUAD; n++)
		{
			if(mc[n].check != crc32(&mc[n], (sizeof(mc[n]) - sizeof(mc[n].check)), 0))
			{
				config_check();
				
				if(file.parameter.read("measurements", \
										STRUCT_OFFSET(struct __metering_data, energy[mp.rate]), \
										sizeof(mc), \
										(void *)&mc) != sizeof(mc))
				{
					TRACE(TRACE_ERR, "Task metering metering_init.");
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
				if(val)
				{
					flush = 0xff;
					active = DEV_M.read(R_PT);
					reactive = DEV_M.read(R_QT);
					
					if(active >= 0)
					{
						if(reactive >= 0)
						{
							//第一相限
							mc[0].active[0] += val;
						}
						else
						{
							//第四相限
							mc[3].active[0] += val;
						}
					}
					else
					{
						if(reactive >= 0)
						{
							//第二相限
							mc[1].active[0] += val;
						}
						else
						{
							//第三相限
							mc[2].active[0] += val;
						}
					}
				}
				break;
			}
			case 2://有功A
			{
				val = DEV_M.read(R_EPA);
				if(val)
				{
					flush = 0xff;
					active = DEV_M.read(R_PA);
					reactive = DEV_M.read(R_QA);
					if(active >= 0)
					{
						if(reactive >= 0)
						{
							//第一相限
							mc[0].active[1] += val;
						}
						else
						{
							//第四相限
							mc[3].active[1] += val;
						}
					}
					else
					{
						if(reactive >= 0)
						{
							//第二相限
							mc[1].active[1] += val;
						}
						else
						{
							//第三相限
							mc[2].active[1] += val;
						}
					}
				}
				break;
			}
			case 3://有功B
			{
				val = DEV_M.read(R_EPB);
				if(val)
				{
					flush = 0xff;
					active = DEV_M.read(R_PB);
					reactive = DEV_M.read(R_QB);
					if(active >= 0)
					{
						if(reactive >= 0)
						{
							//第一相限
							mc[0].active[2] += val;
						}
						else
						{
							//第四相限
							mc[3].active[2] += val;
						}
					}
					else
					{
						if(reactive >= 0)
						{
							//第二相限
							mc[1].active[2] += val;
						}
						else
						{
							//第三相限
							mc[2].active[2] += val;
						}
					}
				}
				break;
			}
			case 4://有功C
			{
				val = DEV_M.read(R_EPC);
				if(val)
				{
					flush = 0xff;
					active = DEV_M.read(R_PC);
					reactive = DEV_M.read(R_QC);
					if(active >= 0)
					{
						if(reactive >= 0)
						{
							//第一相限
							mc[0].active[3] += val;
						}
						else
						{
							//第四相限
							mc[3].active[3] += val;
						}
					}
					else
					{
						if(reactive >= 0)
						{
							//第二相限
							mc[1].active[3] += val;
						}
						else
						{
							//第三相限
							mc[2].active[3] += val;
						}
					}
				}
				break;
			}
			case 5://无功总
			{
				val = DEV_M.read(R_EQT);
				if(val)
				{
					flush = 0xff;
					active = DEV_M.read(R_PT);
					reactive = DEV_M.read(R_QT);
					
					if(active >= 0)
					{
						if(reactive >= 0)
						{
							//第一相限
							mc[0].reactive[0] += val;
						}
						else
						{
							//第四相限
							mc[3].reactive[0] += val;
						}
					}
					else
					{
						if(reactive >= 0)
						{
							//第二相限
							mc[1].reactive[0] += val;
						}
						else
						{
							//第三相限
							mc[2].reactive[0] += val;
						}
					}
				}
				break;
			}
			case 6://无功A
			{
				val = DEV_M.read(R_EQA);
				if(val)
				{
					flush = 0xff;
					active = DEV_M.read(R_PA);
					reactive = DEV_M.read(R_QA);
					
					if(active >= 0)
					{
						if(reactive >= 0)
						{
							//第一相限
							mc[0].reactive[1] += val;
						}
						else
						{
							//第四相限
							mc[3].reactive[1] += val;
						}
					}
					else
					{
						if(reactive >= 0)
						{
							//第二相限
							mc[1].reactive[1] += val;
						}
						else
						{
							//第三相限
							mc[2].reactive[1] += val;
						}
					}
				}
				break;
			}
			case 7://无功B
			{
				val = DEV_M.read(R_EQB);
				if(val)
				{
					flush = 0xff;
					active = DEV_M.read(R_PB);
					reactive = DEV_M.read(R_QB);
					
					if(active >= 0)
					{
						if(reactive >= 0)
						{
							//第一相限
							mc[0].reactive[2] += val;
						}
						else
						{
							//第四相限
							mc[3].reactive[2] += val;
						}
					}
					else
					{
						if(reactive >= 0)
						{
							//第二相限
							mc[1].reactive[2] += val;
						}
						else
						{
							//第三相限
							mc[2].reactive[2] += val;
						}
					}
				}
				break;
			}
			case 8://无功C
			{
				val = DEV_M.read(R_EQC);
				if(val)
				{
					flush = 0xff;
					active = DEV_M.read(R_PC);
					reactive = DEV_M.read(R_QC);
					
					if(active >= 0)
					{
						if(reactive >= 0)
						{
							//第一相限
							mc[0].reactive[3] += val;
						}
						else
						{
							//第四相限
							mc[3].reactive[3] += val;
						}
					}
					else
					{
						if(reactive >= 0)
						{
							//第二相限
							mc[1].reactive[3] += val;
						}
						else
						{
							//第三相限
							mc[2].reactive[3] += val;
						}
					}
				}
				break;
			}
			case 9://视在总
			{
				val = DEV_M.read(R_EST);
				if(val)
				{
					flush = 0xff;
					active = DEV_M.read(R_PT);
					if(active >= 0)
					{
						//正向
						mc[0].apparent[1] += val;
					}
					else
					{
						//反向
						mc[2].apparent[1] += val;
					}
				}
				break;
			}
			case 10://视在A
			{
				val = DEV_M.read(R_ESA);
				if(val)
				{
					flush = 0xff;
					active = DEV_M.read(R_PA);
					if(active >= 0)
					{
						//正向
						mc[0].apparent[1] += val;
					}
					else
					{
						//反向
						mc[2].apparent[1] += val;
					}
				}
				break;
			}
			case 11://视在B
			{
				val = DEV_M.read(R_ESB);
				if(val)
				{
					flush = 0xff;
					active = DEV_M.read(R_PB);
					if(active >= 0)
					{
						//正向
						mc[0].apparent[2] += val;
					}
					else
					{
						//反向
						mc[2].apparent[2] += val;
					}
				}
				break;
			}
			case 12://视在C
			{
				val = DEV_M.read(R_ESC);
				if(val)
				{
					flush = 0xff;
					active = DEV_M.read(R_PC);
					if(active >= 0)
					{
						//正向
						mc[0].apparent[3] += val;
					}
					else
					{
						//反向
						mc[2].apparent[3] += val;
					}
				}
				break;
			}
			default:
			{
				step = 0;
			}
		}
		
		//数据有变更，更新校验
		if(flush)
		{
			for(uint8_t n=0; n<MAX_QUAD; n++)
			{
				mc[n].check = crc32(&mc[n], (sizeof(mc[n]) - sizeof(mc[n].check)), 0);
			}
		}
		
		//最快每5秒写回一次，磨损平衡由文件系统提供支持
		if((period >= 5*1000) && (step == 0))
		{
			period = 0;
			
			if(flush)
			{
				flush = 0;
				
				config_check();
				
				if(file.parameter.write("measurements", \
										STRUCT_OFFSET(struct __metering_data, energy[mp.rate]), \
										sizeof(mc), \
										(void *)&mc) != sizeof(mc))
				{
					TRACE(TRACE_ERR, "Task metering metering_loop.");
				}
			}
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
	
	calibrates->param.step = 2;
	DEV_M.calibrate.enter(sizeof(struct __calibrates), (void *)calibrates);
	file.parameter.write("calibration", 0, sizeof(calibrates->data), (void *)&(calibrates->data));
	
	calibrates->param.step = 3;
	DEV_M.calibrate.enter(sizeof(struct __calibrates), (void *)calibrates);
	file.parameter.write("calibration", 0, sizeof(calibrates->data), (void *)&(calibrates->data));
	
	calibrates->param.step = 4;
	DEV_M.calibrate.enter(sizeof(struct __calibrates), (void *)calibrates);
	file.parameter.write("calibration", 0, sizeof(calibrates->data), (void *)&(calibrates->data));
	
	calibrates->param.step = 5;
	DEV_M.calibrate.enter(sizeof(struct __calibrates), (void *)calibrates);
	file.parameter.write("calibration", 0, sizeof(calibrates->data), (void *)&(calibrates->data));
	
	calibrates->param.step = 6;
	DEV_M.calibrate.enter(sizeof(struct __calibrates), (void *)calibrates);
	file.parameter.write("calibration", 0, sizeof(calibrates->data), (void *)&(calibrates->data));
	
	calibrates->param.step = 7;
	DEV_M.calibrate.enter(sizeof(struct __calibrates), (void *)calibrates);
	file.parameter.write("calibration", 0, sizeof(calibrates->data), (void *)&(calibrates->data));
	
	DEV_M.calibrate.exit();
	
	heap.free(calibrates);
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
