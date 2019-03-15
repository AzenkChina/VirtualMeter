/**
 * @brief		
 * @details		
 * @date		2019-02-16
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "types_logger.h"
#include "vm_logger.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MODULE_NAME		"logger"

/* Private variables ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



static const luaL_Tree logger_tree[] =
{
	{NULL,			NULL,		NULL}
};

static int vm_logger_open(lua_State *L)
{
	luaL_packing(L, MODULE_NAME, logger_tree);
	return 1;
}




const luaL_Reg vm_logger = 
{
	.name		= MODULE_NAME,
	.func		= vm_logger_open,
};

