/**
 * @brief		
 * @details		
 * @date		azenk@2019-01-10
 **/

/* Includes ------------------------------------------------------------------*/
#include "system.h"
#include "axdr.h"
#include "cosem_objects_clock.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**	
  * @brief 
  */
static ObjectErrs GetLogicalName(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetLogicalName(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetTime(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetTime(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetTimeZone(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetTimeZone(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetStatus(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetStatus(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetDaylightSavingsBegin(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetDaylightSavingsBegin(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetDaylightSavingsEnd(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetDaylightSavingsEnd(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetDaylightSavingsDeviation(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetDaylightSavingsDeviation(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetDaylightSavingsEnabled(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetDaylightSavingsEnabled(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs GetClockBase(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs SetClockBase(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}


/**	
  * @brief 
  */
static ObjectErrs AdjusttoQuarter(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs AdjusttoMeasuringPeriod(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs AdjusttoMinute(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs AdjusttoPresetTime(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs PresetAdjustingTime(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}

/**	
  * @brief 
  */
static ObjectErrs ShiftTime(ObjectPara *P)
{
    return(OBJECT_ERR_NODEF);
}


const ClassClock Clock = 
{
    .GetLogicalName                 = GetLogicalName,
    .SetLogicalName                 = SetLogicalName,
    .GetTime                        = GetTime,
    .SetTime                        = SetTime,
    .GetTimeZone                    = GetTimeZone,
    .SetTimeZone                    = SetTimeZone,
    .GetStatus                      = GetStatus,
    .SetStatus                      = SetStatus,
    .GetDaylightSavingsBegin        = GetDaylightSavingsBegin,
    .SetDaylightSavingsBegin        = SetDaylightSavingsBegin,
    .GetDaylightSavingsEnd          = GetDaylightSavingsEnd,
    .SetDaylightSavingsEnd          = SetDaylightSavingsEnd,
    .GetDaylightSavingsDeviation    = GetDaylightSavingsDeviation,
    .SetDaylightSavingsDeviation    = SetDaylightSavingsDeviation,
    .GetDaylightSavingsEnabled      = GetDaylightSavingsEnabled,
    .SetDaylightSavingsEnabled      = SetDaylightSavingsEnabled,
    .GetClockBase                   = GetClockBase,
    .SetClockBase                   = SetClockBase,

    .AdjusttoQuarter                = AdjusttoQuarter,
    .AdjusttoMeasuringPeriod        = AdjusttoMeasuringPeriod,
    .AdjusttoMinute                 = AdjusttoMinute,
    .AdjusttoPresetTime             = AdjusttoPresetTime,
    .PresetAdjustingTime            = PresetAdjustingTime,
    .ShiftTime                      = ShiftTime,
};