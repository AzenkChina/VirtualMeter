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
#define __USE_64BIT_ENERGY__        //选择是否使用64位数据类型来存储电能
//#define __USE_PRIMARY_ENERGY__      //选择是否使用一次侧数据而不用二次侧数据
#define DEV_M			meter

/* Private macro -------------------------------------------------------------*/
#if defined ( __STORE_PRIMARY_ENERGY__ ) && !defined ( __USE_64BIT_ENERGY__ )
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
                DEV_M.data.read((uint32_t)R_UARMS, 1, &read);
				*val = read;
			}
			else if(id.phase == M_PHASE_B)
			{
                DEV_M.data.read((uint32_t)R_UBRMS, 1, &read);
				*val = read;
			}
			else if(id.phase == M_PHASE_C)
			{
                DEV_M.data.read((uint32_t)R_UCRMS, 1, &read);
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
                DEV_M.data.read((uint32_t)R_IARMS, 1, &read);
				*val = read;
			}
			else if(id.phase == M_PHASE_B)
			{
                DEV_M.data.read((uint32_t)R_IBRMS, 1, &read);
				*val = read;
			}
			else if(id.phase == M_PHASE_C)
			{
                DEV_M.data.read((uint32_t)R_ICRMS, 1, &read);
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
	
}

static int64_t metering_primary(struct __meta_identifier id, int64_t val)
{
	
}





static uint8_t config_rate_read(void)
{
	
}

static uint8_t config_rate_change(uint8_t rate)
{
	
}

static uint8_t config_rate_max(void)
{
	
}


static uint32_t config_current_ratio_get(void)
{
	
}

static uint32_t config_current_ratio_set(uint32_t val)
{
	
}

static uint32_t config_current_ratio_get_num(void)
{
	
}

static uint32_t config_current_ratio_set_num(uint32_t val)
{
	
}

static uint32_t config_current_ratio_get_denum(void)
{
	
}

static uint32_t config_current_ratio_set_denum(uint32_t val)
{
	
}


static uint32_t config_voltage_ratio_get(void)
{
	
}

static uint32_t config_voltage_ratio_set(uint32_t val)
{
	
}

static uint32_t config_voltage_ratio_get_num(void)
{
	
}

static uint32_t config_voltage_ratio_set_num(uint32_t val)
{
	
}

static uint32_t config_voltage_ratio_get_denum(void)
{
	
}

static uint32_t config_voltage_ratio_set_denum(uint32_t val)
{
	
}

static enum __metering_status config_energy_start(void)
{
	
}

static enum __metering_status config_energy_stop(void)
{
	
}

static enum __metering_status config_energy_status(void)
{
	
}

static void config_energy_clear(void)
{
	
}



static uint8_t config_demand_get_period(void)
{
	
}

static uint8_t config_demand_set_period(uint8_t minute)
{
	
}

static uint8_t config_demand_get_multiple(void)
{
	
}

static uint8_t config_demand_set_multiple(uint8_t val)
{
	
}

static void config_demand_clear_current(void)
{
	
}

static void config_demand_clear_max(void)
{
	
}


static uint32_t config_active_div_get(void)
{
	
}

static uint32_t config_active_div_set(uint32_t val)
{
	
}


static uint32_t config_reactive_div_get(void)
{
	
}

static uint32_t config_reactive_div_set(uint32_t val)
{
	
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
	//只有正常上电状态下才运行，其它状态下不运行
    if(system_status() == SYSTEM_RUN)
    {
        DEV_M.control.init(DEVICE_NORMAL); 
        
        status = TASK_INIT;
        
        TRACE(TRACE_INFO, "Task metering initialized.");
	}
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
