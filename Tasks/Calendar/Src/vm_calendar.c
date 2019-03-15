/**
 * @brief		
 * @details		
 * @date		2019-02-16
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "types_calendar.h"
#include "vm_calendar.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MODULE_NAME		"calendar"

/* Private variables ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



static const luaL_Tree calendar_tree[] =
{
	{NULL,			NULL,		NULL}
};

static int vm_calendar_open(lua_State *L)
{
	luaL_packing(L, MODULE_NAME, calendar_tree);
	return 1;
}




const luaL_Reg vm_calendar = 
{
	.name		= MODULE_NAME,
	.func		= vm_calendar_open,
};

