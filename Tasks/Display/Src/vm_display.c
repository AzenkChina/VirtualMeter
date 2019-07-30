/**
 * @brief		
 * @details		
 * @date		2019-02-16
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "types_display.h"
#include "vm_display.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MODULE_NAME		"display"

/* Private variables ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static int display_change(lua_State *L)
{
	uint8_t channel = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
    
	channel = (uint8_t)luaL_checknumber(L, 1);
	channel = api_display->change(channel);
	lua_pushnumber(L, (lua_Number)channel);
	return 1;
}

static int display_channel(lua_State *L)
{
	uint8_t channel = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
	channel = api_display->channel();
	lua_pushnumber(L, (lua_Number)channel);
	return 1;
}

static int display_list_show_next(lua_State *L)
{
	uint8_t index = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
	index = api_display->list.show.next();
	lua_pushnumber(L, (lua_Number)index);
	return 1;
}

static int display_list_show_last(lua_State *L)
{
	uint8_t index = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
	index = api_display->list.show.last();
	lua_pushnumber(L, (lua_Number)index);
	return 1;
}

static int display_list_show_index(lua_State *L)
{
	uint8_t index = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
    index = (uint8_t)luaL_checknumber(L, 1);
	index = api_display->list.show.index(index);
	lua_pushnumber(L, (lua_Number)index);
	return 1;
}

static int display_list_current(lua_State *L)
{
	uint8_t index = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
	index = api_display->list.current();
	lua_pushnumber(L, (lua_Number)index);
	return 1;
}

static int display_config_start_get(lua_State *L)
{
	uint8_t second = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
	second = api_display->config.time.start.get();
	lua_pushnumber(L, (lua_Number)second);
	return 1;
}

static int display_config_start_set(lua_State *L)
{
	uint8_t second = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
    second = (uint8_t)luaL_checknumber(L, 1);
	second = api_display->config.time.start.set(second);
	lua_pushnumber(L, (lua_Number)second);
	return 1;
}

static int display_config_scroll_get(lua_State *L)
{
	uint8_t second = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
	second = api_display->config.time.scroll.get();
	lua_pushnumber(L, (lua_Number)second);
	return 1;
}

static int display_config_scroll_set(lua_State *L)
{
	uint8_t second = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
    second = (uint8_t)luaL_checknumber(L, 1);
	second = api_display->config.time.scroll.set(second);
	lua_pushnumber(L, (lua_Number)second);
	return 1;
}

static int display_config_backlight_get(lua_State *L)
{
	uint8_t second = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
	second = api_display->config.time.backlight.get();
	lua_pushnumber(L, (lua_Number)second);
	return 1;
}

static int display_config_backlight_set(lua_State *L)
{
	uint8_t second = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
    second = (uint8_t)luaL_checknumber(L, 1);
	second = api_display->config.time.backlight.set(second);
	lua_pushnumber(L, (lua_Number)second);
	return 1;
}

static int display_config_power_dot_get(lua_State *L)
{
	uint8_t point = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
	point = api_display->config.dot.power.get();
	lua_pushnumber(L, (lua_Number)point);
	return 1;
}

static int display_config_power_dot_set(lua_State *L)
{
	uint8_t point = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
    point = (uint8_t)luaL_checknumber(L, 1);
	point = api_display->config.dot.power.set(point);
	lua_pushnumber(L, (lua_Number)point);
	return 1;
}

static int display_config_voltage_dot_get(lua_State *L)
{
	uint8_t point = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
	point = api_display->config.dot.voltage.get();
	lua_pushnumber(L, (lua_Number)point);
	return 1;
}

static int display_config_voltage_dot_set(lua_State *L)
{
	uint8_t point = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
    point = (uint8_t)luaL_checknumber(L, 1);
	point = api_display->config.dot.voltage.set(point);
	lua_pushnumber(L, (lua_Number)point);
	return 1;
}

static int display_config_current_dot_get(lua_State *L)
{
	uint8_t point = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
	point = api_display->config.dot.current.get();
	lua_pushnumber(L, (lua_Number)point);
	return 1;
}

static int display_config_current_dot_set(lua_State *L)
{
	uint8_t point = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
    point = (uint8_t)luaL_checknumber(L, 1);
	point = api_display->config.dot.current.set(point);
	lua_pushnumber(L, (lua_Number)point);
	return 1;
}

static int display_config_energy_dot_get(lua_State *L)
{
	uint8_t point = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
	point = api_display->config.dot.energy.get();
	lua_pushnumber(L, (lua_Number)point);
	return 1;
}

static int display_config_energy_dot_set(lua_State *L)
{
	uint8_t point = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
    point = (uint8_t)luaL_checknumber(L, 1);
	point = api_display->config.dot.energy.set(point);
	lua_pushnumber(L, (lua_Number)point);
	return 1;
}

static int display_config_demand_dot_get(lua_State *L)
{
	uint8_t point = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
	point = api_display->config.dot.demand.get();
	lua_pushnumber(L, (lua_Number)point);
	return 1;
}

static int display_config_demand_dot_set(lua_State *L)
{
	uint8_t point = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
    point = (uint8_t)luaL_checknumber(L, 1);
	point = api_display->config.dot.demand.set(point);
	lua_pushnumber(L, (lua_Number)point);
	return 1;
}

static int display_config_angle_dot_get(lua_State *L)
{
	uint8_t point = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
	point = api_display->config.dot.angle.get();
	lua_pushnumber(L, (lua_Number)point);
	return 1;
}

static int display_config_angle_dot_set(lua_State *L)
{
	uint8_t point = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
    point = (uint8_t)luaL_checknumber(L, 1);
	point = api_display->config.dot.angle.set(point);
	lua_pushnumber(L, (lua_Number)point);
	return 1;
}

static int display_config_freq_dot_get(lua_State *L)
{
	uint8_t point = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
	point = api_display->config.dot.freq.get();
	lua_pushnumber(L, (lua_Number)point);
	return 1;
}

static int display_config_freq_dot_set(lua_State *L)
{
	uint8_t point = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
    point = (uint8_t)luaL_checknumber(L, 1);
	point = api_display->config.dot.freq.set(point);
	lua_pushnumber(L, (lua_Number)point);
	return 1;
}

static int display_config_pf_dot_get(lua_State *L)
{
	uint8_t point = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
	point = api_display->config.dot.pf.get();
	lua_pushnumber(L, (lua_Number)point);
	return 1;
}

static int display_config_pf_dot_set(lua_State *L)
{
	uint8_t point = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
    point = (uint8_t)luaL_checknumber(L, 1);
	point = api_display->config.dot.pf.set(point);
	lua_pushnumber(L, (lua_Number)point);
	return 1;
}

static int display_config_list_write(lua_State *L)
{
	lua_pushnil(L);
	return 1;
}

static int display_config_list_read(lua_State *L)
{
	lua_pushnil(L);
	return 1;
}

static int display_config_list_amount(lua_State *L)
{
	uint16_t amount = 0;
	uint8_t channel = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
    	lua_pushnil(L);
        return 1;
    }
    channel = (uint8_t)luaL_checknumber(L, 1);
	amount = api_display->config.list.amount(channel);
	lua_pushnumber(L, (lua_Number)amount);
	return 1;
}

static int display_config_list_clean(lua_State *L)
{
	uint8_t channel = 0;
	struct __display *api_display = api("task_display");
	
    if(!api_display)
    {
        return lua_error(L);
    }
    channel = (uint8_t)luaL_checknumber(L, 1);
	api_display->config.list.clean(channel);
	return 0;
}



static const luaL_Tree display_tree[] =
{
	{NULL,			"change",		display_change},
	{NULL,			"channel",		display_channel},
	{NULL,			"list",			NULL},
	{"list",		"show",			NULL},
	{"show",		"next",			display_list_show_next},
	{"show",		"last",			display_list_show_last},
	{"show",		"index",		display_list_show_index},
	{"list",		"current",		display_list_current},
	{NULL,			"config",		NULL},
	{"config",		"time",			NULL},
	{"time",		"start",		NULL},
	{"start",		"get",			display_config_start_get},
	{"start",		"set",			display_config_start_set},
	{"time",		"scroll",		NULL},
	{"scroll",		"get",			display_config_scroll_get},
	{"scroll",		"set",			display_config_scroll_set},
	{"time",		"backlight",	NULL},
	{"backlight",	"get",			display_config_backlight_get},
	{"backlight",	"set",			display_config_backlight_set},
	{"config",		"dot",			NULL},
	{"dot",			"power",		NULL},
	{"power",		"get",			display_config_power_dot_get},
	{"power",		"set",			display_config_power_dot_set},
	{"dot",			"voltage",		NULL},
	{"voltage",		"get",			display_config_voltage_dot_get},
	{"voltage",		"set",			display_config_voltage_dot_set},
	{"dot",			"current",		NULL},
	{"current",		"get",			display_config_current_dot_get},
	{"current",		"set",			display_config_current_dot_set},
	{"dot",			"energy",		NULL},
	{"energy",		"get",			display_config_energy_dot_get},
	{"energy",		"set",			display_config_energy_dot_set},
	{"dot",			"demand",		NULL},
	{"demand",		"get",			display_config_demand_dot_get},
	{"demand",		"set",			display_config_demand_dot_set},
	{"dot",			"angle",		NULL},
	{"angle",		"get",			display_config_angle_dot_get},
	{"angle",		"set",			display_config_angle_dot_set},
	{"dot",			"freq",			NULL},
	{"freq",		"get",			display_config_freq_dot_get},
	{"freq",		"set",			display_config_freq_dot_set},
	{"dot",			"pf",			NULL},
	{"pf",			"get",			display_config_pf_dot_get},
	{"pf",			"set",			display_config_pf_dot_set},
	{"config",		"list",			NULL},
	{"list",		"write",		display_config_list_write},
	{"list",		"read",			display_config_list_read},
	{"list",		"amount",		display_config_list_amount},
	{"list",		"clean",		display_config_list_clean},
	{NULL,			NULL,			NULL},
};

static int vm_display_open(lua_State *L)
{
	luaL_packing(L, MODULE_NAME, display_tree);
	return 1;
}




const luaL_Reg vm_display = 
{
	.name		= MODULE_NAME,
	.func		= vm_display_open,
};

