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
static int file_parameter_read(lua_State *L)
{
	size_t len;
	const char *name;
	uint32_t offset;
	uint32_t size;
	void *buff;
	uint32_t result;
	
	name = (const char *)luaL_checklstring(L, 1, &len);
	offset = (uint32_t)luaL_checknumber(L, 2);
	size = (uint32_t)luaL_checknumber(L, 3);
	
	if(size)
	{
		buff = malloc(size);
		memset(buff, 0, size);
	}
	
	result = file.parameter.read(name, offset, size, buff);
    
	lua_pushlstring(L, buff, result);
	
	if(size)
	{
		free(buff);
	}
	
	return 1;
}

static int file_parameter_write(lua_State *L)
{
	size_t len;
	const char *name;
	uint32_t offset;
	uint32_t size;
	void *buff;
	uint32_t result;
	
	name = (const char *)luaL_checklstring(L, 1, &len);
	offset = (uint32_t)luaL_checknumber(L, 2);
	size = (uint32_t)luaL_checknumber(L, 3);
	buff = (void *)luaL_checklstring(L, 4, &len);
	
	result = file.parameter.write(name, offset, size, buff);
    
	lua_pushnumber(L, result);
	return 1;
}

static int file_parameter_size(lua_State *L)
{
	size_t len;
	const char *name;
	uint32_t result;
	
	name = (const char *)luaL_checklstring(L, 1, &len);
	
	result = file.parameter.size(name);
    
	lua_pushnumber(L, result);
	return 1;
}


static int file_ring_read(lua_State *L)
{
	size_t len;
	const char *name;
	uint32_t index;
	uint32_t size;
	bool reverse = false;
	void *buff;
	uint32_t result;
	
	name = (const char *)luaL_checklstring(L, 1, &len);
	index = (uint32_t)luaL_checknumber(L, 2);
	size = (uint32_t)luaL_checknumber(L, 3);
	reverse = luaL_checkinteger(L, 4);
	
	if(size)
	{
		buff = malloc(size);
		memset(buff, 0, size);
	}
	
	result = file.ring.read(name, index, size, buff, reverse);
    
	lua_pushlstring(L, buff, result);
	
	if(size)
	{
		free(buff);
	}
	
	return 1;
}

static int file_ring_append(lua_State *L)
{
	size_t len;
	const char *name;
	uint32_t size;
	void *buff;
	uint32_t result;
	
	name = (const char *)luaL_checklstring(L, 1, &len);
	size = (uint32_t)luaL_checknumber(L, 2);
	buff = (void *)luaL_checklstring(L, 3, &len);
	
	result = file.ring.append(name, size, buff);
    
	lua_pushnumber(L, result);
	return 1;
}

static int file_ring_truncate(lua_State *L)
{
	size_t len;
	const char *name;
	uint32_t amount;
	bool reverse = false;
	uint32_t result;
	
	name = (const char *)luaL_checklstring(L, 1, &len);
	amount = (uint32_t)luaL_checknumber(L, 2);
	reverse = luaL_checkinteger(L, 3);
	
	result = file.ring.truncate(name, amount, reverse);
    
	lua_pushnumber(L, result);
	
	return 1;
}

static int file_ring_info(lua_State *L)
{
	size_t len;
	const char *name;
	bool result;
	struct __ring_info ring_info;
	
	name = (const char *)luaL_checklstring(L, 1, &len);
	
	result = file.ring.info(name, &ring_info);
	
    lua_pushnumber(L, ring_info.amount);
    lua_pushnumber(L, ring_info.length);
    lua_pushnumber(L, ring_info.capacity);
	lua_pushnumber(L, result);
	
	return 4;
}

static int file_ring_reset(lua_State *L)
{
	size_t len;
	const char *name;
	bool result;
	
	name = (const char *)luaL_checklstring(L, 1, &len);
	
	result = file.ring.reset(name);
	
	lua_pushnumber(L, result);
	
	return 1;
}

static int file_ring_init(lua_State *L)
{
	size_t len;
	const char *name;
	uint32_t length;
	bool result;
	
	name = (const char *)luaL_checklstring(L, 1, &len);
	length = (uint32_t)luaL_checknumber(L, 2);
	
	result = file.ring.init(name, length);
	
	lua_pushnumber(L, result);
	
	return 1;
}


static int file_parallel_read(lua_State *L)
{
	size_t len;
	const char *name;
	uint32_t index;
	uint32_t size;
	void *buff;
	uint32_t result;
	
	name = (const char *)luaL_checklstring(L, 1, &len);
	index = (uint32_t)luaL_checknumber(L, 2);
	size = (uint32_t)luaL_checknumber(L, 3);
	
	if(size)
	{
		buff = malloc(size);
		memset(buff, 0, size);
	}
	
	result = file.parallel.read(name, index, size, buff);
    
	lua_pushlstring(L, buff, result);
	
	if(size)
	{
		free(buff);
	}
	
	return 1;
}

static int file_parallel_write(lua_State *L)
{
	size_t len;
	const char *name;
	uint32_t index;
	uint32_t size;
	void *buff;
	uint32_t result;
	
	name = (const char *)luaL_checklstring(L, 1, &len);
	index = (uint32_t)luaL_checknumber(L, 2);
	size = (uint32_t)luaL_checknumber(L, 3);
	buff = (void *)luaL_checklstring(L, 4, &len);
	
	result = file.parallel.write(name, index, size, buff);
    
	lua_pushnumber(L, result);
	return 1;
}

static int file_parallel_signature(lua_State *L)
{
	size_t len;
	const char *name;
	uint32_t sign;
	bool result;
	
	name = (const char *)luaL_checklstring(L, 1, &len);
	
	result = file.parallel.signature(name, &sign);
    
    lua_pushnumber(L, sign);
	lua_pushnumber(L, result);
	return 2;
}

static int file_parallel_status(lua_State *L)
{
	size_t len;
	const char *name;
	uint32_t index;
	bool result;
	
	name = (const char *)luaL_checklstring(L, 1, &len);
	index = (uint32_t)luaL_checknumber(L, 2);
	
	result = file.parallel.status(name, index);
    
	lua_pushnumber(L, result);
	return 1;
}

static int file_parallel_renew(lua_State *L)
{
	size_t len;
	const char *name;
	uint32_t index;
	bool result;
	
	name = (const char *)luaL_checklstring(L, 1, &len);
	index = (uint32_t)luaL_checknumber(L, 2);
	
	result = file.parallel.renew(name, index);
    
	lua_pushnumber(L, result);
	return 1;
}

static int file_parallel_info(lua_State *L)
{
	size_t len;
	const char *name;
	uint32_t index;
	bool result;
	struct __parallel_info info;
	
	name = (const char *)luaL_checklstring(L, 1, &len);
	
	result = file.parallel.info(name, &info);
    
	lua_pushnumber(L, info.capacity);
	lua_pushnumber(L, info.length);
	lua_pushnumber(L, result);
	return 3;
}

static int file_parallel_reset(lua_State *L)
{
	size_t len;
	const char *name;
	bool result;
	
	name = (const char *)luaL_checklstring(L, 1, &len);
	
	result = file.parallel.reset(name);
    
	lua_pushnumber(L, result);
	return 1;
}

static int file_parallel_init(lua_State *L)
{
	size_t len;
	const char *name;
	uint32_t length;
	bool result;
	
	name = (const char *)luaL_checklstring(L, 1, &len);
	length = (uint32_t)luaL_checknumber(L, 2);
	
	result = file.parallel.init(name, length);
    
	lua_pushnumber(L, result);
	return 1;
}



static const luaL_Tree basis_tree[] =
{
	{NULL,				"file",			NULL},
	{"file",			"parameter",	NULL},
	{"parameter",		"read",			file_parameter_read},
	{"parameter",		"write",		file_parameter_write},
	{"parameter",		"size",			file_parameter_size},
	
	{"file",			"ring",			NULL},
	{"ring",			"read",			file_ring_read},
	{"ring",			"append",		file_ring_append},
	{"ring",			"truncate",		file_ring_truncate},
	{"ring",			"info",			file_ring_info},
	{"ring",			"reset",		file_ring_reset},
	{"ring",			"init",			file_ring_init},
	
	{"file",			"parallel",		NULL},
	{"parallel",		"read",			file_parallel_read},
	{"parallel",		"write",		file_parallel_write},
	{"parallel",		"signature",	file_parallel_signature},
	{"parallel",		"status",		file_parallel_status},
	{"parallel",		"renew",		file_parallel_renew},
	{"parallel",		"info",			file_parallel_info},
	{"parallel",		"reset",		file_parallel_reset},
	{"parallel",		"init",			file_parallel_init},
	
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

