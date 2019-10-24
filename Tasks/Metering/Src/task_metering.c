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

#include "meter.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define DEV_M			meter

/* Private macro -------------------------------------------------------------*/
#if defined ( __USE_PRIMARY_ENERGY__ ) && !defined ( __USE_64BIT_ENERGY__ )
#define __USE_64BIT_ENERGY__        //当选择使用一次侧数据，则必须选择使用64位数据类型来存储电能
#endif

/* Private variables ---------------------------------------------------------*/
static enum __task_status status = TASK_NOTINIT;

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
                read = DEV_M.read(R_UARMS);
				*val = read;
			}
			else if(id.phase == M_PHASE_B)
			{
                read = DEV_M.read(R_UBRMS);
				*val = read;
			}
			else if(id.phase == M_PHASE_C)
			{
                read = DEV_M.read(R_UCRMS);
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
                read = DEV_M.read(R_IARMS);
				*val = read;
			}
			else if(id.phase == M_PHASE_B)
			{
                read = DEV_M.read(R_IBRMS);
				*val = read;
			}
			else if(id.phase == M_PHASE_C)
			{
                read = DEV_M.read(R_ICRMS);
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





static uint8_t config_rate_read(void)
{
	return(0);
}

static uint8_t config_rate_change(uint8_t rate)
{
	return(0);
}

static uint8_t config_rate_max(void)
{
	return(0);
}


static uint32_t config_current_ratio_get(void)
{
	return(0);
}

static uint32_t config_current_ratio_set(uint32_t val)
{
	return(0);
}

static uint32_t config_current_ratio_get_num(void)
{
	return(0);
}

static uint32_t config_current_ratio_set_num(uint32_t val)
{
	return(0);
}

static uint32_t config_current_ratio_get_denum(void)
{
	return(0);
}

static uint32_t config_current_ratio_set_denum(uint32_t val)
{
	return(0);
}


static uint32_t config_voltage_ratio_get(void)
{
	return(0);
}

static uint32_t config_voltage_ratio_set(uint32_t val)
{
	return(0);
}

static uint32_t config_voltage_ratio_get_num(void)
{
	return(0);
}

static uint32_t config_voltage_ratio_set_num(uint32_t val)
{
	return(0);
}

static uint32_t config_voltage_ratio_get_denum(void)
{
	return(0);
}

static uint32_t config_voltage_ratio_set_denum(uint32_t val)
{
	return(0);
}

static enum __metering_status config_energy_start(void)
{
	return(M_ENERGY_START);
}

static enum __metering_status config_energy_stop(void)
{
	return(M_ENERGY_START);
}

static enum __metering_status config_energy_status(void)
{
	return(M_ENERGY_START);
}

static void config_energy_clear(void)
{
	
}



static uint8_t config_demand_get_period(void)
{
	return(0);
}

static uint8_t config_demand_set_period(uint8_t minute)
{
	return(0);
}

static uint8_t config_demand_get_multiple(void)
{
	return(0);
}

static uint8_t config_demand_set_multiple(uint8_t val)
{
	return(0);
}

static void config_demand_clear_current(void)
{
	
}

static void config_demand_clear_max(void)
{
	
}


static uint32_t config_active_div_get(void)
{
	return(0);
}

static uint32_t config_active_div_set(uint32_t val)
{
	return(0);
}


static uint32_t config_reactive_div_get(void)
{
	return(0);
}

static uint32_t config_reactive_div_set(uint32_t val)
{
	return(0);
}

static void config_calibration_enter(void *args)
{
    
}

static void config_calibration_exit(void)
{
    
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
        
        calibrates = heap.dalloc(sizeof(struct __calibrate_data));
        
        if(calibrates)
        {
            if(file.read("calibration", 0, sizeof(struct __calibrate_data), (void *)calibrates) != sizeof(struct __calibrate_data))
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
        
        TRACE(TRACE_INFO, "Task metering initialized.");
	}
	else
	{
		//计量芯片的低功耗初始化类似于挂起，对正常电表来说并没有意义
		//这样操作是为了使模拟表正常运行 
		DEV_M.control.init(DEVICE_LOWPOWER); 
	}
    
    status = TASK_INIT;
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
    
    TRACE(TRACE_INFO, "Task metering reset.");
    
#if defined ( MAKE_RUN_FOR_DEBUG )
    struct __calibrates *calibrates = heap.dalloc(sizeof(struct __calibrates));
    
    if(!calibrates)
    {
        return;
    }
    
    heap.set(calibrates, 0, sizeof(struct __calibrates));
    
    calibrates->param.voltage = 220000;
    calibrates->param.current = 1500;
    calibrates->param.ppulse = 10000;
    calibrates->param.qpulse = 10000;
    
    DEV_M.calibrate.enter(sizeof(struct __calibrates), (void *)calibrates);
    
    while(DEV_M.calibrate.status() != true);
    
    DEV_M.calibrate.exit();
    
    file.write("calibration", 0, sizeof(calibrates->data), (void *)&(calibrates->data));
    
    heap.free(calibrates);
#endif
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
