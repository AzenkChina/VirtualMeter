/**
 * @brief		
 * @details		
 * @date		2019-02-16
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "types_protocol.h"
#include "vm_protocol.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MODULE_NAME		"protocol"

/* Private variables ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



static const luaL_Tree protocol_tree[] =
{
	{NULL,			NULL,		NULL}
};

static int vm_protocol_open(lua_State *L)
{
	luaL_packing(L, MODULE_NAME, protocol_tree);
	return 1;
}




const luaL_Reg vm_protocol = 
{
	.name		= MODULE_NAME,
	.func		= vm_protocol_open,
};

