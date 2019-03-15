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
#include <math.h>

#define lpwrlib_c
#define LUA_LIB

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

#include "comm_socket.h"
#if defined ( __linux )
#include <unistd.h>
#include <signal.h>
#include <time.h>
#else
#include <windows.h>
#if ( defined ( _WIN32 ) || defined ( _WIN64 ) ) && defined ( _MSC_VER )
#pragma comment(lib,"Winmm.lib")
#endif
#endif


/**
  * @brief  计量数据元
  */
enum __metering_meta
{
    R_EPT	 = 0x01, //合相有功电能
    R_EPA	 = 0x02, //A相有功电能
    R_EPB	 = 0x03, //B相有功电能
    R_EPC	 = 0x04, //C相有功电能

    R_EQT	 = 0x05, //合相无功电能
    R_EQA	 = 0x06, //A相无功电能
    R_EQB	 = 0x07, //B相无功电能
    R_EQC	 = 0x08, //C相无功电能

    R_EST	 = 0x09, //合相视在电能
    R_ESA	 = 0x0A, //A相视在电能
    R_ESB	 = 0x0B, //B相视在电能
    R_ESC	 = 0x0C, //C相视在电能

    R_PT	 = 0x0D, //合相有功功率
    R_PA	 = 0x0E, //A相有功功率
    R_PB	 = 0x0F, //B相有功功率
    R_PC	 = 0x10, //C相有功功率

    R_QT	 = 0x11, //合相无功功率
    R_QA	 = 0x12, //A相无功功率
    R_QB	 = 0x13, //B相无功功率
    R_QC	 = 0x14, //C相无功功率

    R_ST	 = 0x15, //合相视在功率
    R_SA	 = 0x16, //A相视在功率
    R_SB	 = 0x17, //B相视在功率
    R_SC	 = 0x18, //C相视在功率

    R_PFT	 = 0x19, //合相功率因数
    R_PFA	 = 0x1A, //A相功率因数
    R_PFB	 = 0x1B, //B相功率因数
    R_PFC	 = 0x1C, //C相功率因数


    R_UARMS	 = 0x1D, //A相电压有效值
    R_UBRMS	 = 0x1E, //B相电压有效值
    R_UCRMS	 = 0x1F, //C相电压有效值

    R_ITRMS	 = 0x20, //三相电流矢量和的有效值
    R_IARMS	 = 0x21, //A相电流有效值
    R_IBRMS	 = 0x22, //B相电流有效值
    R_ICRMS	 = 0x23, //C相电流有效值


    R_PGA	 = 0x24, //A相电流与电压相角
    R_PGB	 = 0x25, //B相电流与电压相角
    R_PGC	 = 0x26, //C相电流与电压相角

    R_YUAUB	 = 0x27, //Ua与Ub的电压夹角
    R_YUAUC	 = 0x28, //Ua与Uc的电压夹角
    R_YUBUC	 = 0x29, //Ub与Uc的电压夹角

    R_FREQ	 = 0x2A, //频率
};




enum __Phase
{
	A = 0,
	B,
	C,
	N,
	T,
};

enum __Quadrant
{
	Active = 0,
	Reactive,
	Apparent,
};

enum __Voltage
{
	Normal = 0,
	Abnormal,
};

enum __Mode
{
	Single = 0,
	Three,
	Four,
};

enum __Item
{
	Voltage = 0x10,
	Current,
	Angle,
	Power,
	Const,
	Mode,
	Inverse,
};

struct __configs
{
    double Voltage[3];
    double Current[4];
    uint16_t Angle[3];
    uint32_t Const[2];
    enum __Voltage Inverse;
    enum __Mode Mode;
	uint8_t Running;
};

struct __energy
{
    double_t Active[4];
    double_t Reactive[4];
    double_t Apparent[4];
};

static struct __configs Configs;
static struct __energy Energy;
static int32_t OutData[42] = {0};
static const double PI = 3.141592653589793;
static SOCKET sock = INVALID_SOCKET;
static SOCKADDR_IN src;
#if defined ( __linux )
static timer_t TimerID = 0;
static struct sigevent ent;
static struct itimerspec value;
#else
static MMRESULT TimerID = 0;
#endif


/**
  * @brief   定时中断函数 100MS
  */
#if defined ( __linux )
static void onTimeFunc(union sigval val)
#else
static void WINAPI onTimeFunc(UINT wTimerID, UINT msg,DWORD dwUser,DWORD dwl,DWORD dw2)
#endif
{
    static uint32_t timing = 0;
    uint16_t count;
	uint16_t numb;
	double IncreasePositive = ((double)1000000 / (double)Configs.Const[0]);
	double IncreaseNegitive = ((double)1000000 / (double)Configs.Const[1]);
	
	timing += 1;

	if(timing >= 5)
	{
		timing = 0;

		if(sock == INVALID_SOCKET)
		{
			sock = emitter.open(50002, &src);
		}
	}

	//mWh
	Energy.Active[0] += OutData[R_PA - 1] / 36000;
	Energy.Active[1] += OutData[R_PB - 1] / 36000;
	Energy.Active[2] += OutData[R_PC - 1] / 36000;
	Energy.Active[3] += OutData[R_PT - 1] / 36000;
	//mVarh
	Energy.Reactive[0] += OutData[R_QA - 1] / 36000;
	Energy.Reactive[1] += OutData[R_QB - 1] / 36000;
	Energy.Reactive[2] += OutData[R_QC - 1] / 36000;
	Energy.Reactive[3] += OutData[R_QT - 1] / 36000;
	//mVAh
	Energy.Apparent[0] += OutData[R_SA - 1] / 36000;
	Energy.Apparent[1] += OutData[R_SB - 1] / 36000;
	Energy.Apparent[2] += OutData[R_SC - 1] / 36000;
	Energy.Apparent[3] += OutData[R_ST - 1] / 36000;

	if(Energy.Active[0] > IncreasePositive)
	{
		numb = Energy.Active[0] / IncreasePositive;
		Energy.Active[0] -= (IncreasePositive * numb);
		OutData[R_EPA - 1] += numb;
	}

	if(Energy.Active[1] > IncreasePositive)
	{
		numb = Energy.Active[1] / IncreasePositive;
		Energy.Active[1] -= (IncreasePositive * numb);
		OutData[R_EPB - 1] += numb;
	}

	if(Energy.Active[2] > IncreasePositive)
	{
		numb = Energy.Active[2] / IncreasePositive;
		Energy.Active[2] -= (IncreasePositive * numb);
		OutData[R_EPC - 1] += numb;
	}

	if(Energy.Active[3] > IncreasePositive)
	{
		numb = Energy.Active[3] / IncreasePositive;
		Energy.Active[3] -= (IncreasePositive * numb);
		OutData[R_EPT - 1] += numb;
	}


	if(Energy.Reactive[0] > IncreaseNegitive)
	{
		numb = Energy.Reactive[0] / IncreaseNegitive;
		Energy.Reactive[0] -= (IncreaseNegitive * numb);
		OutData[R_EQA - 1] += numb;
	}

	if(Energy.Reactive[1] > IncreaseNegitive)
	{
		numb = Energy.Reactive[1] / IncreaseNegitive;
		Energy.Reactive[1] -= (IncreaseNegitive * numb);
		OutData[R_EQB - 1] += numb;
	}

	if(Energy.Reactive[2] > IncreaseNegitive)
	{
		numb = Energy.Reactive[2] / IncreaseNegitive;
		Energy.Reactive[2] -= (IncreaseNegitive * numb);
		OutData[R_EQC - 1] += numb;
	}

	if(Energy.Reactive[3] > IncreaseNegitive)
	{
		numb = Energy.Reactive[3] / IncreaseNegitive;
		Energy.Reactive[3] -= (IncreaseNegitive * numb);
		OutData[R_EQT - 1] += numb;
	}

	if(Energy.Apparent[0] > IncreasePositive)
	{
		numb = Energy.Apparent[0] / IncreasePositive;
		Energy.Apparent[0] -= (IncreasePositive * numb);
		OutData[R_ESA - 1] += numb;
	}

	if(Energy.Apparent[1] > IncreasePositive)
	{
		numb = Energy.Apparent[1] / IncreasePositive;
		Energy.Apparent[1] -= (IncreasePositive * numb);
		OutData[R_ESB - 1] += numb;
	}

	if(Energy.Apparent[2] > IncreasePositive)
	{
		numb = Energy.Apparent[2] / IncreasePositive;
		Energy.Apparent[2] -= (IncreasePositive * numb);
		OutData[R_ESC - 1] += numb;
	}

	if(Energy.Apparent[3] > IncreasePositive)
	{
		numb = Energy.Apparent[3] / IncreasePositive;
		Energy.Apparent[3] -= (IncreasePositive * numb);
		OutData[R_EST - 1] += numb;
	}

	if(sock != INVALID_SOCKET)
	{
		if(emitter.write(sock, &src, (uint8_t *)OutData, sizeof(OutData)) == sizeof(OutData))
		{
			for(count=0; count<(uint16_t)R_ESC; count++)
			{
				OutData[count] = 0;
			}
		}
		else
		{
			emitter.close(sock);
			sock = INVALID_SOCKET;
		}
	}
}

/**
  * @brief   功率源启动
  */
static int pwr_start(lua_State *L)
{
	double val[6] = {0};
	
	sock = emitter.open(50002, &src);
	
	if(Configs.Const[0] < 10)
	{
		Configs.Const[0] = 10;
	}
	
	if(Configs.Const[1] < 10)
	{
		Configs.Const[1] = 10;
	}
	
	Configs.Running = 0xff;
	
	//计算视在功率
	val[0] = Configs.Voltage[0] * Configs.Current[0];
	val[1] = Configs.Voltage[1] * Configs.Current[1];
	val[2] = Configs.Voltage[2] * Configs.Current[2];
	
	//输出视在功率
	if(Configs.Mode == Single)
	{
		OutData[R_SA - 1] = 0;
		OutData[R_SB - 1] = val[1] * 1000;
		OutData[R_SC - 1] = 0;
	}
	else if(Configs.Mode == Three)
	{
		OutData[R_SA - 1] = val[0] * 1000;
		OutData[R_SB - 1] = 0;
		OutData[R_SC - 1] = val[2] * 1000;
	}
	else
	{
		OutData[R_SA - 1] = val[0] * 1000;
		OutData[R_SB - 1] = val[1] * 1000;
		OutData[R_SC - 1] = val[2] * 1000;
	}
	
	OutData[R_ST - 1] = (val[0] + val[1] + val[2]) * 1000;
	
	//输出有功功率
	if(Configs.Mode == Single)
	{
		OutData[R_PA - 1] = 0;
		
		val[3] = val[1] * cos(2 * PI * Configs.Angle[1] / 360);
		val[4] = val[3];
		OutData[R_PB - 1] = val[3] * 1000;
		
		OutData[R_PC - 1] = 0;
		
		OutData[R_PT - 1] = val[4] * 1000;
	}
	else if(Configs.Mode == Three)
	{
		val[3] = val[0] * cos(2 * PI * Configs.Angle[0] / 360);
		val[4] = val[3];
		OutData[R_PA - 1] = val[3] * 1000;
	
		OutData[R_PB - 1] = 0;
	
		val[3] = val[2] * cos(2 * PI * Configs.Angle[2] / 360);
		val[4] += val[3];
		OutData[R_PC - 1] = val[3] * 1000;
	
		OutData[R_PT - 1] = val[4] * 1000;
	}
	else
	{
		val[3] = val[0] * cos(2 * PI * Configs.Angle[0] / 360);
		val[4] = val[3];
		OutData[R_PA - 1] = val[3] * 1000;
	
		val[3] = val[1] * cos(2 * PI * Configs.Angle[1] / 360);
		val[4] += val[3];
		OutData[R_PB - 1] = val[3] * 1000;
	
		val[3] = val[2] * cos(2 * PI * Configs.Angle[2] / 360);
		val[4] += val[3];
		OutData[R_PC - 1] = val[3] * 1000;
	
		OutData[R_PT - 1] = val[4] * 1000;
	}


	//输出无功功率
	if(Configs.Mode == Single)
	{
		OutData[R_QA - 1] = 0;
	
		val[3] = val[1] * sin(2 * PI * Configs.Angle[1] / 360);
		val[5] = val[3];
		OutData[R_QB - 1] = val[3] * 1000;
	
		OutData[R_QC - 1] = 0;
	
		OutData[R_QT - 1] = val[5] * 1000;
	}
	else if(Configs.Mode == Three)
	{
		val[3] = val[0] * sin(2 * PI * Configs.Angle[0] / 360);
		val[5] = val[3];
		OutData[R_QA - 1] = val[3] * 1000;
		
		OutData[R_QB - 1] = 0;
		
		val[3] = val[2] * sin(2 * PI * Configs.Angle[2] / 360);
		val[5] += val[3];
		OutData[R_QC - 1] = val[3] * 1000;
		
		OutData[R_QT - 1] = val[5] * 1000;
	}
	else
	{
		val[3] = val[0] * sin(2 * PI * Configs.Angle[0] / 360);
		val[5] = val[3];
		OutData[R_QA - 1] = val[3] * 1000;
	
		val[3] = val[1] * sin(2 * PI * Configs.Angle[1] / 360);
		val[5] += val[3];
		OutData[R_QB - 1] = val[3] * 1000;
	
		val[3] = val[2] * sin(2 * PI * Configs.Angle[2] / 360);
		val[5] += val[3];
		OutData[R_QC - 1] = val[3] * 1000;
	
		OutData[R_QT - 1] = val[5] * 1000;
	}
	
	
	//输出功率因数
	if(Configs.Mode == Single)
	{
		OutData[R_PFA - 1] = 0;
		OutData[R_PFB - 1] = fabs(cos(2 * PI * Configs.Angle[1] / 360)) * 1000;
		OutData[R_PFC - 1] = 0;
	}
	else if(Configs.Mode == Three)
	{
		OutData[R_PFA - 1] = fabs(cos(2 * PI * Configs.Angle[0] / 360)) * 1000;
		OutData[R_PFB - 1] = 0;
		OutData[R_PFC - 1] = fabs(cos(2 * PI * Configs.Angle[2] / 360)) * 1000;
	}
	else
	{
		OutData[R_PFA - 1] = fabs(cos(2 * PI * Configs.Angle[0] / 360)) * 1000;
		OutData[R_PFB - 1] = fabs(cos(2 * PI * Configs.Angle[1] / 360)) * 1000;
		OutData[R_PFC - 1] = fabs(cos(2 * PI * Configs.Angle[2] / 360)) * 1000;
	}
	
	OutData[R_PFT - 1] = fabs(cos(atan(val[5]/val[4]))) * 1000;
	
	
	//输出电压、电流、功率相角
	if(Configs.Mode == Single)
	{
		OutData[R_UARMS - 1] = 0;
		OutData[R_UBRMS - 1] = Configs.Voltage[1] * 1000;
		OutData[R_UCRMS - 1] = 0;
	
		OutData[R_IARMS - 1] = 0;
		OutData[R_IBRMS - 1] = Configs.Current[1] * 1000;
		OutData[R_ICRMS - 1] = 0;
		OutData[R_ITRMS - 1] = Configs.Current[3] * 1000;
	
		OutData[R_PGA - 1] = 0;
		OutData[R_PGB - 1] = Configs.Angle[1] * 1000;
		OutData[R_PGC - 1] = 0;
	}
	else if(Configs.Mode == Three)
	{
		OutData[R_UARMS - 1] = Configs.Voltage[0] * 1000;
		OutData[R_UBRMS - 1] = 0;
		OutData[R_UCRMS - 1] = Configs.Voltage[2] * 1000;
	
		OutData[R_IARMS - 1] = Configs.Current[0] * 1000;
		OutData[R_IBRMS - 1] = 0;
		OutData[R_ICRMS - 1] = Configs.Current[2] * 1000;
		OutData[R_ITRMS - 1] = 0;
	
		OutData[R_PGA - 1] = Configs.Angle[0] * 1000;
		OutData[R_PGB - 1] = 0;
		OutData[R_PGC - 1] = Configs.Angle[2] * 1000;
	}
	else
	{
		OutData[R_UARMS - 1] = Configs.Voltage[0] * 1000;
		OutData[R_UBRMS - 1] = Configs.Voltage[1] * 1000;
		OutData[R_UCRMS - 1] = Configs.Voltage[2] * 1000;
	
		OutData[R_IARMS - 1] = Configs.Current[0] * 1000;
		OutData[R_IBRMS - 1] = Configs.Current[1] * 1000;
		OutData[R_ICRMS - 1] = Configs.Current[2] * 1000;
		OutData[R_ITRMS - 1] = Configs.Current[3] * 1000;
	
		OutData[R_PGA - 1] = Configs.Angle[0] * 1000;
		OutData[R_PGB - 1] = Configs.Angle[1] * 1000;
		OutData[R_PGC - 1] = Configs.Angle[2] * 1000;
	}
	
	
	//输出电压相角
	if(Configs.Mode == Single)
	{
		OutData[R_YUAUB - 1] = 0;
		OutData[R_YUAUC - 1] = 0;
		OutData[R_YUBUC - 1] = 0;
	}
	else if(Configs.Mode == Three)
	{
		OutData[R_YUAUB - 1] = 120 * 1000;
		OutData[R_YUAUC - 1] = 240 * 1000;
		OutData[R_YUBUC - 1] = 120 * 1000;
	}
	else
	{
		if(Configs.Inverse == Abnormal)
		{
			OutData[R_YUAUB - 1] = 240 * 1000;
			OutData[R_YUAUC - 1] = 120 * 1000;
			OutData[R_YUBUC - 1] = 240 * 1000;
		}
		else
		{
			OutData[R_YUAUB - 1] = 120 * 1000;
			OutData[R_YUAUC - 1] = 240 * 1000;
			OutData[R_YUBUC - 1] = 120 * 1000;
		}
	}

	//输出电网频率
	OutData[R_FREQ - 1] = 50 * 1000;
	
#if defined ( __linux )
    ent.sigev_notify = SIGEV_THREAD;
    ent.sigev_notify_function = onTimeFunc;
    timer_create(CLOCK_MONOTONIC, &ent, &TimerID);
    value.it_value.tv_sec = 0;
    value.it_value.tv_nsec = 100*1000*1000;
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_nsec = 100*1000*1000;
    timer_settime(TimerID, 0, &value, NULL);
#else
	TimerID = timeSetEvent(100, 1, (LPTIMECALLBACK)onTimeFunc, (DWORD_PTR)NULL, TIME_PERIODIC);
#endif
	
	return 1;
}

/**
  * @brief   功率源停止
  */
static int pwr_stop(lua_State *L)
{
	if(TimerID != 0)
	{
#if defined ( __linux )
		timer_delete(TimerID);
#else
		timeKillEvent(TimerID);
#endif
		TimerID = 0;
	}
	
	Configs.Running = 0;
	
#if defined ( __linux )
	sleep(100*1000);
#else
	Sleep(100);
#endif
	
	
    if(sock != INVALID_SOCKET)
    {
		memset(OutData, 0, sizeof(OutData));
		emitter.write(sock, &src, (uint8_t *)OutData, sizeof(OutData));
#if defined ( __linux )
		sleep(100*1000);
#else
		Sleep(100);
#endif
		emitter.close(sock);
        sock = INVALID_SOCKET;
    }
	
	return 1;
}

/**
  * @brief   功率源状态 
  */
static int pwr_status(lua_State *L)
{
	if(Configs.Running == 0xff)
	{
		lua_pushstring(L, "Source is running.");
	}
	else
	{
		lua_pushstring(L, "Source is stoped.");
	}
	
	return 1;
}

/**
  * @brief   功率源获取参数
  */
static int pwr_get(lua_State *L)
{
	int n = lua_gettop(L);
	int param[4];
	int i;
	
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
	
	switch((enum __Item)param[0])
	{
		case Voltage:
		{
			if(n < 2)
			{
				lua_pushliteral(L, "incorrect argument");
				lua_error(L);
			}
		
			if((param[1] >= 0) && (param[1] <= 2))
			{
				lua_pushnumber(L, (lua_Number)Configs.Voltage[param[1]]);
			}
			else
			{
				lua_pushliteral(L, "incorrect argument");
				lua_error(L);
			}
			break;
		}
		case Current:
		{
			if(n < 2)
			{
				lua_pushliteral(L, "incorrect argument");
				lua_error(L);
			}
			
			if((param[1] >= 0) && (param[1] <= 3))
			{
				lua_pushnumber(L, (lua_Number)Configs.Current[param[1]]);
			}
			else
			{
				lua_pushliteral(L, "incorrect argument");
				lua_error(L);
			}
			break;
		}
		case Angle:
		{
			if(n < 2)
			{
				lua_pushliteral(L, "incorrect argument");
				lua_error(L);
			}
		
			if((param[1] >= 0) && (param[1] <= 2))
			{
				lua_pushnumber(L, (lua_Number)Configs.Angle[param[1]]);
			}
			else
			{
				lua_pushliteral(L, "incorrect argument");
				lua_error(L);
			}
			break;
		}
		case Power:
		{
			lua_Number Val;
			
			if(n < 3)
			{
				lua_pushliteral(L, "incorrect argument");
				lua_error(L);
			}
			
			if((param[1] >= 0) && (param[1] <= 4))
			{
				if(param[1] == 4)
				{
					param[1] = 3;
				}
				
				if(param[2] == (int)Active)
				{
					Val = OutData[R_PA - 1 + param[1]];
					Val /= 1000;
					lua_pushnumber(L, Val);
				}
				else if(param[2] == (int)Reactive)
				{
					Val = OutData[R_QA - 1 + param[1]];
					Val /= 1000;
					lua_pushnumber(L, Val);
				}
				else if(param[2] == (int)Apparent)
				{
					Val = OutData[R_SA - 1 + param[1]];
					Val /= 1000;
					lua_pushnumber(L, Val);
				}
				else
				{
					lua_pushliteral(L, "incorrect argument");
					lua_error(L);
				}
			}
			else
			{
				lua_pushliteral(L, "incorrect argument");
				lua_error(L);
			}
			break;
		}
		case Const:
		{
			if(n < 2)
			{
				lua_pushliteral(L, "incorrect argument");
				lua_error(L);
			}
		
			if((param[1] >= 0) && (param[1] <= 1))
			{
				lua_pushnumber(L, (lua_Number)Configs.Const[param[1]]);
			}
			else
			{
				lua_pushliteral(L, "incorrect argument  Get Const");
				lua_error(L);
			}
			break;
		}
		case Mode:
		{
			if(n < 1)
			{
				lua_pushliteral(L, "incorrect argument");
				lua_error(L);
			}
		
			switch(Configs.Mode)
			{
				case Single:
				{
					lua_pushstring(L, "Single Phase");
					break;
				}
				case Three:
				{
					lua_pushstring(L, "Three Wire");
					break;
				}
				case Four:
				{
					lua_pushstring(L, "Four Wire");
					break;
				}
				default:
				{
					lua_pushstring(L, "Unknown");
					break;
				}
			}
			break;
		}
		case Inverse:
		{
			if(n < 1)
			{
				lua_pushliteral(L, "incorrect argument");
				lua_error(L);
			}
		
			switch(Configs.Inverse)
			{
				case Normal:
				{
					lua_pushstring(L, "Voltage normal");
					break;
				}
				case Abnormal:
				{
					lua_pushstring(L, "Voltage inverse");
					break;
				}
				default:
				{
					lua_pushstring(L, "Unknown");
					break;
				}
			}
			break;
		}
		default:
		{
			lua_pushliteral(L, "incorrect argument");
			lua_error(L);
		}
	}
	
    return 1;
}

/**
  * @brief   功率源设置参数
  */
static int pwr_set(lua_State *L)
{
	int n = lua_gettop(L);
	lua_Number param[4];
	int i;
	
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
		
		param[i-1] = lua_tonumber(L, i);
	}
	
	
	lua_number2integer(i, param[0]);
	
	switch((enum __Item)i)
	{
		case Voltage:
		{
			if(n < 3)
			{
				lua_pushliteral(L, "incorrect argument");
				lua_error(L);
			}
			
			if((param[2] > 10000) || (param[2] < 0))
			{
				lua_pushliteral(L, "incorrect argument");
				lua_error(L);
			}
			
			lua_number2integer(i, param[1]);
			
			switch((enum __Phase)i)
			{
				case A:
				{
					Configs.Voltage[0] = param[2];
					break;
				}
				case B:
				{
					Configs.Voltage[1] = param[2];
					break;
				}
				case C:
				{
					Configs.Voltage[2] = param[2];
					break;
				}
				default:
				{
					lua_pushliteral(L, "incorrect argument");
					lua_error(L);
				}
			}
			break;
		}
		case Current:
		{
			if(n < 3)
			{
				lua_pushliteral(L, "incorrect argument");
				lua_error(L);
			}
			
			if((param[2] > 10000) || (param[2] < 0))
			{
				lua_pushliteral(L, "incorrect argument");
				lua_error(L);
			}
			
			lua_number2integer(i, param[1]);
			
			switch((enum __Phase)i)
			{
				case A:
				{
					Configs.Current[0] = param[2];
					break;
				}
				case B:
				{
					Configs.Current[1] = param[2];
					break;
				}
				case C:
				{
					Configs.Current[2] = param[2];
					break;
				}
				case N:
				{
					Configs.Current[3] = param[2];
					break;
				}
				default:
				{
					lua_pushliteral(L, "incorrect argument");
					lua_error(L);
				}
			}
			break;
		}
		case Angle:
		{
			if(n < 3)
			{
				lua_pushliteral(L, "incorrect argument");
				lua_error(L);
			}
			
			if((param[2] > 360) || (param[2] < 0))
			{
				lua_pushliteral(L, "incorrect argument");
				lua_error(L);
			}
			
			lua_number2integer(i, param[1]);
			
			switch((enum __Phase)i)
			{
				case A:
				{
					Configs.Angle[0] = param[2];
					break;
				}
				case B:
				{
					Configs.Angle[1] = param[2];
					break;
				}
				case C:
				{
					Configs.Angle[2] = param[2];
					break;
				}
				default:
				{
					lua_pushliteral(L, "incorrect argument");
					lua_error(L);
				}
			}
			break;
		}
		case Const:
		{
			if(Configs.Running == 0xff)
			{
				lua_pushstring(L, "Forbidden when source is running");
				return 1;
			}
			
			if(n < 3)
			{
				lua_pushliteral(L, "incorrect argument");
				lua_error(L);
			}
			
			if((param[2] > 5000000) || (param[2] < 10))
			{
				lua_pushliteral(L, "incorrect argument");
				lua_error(L);
			}
			
			lua_number2integer(i, param[1]);
			
			switch((enum __Quadrant)i)
			{
				case Active:
				{
					lua_number2integer(i, param[2]);
					Configs.Const[0] = i;
					break;
				}
				case Reactive:
				{
					lua_number2integer(i, param[2]);
					Configs.Const[1] = i;
					break;
				}
				default:
				{
					lua_pushliteral(L, "incorrect argument");
					lua_error(L);
				}
			}
			break;
		}
		case Mode:
		{
			if(n < 2)
			{
				lua_pushliteral(L, "incorrect argument");
				lua_error(L);
			}
			
			lua_number2integer(i, param[1]);
			
			switch((enum __Mode)i)
			{
				case Single:
				case Three:
				case Four:
				{
					Configs.Mode = i;
					break;
				}
				default:
				{
					lua_pushliteral(L, "incorrect argument");
					lua_error(L);
				}
			}
			break;
		}
		case Inverse:
		{
			if(n < 2)
			{
				lua_pushliteral(L, "incorrect argument");
				lua_error(L);
			}
			
			lua_number2integer(i, param[1]);
			
			switch((enum __Voltage)i)
			{
				case Normal:
				case Abnormal:
				{
					Configs.Inverse = i;
					break;
				}
				default:
				{
					lua_pushliteral(L, "incorrect argument");
					lua_error(L);
				}
			}
			break;
		}
		default:
		{
			lua_pushliteral(L, "incorrect argument");
			lua_error(L);
		}
	}
	
    return 1;
}


static const luaL_Reg syslib[] =
{
	{"start", pwr_start},
	{"stop", pwr_stop},
	{"status", pwr_status},
	{"get", pwr_get},
	{"set", pwr_set},
	{NULL, NULL}
};


static const luaL_Enmu enmu_mode[] =
{
	{"Single",	(int)Single},
	{"Three",	(int)Three},
	{"Four",	(int)Four},
	{NULL,		0}
};

static const luaL_Enmu enmu_phase[] =
{
	{"A",	(int)A},
	{"B",	(int)B},
	{"C",	(int)C},
	{"N",	(int)N},
	{"T",	(int)T},
	{NULL,	0}
};

static const luaL_Enmu enmu_quadrant[] =
{
	{"Active",		(int)Active},
	{"Reactive",	(int)Reactive},
	{"Apparent",	(int)Apparent},
	{NULL,			0}
};

static const luaL_Enmu enmu_voltage[] =
{
	{"Normal",		(int)Normal},
	{"Abnormal",	(int)Abnormal},
	{NULL,			0}
};

static const luaL_Enmu enmu_item[] =
{
	{"Voltage",		(int)Voltage},
	{"Current",		(int)Current},
	{"Angle",		(int)Angle},
	{"Power",		(int)Power},
	{"Const",		(int)Const},
	{"Mode",		(int)Mode},
	{"Inverse",		(int)Inverse},
	{NULL,			0}
};

LUALIB_API int luaopen_power(lua_State *L)
{
	luaL_register(L, LUA_PWRLIBNAME, syslib);
	
	luaL_enumerating(L, "__Mode", enmu_mode);
	luaL_enumerating(L, "__Phase", enmu_phase);
	luaL_enumerating(L, "__Quadrant", enmu_quadrant);
	luaL_enumerating(L, "__Voltage", enmu_voltage);
	luaL_enumerating(L, "__Item", enmu_item);
	
	memset(&Configs, 0, sizeof(Configs));
	memset(&Energy, 0, sizeof(Energy));
	memset(OutData, 0, sizeof(OutData));
	
	return 1;
}
