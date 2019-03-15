/**
 * @brief		
 * @details		
 * @date		2019-02-16
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "types_comm.h"
#include "vm_comm.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MODULE_NAME		"comm"

/* Private variables ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static int serial_porttype(lua_State *L)
{
	uint8_t channel = 0;
	enum __port_type type;
	struct __comm *api_comm = api.query("task_comm");
	
    if(!api_comm)
    {
    	lua_pushnil(L);
        return 1;
    }
    
	channel = (uint8_t)luaL_checknumber(L, 1);
	type = api_comm->attrib.type(channel);
	api.release();
	lua_pushnumber(L, (lua_Number)type);
	return 1; 
}

static int serial_portstatus(lua_State *L)
{
	uint8_t channel = 0;
	enum __port_type status;
	struct __comm *api_comm = api.query("task_comm");
	
    if(!api_comm)
    {
    	lua_pushnil(L);
        return 1;
    }
    
	channel = (uint8_t)luaL_checknumber(L, 1);
	status = api_comm->attrib.status(channel);
	api.release();
	lua_pushnumber(L, (lua_Number)status);
	return 1; 
}

static int serial_timeout_set(lua_State *L)
{
	uint8_t channel = 0;
	uint16_t msecond;
	struct __comm *api_comm = api.query("task_comm");
	
    if(!api_comm)
    {
    	lua_pushnil(L);
        return 1;
    }
    
	channel = (uint8_t)luaL_checknumber(L, 1);
	msecond = (uint16_t)luaL_checknumber(L, 2);
	msecond = api_comm->timeout.set(channel, msecond);
	api.release();
	lua_pushnumber(L, (lua_Number)msecond);
	return 1;
}

static int serial_timeout_get(lua_State *L)
{
	uint8_t channel = 0;
	uint16_t msecond;
	struct __comm *api_comm = api.query("task_comm");
	
    if(!api_comm)
    {
    	lua_pushnil(L);
        return 1;
    }
    
	channel = (uint8_t)luaL_checknumber(L, 1);
	msecond = api_comm->timeout.get(channel);
	api.release();
	lua_pushnumber(L, (lua_Number)msecond);
	return 1;
}

static int serial_baudrate_set(lua_State *L)
{
	uint8_t channel = 0;
	uint16_t baud;
	struct __comm *api_comm = api.query("task_comm");
	
    if(!api_comm)
    {
    	lua_pushnil(L);
        return 1;
    }
    
	channel = (uint8_t)luaL_checknumber(L, 1);
	baud = (uint16_t)luaL_checknumber(L, 2);
	baud = api_comm->baudrate.set(channel, (enum __baud)(baud/100));
	api.release();
	lua_pushnumber(L, (lua_Number)(baud*100));
	return 1;
}

static int serial_baudrate_get(lua_State *L)
{
	uint8_t channel = 0;
	enum __baud baud;
	struct __comm *api_comm = api.query("task_comm");
	
    if(!api_comm)
    {
    	lua_pushnil(L);
        return 1;
    }
    
	channel = (uint8_t)luaL_checknumber(L, 1);
	baud = api_comm->baudrate.get(channel);
	api.release();
	lua_pushnumber(L, ((lua_Number)baud)*100);
	return 1;
}

static int serial_parity_set(lua_State *L)
{
	uint8_t channel = 0;
	uint8_t parity;
	struct __comm *api_comm = api.query("task_comm");
	
    if(!api_comm)
    {
    	lua_pushnil(L);
        return 1;
    }
    
	channel = (uint8_t)luaL_checknumber(L, 1);
	parity = (uint8_t)luaL_checknumber(L, 2);
	parity = api_comm->parity.set(channel, (enum __parity)parity);
	api.release();
	lua_pushnumber(L, (lua_Number)parity);
	return 1;
}

static int serial_parity_get(lua_State *L)
{
	uint8_t channel = 0;
	enum __parity parity;
	struct __comm *api_comm = api.query("task_comm");
	
    if(!api_comm)
    {
    	lua_pushnil(L);
        return 1;
    }
    
	channel = (uint8_t)luaL_checknumber(L, 1);
	parity = api_comm->parity.get(channel);
	api.release();
	lua_pushnumber(L, ((lua_Number)parity));
	return 1;
}

static int serial_stop_set(lua_State *L)
{
	uint8_t channel = 0;
	uint8_t stop;
	struct __comm *api_comm = api.query("task_comm");
	
    if(!api_comm)
    {
    	lua_pushnil(L);
        return 1;
    }
    
	channel = (uint8_t)luaL_checknumber(L, 1);
	stop = (uint8_t)luaL_checknumber(L, 2);
	stop = api_comm->parity.set(channel, (enum __stop)stop);
	api.release();
	lua_pushnumber(L, (lua_Number)stop);
	return 1;
}

static int serial_stop_get(lua_State *L)
{
	uint8_t channel = 0;
	enum __stop stop;
	struct __comm *api_comm = api.query("task_comm");
	
    if(!api_comm)
    {
    	lua_pushnil(L);
        return 1;
    }
    
	channel = (uint8_t)luaL_checknumber(L, 1);
	stop = api_comm->stop.get(channel);
	api.release();
	lua_pushnumber(L, ((lua_Number)stop));
	return 1;
}





static const luaL_Tree comm_tree[] =
{
	{NULL,			"attrib",	NULL},
	{"attrib",		"type",		serial_porttype},
	{"attrib",		"status",	serial_portstatus},
	{NULL,			"timeout",	NULL},
	{"timeout",		"set",		serial_timeout_set},
	{"timeout",		"get",		serial_timeout_get},
	{NULL,			"baudrate",	NULL},
	{"baudrate",	"set",		serial_baudrate_set},
	{"baudrate",	"get",		serial_baudrate_get},
	{NULL,			"parity",	NULL},
	{"parity",		"set",		serial_parity_set},
	{"parity",		"get",		serial_parity_get},
	{NULL,			"stop",		NULL},
	{"stop",		"set",		serial_stop_set},
	{"stop",		"get",		serial_stop_get},
	{NULL,			NULL,		NULL}
};

static int vm_comm_open(lua_State *L)
{
	luaL_packing(L, MODULE_NAME, comm_tree);
	return 1;
}




const luaL_Reg vm_comm = 
{
	.name		= MODULE_NAME,
	.func		= vm_comm_open,
};

