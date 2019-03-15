/**
 * @brief		
 * @details		
 * @date		2019-02-16
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "types_keyboard.h"
#include "vm_keyboard.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MODULE_NAME		"keyboard"

/* Private variables ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static int waketime_get(lua_State *L)
{
	uint8_t second = 0;
	struct __keyboard *api_keyboard = api.query("task_keyboard");
	
    if(!api_keyboard)
    {
    	lua_pushnil(L);
        return 1;
    }
	second = api_keyboard->waketime.get();
	api.release();
	lua_pushnumber(L, (lua_Number)second);
	return 1;
}

static int waketime_set(lua_State *L)
{
	uint8_t second = 0;
	struct __keyboard *api_keyboard = api.query("task_keyboard");
	
    if(!api_keyboard)
    {
    	lua_pushnil(L);
        return 1;
    }
    second = (uint8_t)luaL_checknumber(L, 1);
	second = api_keyboard->waketime.set(second);
	api.release();
	lua_pushnumber(L, (lua_Number)second);
	return 1;
}

static const luaL_Tree keyboard_tree[] =
{
	{NULL,			"waketime",		NULL},
	{"waketime",	"get",			waketime_get},
	{"waketime",	"set",			waketime_set}, 
	{NULL,			NULL,			NULL},
};

static int vm_keyboard_open(lua_State *L)
{
	luaL_packing(L, MODULE_NAME, keyboard_tree);
	return 1;
}




const luaL_Reg vm_keyboard = 
{
	.name		= MODULE_NAME,
	.func		= vm_keyboard_open,
};

