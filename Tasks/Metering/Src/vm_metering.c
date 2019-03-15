/**
 * @brief		
 * @details		
 * @date		2019-02-16
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "types_comm.h"
#include "vm_metering.h"
#include "types_metering.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MODULE_NAME		"meter"

/* Private variables ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static int metering_instant(lua_State *L)
{
    struct __metering_identifier id;
    uint32_t vid = 0;
    struct __metering *api_metering;
    int64_t val = 0;
    
    api_metering = (struct __metering *)api.query("task_metering");
    if(!api_metering)
    {
    	lua_pushnil(L);
        return 1;
    }
    
    vid = (uint32_t)luaL_checknumber(L, 1);
    M_UINT2ID(vid, id);
    
    if(api_metering->instant(id, &val) == M_NULL)
    {
        api.release();
    	lua_pushnil(L);
        return 1;
    }
    
    api.release();
	lua_pushnumber(L, ((lua_Number)val)/1000);
	return 1;
}

static int metering_recent(lua_State *L)
{
	return 0;
}

static int metering_primary(lua_State *L)
{
	return 0;
}

static int config_rate_read(lua_State *L)
{
	return 0;
}

static int config_rate_change(lua_State *L)
{
	return 0;
}

static int config_rate_max(lua_State *L)
{
	return 0;
}

static int config_current_ratio_get(lua_State *L)
{
	return 0;
}

static int config_current_ratio_set(lua_State *L)
{
	return 0;
}

static int config_current_ratio_get_num(lua_State *L)
{
	return 0;
}

static int config_current_ratio_set_num(lua_State *L)
{
	return 0;
}

static int config_current_ratio_get_denum(lua_State *L)
{
	return 0;
}

static int config_current_ratio_set_denum(lua_State *L)
{
	return 0;
}

static int config_voltage_ratio_get(lua_State *L)
{
	return 0;
}

static int config_voltage_ratio_set(lua_State *L)
{
	return 0;
}

static int config_voltage_ratio_get_num(lua_State *L)
{
	return 0;
}

static int config_voltage_ratio_set_num(lua_State *L)
{
	return 0;
}

static int config_voltage_ratio_get_denum(lua_State *L)
{
	return 0;
}

static int config_voltage_ratio_set_denum(lua_State *L)
{
	return 0;
}

static int config_energy_start(lua_State *L)
{
	return 0;
}

static int config_energy_stop(lua_State *L)
{
	return 0;
}

static int config_energy_status(lua_State *L)
{
	return 0;
}

static int config_energy_clear(lua_State *L)
{
	return 0;
}

static int config_demand_get_period(lua_State *L)
{
	return 0;
}

static int config_demand_set_period(lua_State *L)
{
	return 0;
}

static int config_demand_get_multiple(lua_State *L)
{
	return 0;
}

static int config_demand_set_multiple(lua_State *L)
{
	return 0;
}

static int config_demand_clear_current(lua_State *L)
{
	return 0;
}

static int config_demand_clear_max(lua_State *L)
{
	return 0;
}

static int config_active_div_get(lua_State *L)
{
	return 0;
}

static int config_active_div_set(lua_State *L)
{
	return 0;
}

static int config_reactive_div_get(lua_State *L)
{
	return 0;
}

static int config_reactive_div_set(lua_State *L)
{
	return 0;
}

static int config_calibration_enter(lua_State *L)
{
    return 0;
}

static int config_calibration_exit(lua_State *L)
{
    return 0;
}

static const luaL_Tree metering_tree[] =
{
	{NULL,			"instant",	metering_instant},
	{NULL,			"recent",	metering_recent},
	{NULL,			"primary",			metering_primary},
	{NULL,			"config",			NULL},
	{"config",		"rate",				NULL},
	{"rate",		"read",				config_rate_read},
	{"rate",		"change",			config_rate_change},
	{"rate",		"max",				config_rate_max},
	{"config",		"ratio",			NULL},
	{"ratio",		"current",			NULL},
	{"current",		"get",				config_current_ratio_get},
	{"current",		"set",				config_current_ratio_set},
	{"current",		"get_num",			config_current_ratio_get_num},
	{"current",		"set_num",			config_current_ratio_set_num},
	{"current",		"get_denum",		config_current_ratio_get_denum},
	{"current",		"get_denum",		config_current_ratio_set_denum},
	{"ratio",		"voltage",			NULL},
	{"voltage",		"get",				config_voltage_ratio_get},
	{"voltage",		"set",				config_voltage_ratio_set},
	{"voltage",		"get_num",			config_voltage_ratio_get_num},
	{"voltage",		"set_num",			config_voltage_ratio_set_num},
	{"voltage",		"get_denum",		config_voltage_ratio_get_denum},
	{"voltage",		"get_denum",		config_voltage_ratio_set_denum},
	{"config",		"demand",			NULL},
	{"demand",		"get_period",		config_demand_get_period},
	{"demand",		"set_period",		config_demand_set_period},
	{"demand",		"get_multiple",		config_demand_get_multiple},
	{"demand",		"set_multiple",		config_demand_set_multiple},
	{"demand",		"clear_current",	config_demand_clear_current},
	{"demand",		"clear_max",		config_demand_clear_max},
	{"config",		"energy",			NULL},
	{"energy",		"start",			config_energy_start},
	{"energy",		"stop",				config_energy_stop},
	{"energy",		"status",			config_energy_status},
	{"energy",		"clear",			config_energy_clear},
	{"config",		"div",				NULL},
	{"div",			"active",			NULL},
	{"active",		"get",				config_active_div_get},
	{"active",		"set",				config_active_div_set},
	{"div",			"reactive",			NULL},
	{"reactive",	"get",				config_reactive_div_get},
	{"reactive",	"set",				config_reactive_div_set},
	{"config",		"calibration",		NULL},
	{"calibration",	"enter",			config_calibration_enter},
	{"calibration",	"exit",				config_calibration_exit},
	{NULL,			NULL,				NULL}
};

static int vm_metering_open(lua_State *L)
{
	luaL_packing(L, MODULE_NAME, metering_tree);
	return 1;
}




const luaL_Reg vm_metering = 
{
	.name		= MODULE_NAME,
	.func		= vm_metering_open,
};

