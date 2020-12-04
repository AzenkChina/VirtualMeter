/**
 * @brief		
 * @details		
 * @date		2019-09-28
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "vm_basis.h"
#include "string.h"
#include "stdlib.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MODULE_NAME			"basis"

/* Private variables ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static int file_read(lua_State *L)
{
	size_t len;
	const char *name;
	uint32_t offset;
	uint32_t count;
	void *buff;
	uint32_t result;
	
	name = (const char *)luaL_checklstring(L, 1, &len);
	offset = (uint32_t)luaL_checknumber(L, 2);
	count = (uint32_t)luaL_checknumber(L, 3);
	
	if(count)
	{
		buff = malloc(count);
		memset(buff, 0, count);
	}
	
	result = file.parameter.read(name, offset, count, buff);
    
	lua_pushlstring(L, buff, result);
	
	if(count)
	{
		free(buff);
	}
	
	return 1;
}

static int file_write(lua_State *L)
{
	size_t len;
	const char *name;
	uint32_t offset;
	uint32_t count;
	void *buff;
	uint32_t result;
	
	name = (const char *)luaL_checklstring(L, 1, &len);
	offset = (uint32_t)luaL_checknumber(L, 2);
	count = (uint32_t)luaL_checknumber(L, 3);
	buff = (void *)luaL_checklstring(L, 4, &len);
	
	result = file.parameter.write(name, offset, count, buff);
    
	lua_pushnumber(L, result);
	return 1;
}

static int file_size(lua_State *L)
{
	size_t len;
	const char *name;
	uint32_t result;
	
	name = (const char *)luaL_checklstring(L, 1, &len);
	
	result = file.parameter.size(name);
    
	lua_pushnumber(L, result);
	return 1;
}

static const luaL_Tree basis_tree[] =
{
	{NULL,				"file",			NULL},
	{"file",			"parameter",	NULL},
	{"parameter",		"read",			file_read},
	{"parameter",		"write",		file_write},
	{"parameter",		"size",			file_size},
	{NULL,				NULL,			NULL}
};

static int vm_basis_open(lua_State *L)
{
	luaL_packing(L, MODULE_NAME, basis_tree);
	return 1;
}




const luaL_Reg vm_basis = 
{
	.name		= MODULE_NAME,
	.func		= vm_basis_open,
};

