/**
 * @brief		
 * @details		
 * @date		2019-02-16
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "types_disconnect.h"
#include "vm_disconnect.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MODULE_NAME		"disconnect"

/* Private variables ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



static const luaL_Tree disconnect_tree[] =
{
	{NULL,			NULL,		NULL}
};

static int vm_disconnect_open(lua_State *L)
{
	luaL_packing(L, MODULE_NAME, disconnect_tree);
	return 1;
}




const luaL_Reg vm_disconnect = 
{
	.name		= MODULE_NAME,
	.func		= vm_disconnect_open,
};

