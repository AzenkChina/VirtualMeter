/**
 * @brief		
 * @details		
 * @date		2019-02-16
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "types_timed.h"
#include "vm_timed.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MODULE_NAME		"timed"

/* Private variables ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



static const luaL_Tree timed_tree[] =
{
	{NULL,			NULL,		NULL}
};

static int vm_timed_open(lua_State *L)
{
	luaL_packing(L, MODULE_NAME, timed_tree);
	return 1;
}




const luaL_Reg vm_timed = 
{
	.name		= MODULE_NAME,
	.func		= vm_timed_open,
};

