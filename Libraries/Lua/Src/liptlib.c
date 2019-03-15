/**
  * @file
  * @author  azenk
  * @version
  * @date
  * @brief
  */

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define liptlib_c
#define LUA_LIB

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "comm_socket.h"
#if defined ( __linux )
#include <unistd.h>
#else
#include <windows.h>
#endif

enum __KeyID
{
	N0 = 0,
	N1,
	N2,
	N3,
	N4,
	N5,
	N6,
	N7,
	N8,
	N9,

	BACK = 0x0A,
	ENTER = 0x0B,
	PROG = 0x0C,

	UP = 0x10,
	DOWN = 0x20,
};

enum __KeyOption
{
	NONE = 0,
	PRESSED,
	RELEASED,
	LONG_PRESSED,
	LONG_RELEASED,
};

enum __Switch
{
	OPEN = 0,
	CLOSE,
	DONTCARE,
	UNKNOWN,
};

enum __BatteryID
{
	RTC = 0,
	BACKUP,
};

enum __Battery
{
	FULL = 0,
	LOW,
	EMPTY,
};

enum __SensorID
{
    MAIN_COVER = 0,
    SUB_COVER,
    MAGNETIC,
};

struct __key_data
{
    uint16_t ID;
    enum __KeyOption Status;
};

struct __battery_data
{
    enum __BatteryID Type;
    enum __Battery Status;
};

struct __sensors_data
{
    enum __SensorID Type;
    enum __Switch Status;
};

struct __relay_data
{
    enum __Switch Status;
};

static struct __key_data KeyData;
static struct __battery_data BatteryData;
static struct __sensors_data SensorsData;
static struct __relay_data RelayData;

static SOCKET sock_key = INVALID_SOCKET;
static SOCKADDR_IN src_key;
static SOCKET sock_battery = INVALID_SOCKET;
static SOCKADDR_IN src_battery;
static SOCKET sock_sensors = INVALID_SOCKET;
static SOCKADDR_IN src_sensors;
static SOCKET sock_relay = INVALID_SOCKET;
static SOCKADDR_IN src_relay;


static int ipt_keyboard(lua_State *L)
{
	int n = lua_gettop(L);
	int param[4];
	int i;
	
	memset(&KeyData, 0, sizeof(KeyData));
	
	if(n > 4)
	{
		lua_pushliteral(L, "incorrect argument");
		lua_error(L);
	}
	
	for (i = 1; i <= n; i++)
	{
		if (!lua_isnumber(L, i))
		{
			lua_pushliteral(L, "incorrect argument");
			lua_error(L);
		}
		
		lua_number2integer(param[i-1], lua_tonumber(L, i));
	}
	
	switch((enum __KeyID)param[0])
	{
		case N0:
		case N1:
		case N2:
		case N3:
		case N4:
		case N5:
		case N6:
		case N7:
		case N8:
		case N9:
		case BACK:
		case ENTER:
		case PROG:
		case UP:
		case DOWN:
		{
			KeyData.ID = (uint16_t)param[0];
			break;
		}
		default:
		{
			lua_pushliteral(L, "incorrect argument");
			lua_error(L);
		}
	}
	
	switch((enum __KeyOption)param[1])
	{
		case PRESSED:
		case RELEASED:
		case LONG_PRESSED:
		case LONG_RELEASED:
		{
			KeyData.Status = (enum __KeyOption)param[1];
			break;
		}
		case NONE:
		{
			return 1;
		}
		default:
		{
			lua_pushliteral(L, "incorrect argument");
			lua_error(L);
		}
	}
	
	sock_key = emitter.open(50006, &src_key);

	if(sock_key != INVALID_SOCKET)
	{
		emitter.write(sock_key, &src_key, (uint8_t *)&KeyData, sizeof(KeyData));
#if defined ( __linux )
		usleep(200*1000);
#else
		Sleep(200);
#endif
		emitter.close(sock_key);
        sock_key = INVALID_SOCKET;
	}
	
	return 1;
}

static int ipt_relay(lua_State *L)
{
	int n = lua_gettop(L);
	int param[4];
	int i;
	
	memset(&RelayData, 0, sizeof(RelayData));
	
	if(n > 4)
	{
		lua_pushliteral(L, "incorrect argument");
		lua_error(L);
	}
	
	for (i = 1; i <= n; i++)
	{
		if (!lua_isnumber(L, i))
		{
			lua_pushliteral(L, "incorrect argument");
			lua_error(L);
		}
		
		lua_number2integer(param[i-1], lua_tonumber(L, i));
	}
	
	switch((enum __Switch)param[0])
	{
		case OPEN:
		case CLOSE:
		case DONTCARE:
		case UNKNOWN:
		{
			RelayData.Status = (enum __Switch)param[0];
			break;
		}
		default:
		{
			lua_pushliteral(L, "incorrect argument");
			lua_error(L);
		}
	}
	
	sock_relay = emitter.open(50004, &src_relay);

	if(sock_relay != INVALID_SOCKET)
	{
		emitter.write(sock_relay, &src_relay, (uint8_t *)&RelayData, sizeof(RelayData));
#if defined ( __linux )
		usleep(200*1000);
#else
		Sleep(200);
#endif
		emitter.close(sock_relay);
        sock_relay = INVALID_SOCKET;
	}
	
	return 1;
}

static int ipt_battery(lua_State *L)
{
	int n = lua_gettop(L);
	int param[4];
	int i;
	
	memset(&BatteryData, 0, sizeof(BatteryData));
	
	if(n > 4)
	{
		lua_pushliteral(L, "incorrect argument");
		lua_error(L);
	}
	
	for (i = 1; i <= n; i++)
	{
		if (!lua_isnumber(L, i))
		{
			lua_pushliteral(L, "incorrect argument");
			lua_error(L);
		}
		
		lua_number2integer(param[i-1], lua_tonumber(L, i));
	}
	
	switch((enum __BatteryID)param[0])
	{
		case RTC:
		case BACKUP:
		{
			BatteryData.Type = (enum __BatteryID)param[0];
			break;
		}
		default:
		{
			lua_pushliteral(L, "incorrect argument");
			lua_error(L);
		}
	}

	switch((enum __Battery)param[1])
	{
		case FULL:
		case LOW:
		case EMPTY:
		{
			BatteryData.Status = (enum __Battery)param[1];
			break;
		}
		default:
		{
			lua_pushliteral(L, "incorrect argument");
			lua_error(L);
		}
	}
	
	sock_battery = emitter.open(50005, &src_battery);

	if(sock_battery != INVALID_SOCKET)
	{
		emitter.write(sock_battery, &src_battery, (uint8_t *)&BatteryData, sizeof(BatteryData));
#if defined ( __linux )
		usleep(200*1000);
#else
		Sleep(200);
#endif
		emitter.close(sock_battery);
        sock_battery = INVALID_SOCKET;
	}
	
	return 1;
}

static int ipt_sensors(lua_State *L)
{
	int n = lua_gettop(L);
	int param[4];
	int i;
	
	memset(&SensorsData, 0, sizeof(SensorsData));
	
	if(n > 4)
	{
		lua_pushliteral(L, "incorrect argument");
		lua_error(L);
	}
	
	for (i = 1; i <= n; i++)
	{
		if (!lua_isnumber(L, i))
		{
			lua_pushliteral(L, "incorrect argument");
			lua_error(L);
		}
		
		lua_number2integer(param[i-1], lua_tonumber(L, i));
	}
	
	switch((enum __SensorID)param[0])
	{
		case MAIN_COVER:
		case SUB_COVER:
		case MAGNETIC:
		{
			SensorsData.Type = (enum __SensorID)param[0];
			break;
		}
		default:
		{
			lua_pushliteral(L, "incorrect argument");
			lua_error(L);
		}
	}
	
	switch((enum __Switch)param[1])
	{
		case OPEN:
		case CLOSE:
		case DONTCARE:
		case UNKNOWN:
		{
			SensorsData.Status = (enum __Switch)param[1];
			break;
		}
		default:
		{
			lua_pushliteral(L, "incorrect argument");
			lua_error(L);
		}
	}
	
	sock_sensors = emitter.open(50003, &src_sensors);

	if(sock_sensors != INVALID_SOCKET)
	{
		emitter.write(sock_sensors, &src_sensors, (uint8_t *)&BatteryData, sizeof(BatteryData));
#if defined ( __linux )
		usleep(200*1000);
#else
		Sleep(200);
#endif
		emitter.close(sock_sensors);
        sock_sensors = INVALID_SOCKET;
	}
	
	return 1;
}



static const luaL_Reg syslib[] =
{
	{"keyboard", ipt_keyboard},
	{"relay", ipt_relay},
	{"battery", ipt_battery},
	{"sensors", ipt_sensors},
	{NULL, NULL}
};


static const luaL_Enmu enmu_key_id[] =
{
	{"N0",		(int)N0},
	{"N1",		(int)N1},
	{"N2",		(int)N2},
	{"N3",		(int)N3},
	{"N4",		(int)N4},
	{"N5",		(int)N5},
	{"N6",		(int)N6},
	{"N7",		(int)N7},
	{"N8",		(int)N8},
	{"N9",		(int)N9},
	{"BACK", 	(int)BACK},
	{"ENTER",	(int)ENTER},
	{"PROG",	(int)PROG},
	{"UP",		(int)UP},
	{"DOWN",	(int)DOWN},
	{NULL,		0}
};

static const luaL_Enmu enmu_key_option[] =
{
	{"NONE",			(int)NONE},
	{"PRESSED",			(int)PRESSED},
	{"RELEASED",		(int)RELEASED},
	{"LONG_PRESSED",	(int)LONG_PRESSED},
	{"LONG_RELEASED",	(int)LONG_RELEASED},
	{NULL,				0}
};

static const luaL_Enmu enmu_switch[] =
{
	{"OPEN",		(int)OPEN},
	{"CLOSE",		(int)CLOSE},
	{"DONTCARE",	(int)DONTCARE},
	{"UNKNOWN",		(int)UNKNOWN},
	{NULL,			0}
};

static const luaL_Enmu enmu_battery[] =
{
	{"FULL",		(int)FULL},
	{"LOW",			(int)LOW},
	{"EMPTY",		(int)EMPTY},
	{NULL,			0}
};

static const luaL_Enmu enmu_battery_id[] =
{
	{"RTC",			(int)RTC},
	{"BACKUP",		(int)BACKUP},
	{NULL,			0}
};

static const luaL_Enmu enmu_sensor_id[] =
{
	{"MAIN_COVER",	(int)MAIN_COVER},
	{"SUB_COVER",	(int)SUB_COVER},
	{"MAGNETIC",	(int)MAGNETIC},
	{NULL,			0}
};

/* }====================================================== */



LUALIB_API int luaopen_input(lua_State *L)
{
	luaL_register(L, LUA_INPUTLIBNAME, syslib);
	
	luaL_enumerating(L, "__KeyID", enmu_key_id);
	luaL_enumerating(L, "__KeyOption", enmu_key_option);
	luaL_enumerating(L, "__Switch", enmu_switch);
	luaL_enumerating(L, "__BatteryID", enmu_battery_id);
	luaL_enumerating(L, "__Battery", enmu_battery);
	luaL_enumerating(L, "__SensorID", enmu_sensor_id);
	
	memset(&KeyData, 0, sizeof(KeyData));
	memset(&BatteryData, 0, sizeof(BatteryData));
	memset(&SensorsData, 0, sizeof(SensorsData));
	memset(&RelayData, 0, sizeof(RelayData));
	
	return 1;
}

