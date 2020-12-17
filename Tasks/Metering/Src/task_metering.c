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
	uint32_t voltage; //校准电压
	uint32_t current; //校准电流
	
	uint32_t active_div; //有功脉冲常数
	uint32_t reactive_div; //无功脉冲常数
	
	uint32_t current_num; //电流变比分子
	uint32_t current_denum; //电流变比分母
	
	uint32_t voltage_num; //电流变比分子
	uint32_t voltage_denum; //电流变比分母
	
	uint8_t demand_period; //滑差时间
	uint8_t demand_multiple;//滑差时间倍数
	
	uint8_t energy_switch;//电能计量开关
	uint8_t rate;//当前费率
	
	uint32_t check;
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

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static enum __meta_item metering_instant(struct __meta_identifier id, int64_t *val)
{
    int32_t read;
    
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

static enum __meta_item metering_recent(struct __meta_identifier id, int64_t *val)
{
	return(M_NULL);
}

static int64_t metering_primary(struct __meta_identifier id, int64_t val)
{
	return(0);
}





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
			mp.rate = 1;
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
				mp.rate = 1;
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

static uint8_t config_rate_read(void)
{
	config_check();
	return(mp.rate);
}

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
	
	return(mp.rate);
}

static uint8_t config_rate_max(void)
{
	config_check();
	return(MAX_RATE);
}

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

static uint32_t config_current_ratio_get_num(void)
{
	config_check();
	return(mp.current_num);
}

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

static uint32_t config_voltage_ratio_get_num(void)
{
	config_check();
	return(mp.voltage_num);
}

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

static void config_energy_clear(void)
{
	
}


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

static void config_demand_clear_current(void)
{
	
}

static void config_demand_clear_max(void)
{
	
}


static uint32_t config_active_div_get(void)
{
	config_check();
	return(mp.active_div);
}

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

static uint32_t config_calibration_voltage_get(void)
{
	config_check();
	return(mp.voltage);
}

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

static uint32_t config_calibration_current_get(void)
{
	config_check();
	return(mp.current);
}

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

static void config_calibration_exit(void)
{
    DEV_M.calibrate.exit();
}





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
    
	//只有正常上电状态下才运行，其它状态下不运行
    if(system_status() == SYSTEM_RUN)
    {
        DEV_M.control.init(DEVICE_NORMAL);
		
		if(file.parameter.read("calibration", \
								STRUCT_SIZE(struct __calibrates, data), \
								sizeof(mp), \
								(void *)&mp) != sizeof(mp))
		{
			TRACE(TRACE_ERR, "Calibrate information read faild.");
		}
        
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
	//只有正常上电状态下才运行，其它状态下不运行
    if(system_status() == SYSTEM_RUN)
    {
    	DEV_M.runner(KERNEL_PERIOD);
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
	mp.rate = 1;
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
