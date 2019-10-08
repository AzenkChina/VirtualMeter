/*******************
 *
 * Copyright 2003-2010 IAR Systems AB.  
 *
 * This is the default implementation of the __getzone. It returns
 * the current time-zone, i.e. UTC.
 *
 *
 *
 *
 * The return value should be a string on the following form:
 *
 * :[XXX[:YYY[:NNN[:DST[:DST ...]]]]]
 *
 * Where XXX is the standard time-zone name, YYY is the daylight
 * savings time-zone name, NNN is the time zone offset, and the DSTs
 * are the daylight savings time rules. Daylight savings time will add
 * one hour to the normal time. (The names are only used in the 'Z'
 * formatter in the strftime library function.)
 *
 * The time zone offset NNN is specified as a number relative to UTC,
 * possibly negative (east is positive), on the format HHMM, where HH
 * is hours and MM is minutes.
 *
 * The DSTs specifes a set of rules for how daylight savings time is
 * applied. The rules must be sorted in increasing date order starting
 * from the earliest date. The first rule for a specific year will
 * enable DST, the next will disable it, and so on. Each rule is on
 * the following form:
 *
 *   [(YYYY)]MMDD[HH][-W|+W]
 *
 *    (YYYY) is the first year the daylight savings rule was applied.
 *       It is optional. If not specified it will default to the same
 *       year as the previous rule or zero if no previous rule.
 *    MM is the month number (1-12).  
 *    DD is the day of the month (1-31).  
 *    HH is the hour number in a 24-hour day (optional, defaults to
 *       0).  
 *    +/-W specifies the day of the week the rule takes effect (where
 *       Sunday = 0, Monday = 1, etc). +W means that the rule applies
 *       to the first such day on or after the specified date and -W
 *       strictly before the date. If this is not specified, the rule
 *       will take effect on the exact date, regardless of the day of
 *       the week.
 *
 * On the northern hemisphere the DST rules normally comes in pairs, a
 * start, Aprilish, and an end, Octoberish. On the southern hemisphere
 * one normally has to use three rules: enabling DST from start of
 * year, disabling it in Aprilish, and then enabling it again in
 * Octoberish.
 *
 * Example:
 *
 *   :GMT:GMT+1:0060:(1990)040102-0:100102-0
 *
 *   Here, the time zone is GMT and under daylight savings time the
 *   time zone is named GMT+1. The time zone offset is 0060, i.e. 60
 *   minutes from UTC. As of the year 1990, daylight savings time
 *   started on the Sunday before (but not on) 1:st of April at 2am
 *   and ends on the first Sunday before (but not on) the first of
 *   October.
 *
 *   :GMT+10:GMT+11:0900:(1990)010100-0:040102-0:100102-0
 *
 *   Tasmania is on UTC+10 hours, with daylight savings time from
 *   first Sunday in October until first Sunday in April. Note, the
 *   first DST rule is for enabling from start of the year.
 *
 ********************/

#include <yfuns.h>

_STD_BEGIN

char const * __getzone()
{
  return ":GMT+8:GMT+8:0800";
}

_STD_END
