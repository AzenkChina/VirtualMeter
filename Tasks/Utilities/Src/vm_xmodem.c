/**
 * @brief		
 * @details		
 * @date		2019-02-16
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "types_xmodem.h"
#include "vm_xmodem.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MODULE_NAME		"xmodem"

/* Private variables ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



static const luaL_Tree xmodem_tree[] =
{
	{NULL,			NULL,		NULL}
};

static int vm_xmodem_open(lua_State *L)
{
	luaL_packing(L, MODULE_NAME, xmodem_tree);
	return 1;
}




const luaL_Reg vm_xmodem = 
{
	.name		= MODULE_NAME,
	.func		= vm_xmodem_open,
};

