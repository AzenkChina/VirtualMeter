/*******************
 *
 * Copyright 1998-2010 IAR Systems AB. 
 *
 * This is the default implementation of the "time" function of the
 * standard library.  It can be replaced with a system-specific
 * implementation.
 *
 * The "time" function returns the current calendar time.  (time_t)-1
 * should be returned if the calendar time is not available.  The time
 * is measured in seconds since the first of January 1970.
 *
 ********************/
#if defined ( __ICCARM__ )

#include <time.h>
#include "rtc.h"

#pragma module_name = "?time"

#if _DLIB_TIME_ALLOW_64
__time64_t (__time64)(__time64_t *t)
{
    if (t)
    {
        *t = (__time64_t) -1;
        return(__time64_t) -1;
    }
    else
    {
        return((__time64_t)rtc.read());
    }
}
#endif

#endif