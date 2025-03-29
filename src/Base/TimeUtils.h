#pragma once

#include <iostream>
#include <time.h>
#include <chrono>

namespace TimeUtils
{
    const static uint32_t kMinuteSeconds = 60;
    const static uint32_t kHourSeconds = 3600;
    const static uint32_t kDaySeconds = 24 * kHourSeconds;

#ifdef _WIN32
    // 将 gmtime_r 映射为 gmtime_s，并调整参数顺序
#define gmtime_r(timep, result) gmtime_s((result), (timep))
#endif

    // 获取真实时间
    inline uint64_t getRealTimeSec()
    {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    }

    // 获取时区
    inline int32_t getTimeZone()
    {
        return -8 * 60 * 60;
    }

    // 时间点 --> 本地时间
    inline void getLocalTime(struct tm& tv, time_t t)
    {
        t -= getTimeZone();
        gmtime_r(&t, &tv);
    }

    // 时间点 --> UTC 时间
    inline void getGMTime(struct tm& tv, time_t t)
    {
        gmtime_r(&t, &tv);
    }

    // 从纪元到指定时间点经历过的零点(本地时间)
    inline int32_t localDaysFromEpoch(time_t t)
    {
        return static_cast<int32_t>(t - getTimeZone()) / kDaySeconds;
    }

    // 从纪元到指定时间点经历过的周一零点数量(本地时间)
    inline int32_t localWeeksFromEpoch(time_t t)
    {
        return static_cast<int32_t>(localDaysFromEpoch(t) + 3) / 7;
    }

    // 两个时间点相隔的零点数量 (本地时间)
    inline uint32_t localDaysBetween(time_t t1, time_t t2)
    {
        uint32_t days1 = localDaysFromEpoch(t1);
        uint32_t days2 = localDaysFromEpoch(t2);
        return ((days1 > days2) ? (days1 - days2) : (days2 - days1));
    }

    // 两个时间点相隔的周一数量 (本地时间)
    inline uint32_t localWeeksBetween(time_t t1, time_t t2)
    {
        uint32_t days1 = localWeeksFromEpoch(t1);
        uint32_t days2 = localWeeksFromEpoch(t2);
        return ((days1 > days2) ? (days1 - days2) : (days2 - days1));
    }

    inline bool isSameLocalHour(time_t t1, time_t t2)
    {
        struct tm tv1;
        struct tm tv2;
        getLocalTime(tv1, t1);
        getLocalTime(tv2, t2);

        return (tv1.tm_year == tv2.tm_year && tv1.tm_mon == tv2.tm_mon && tv1.tm_mday == tv2.tm_mday
                && tv1.tm_hour == tv2.tm_hour);
    }

    inline bool isSameLocalDay(time_t t1, time_t t2)
    {
        return localDaysFromEpoch(t1) == localDaysFromEpoch(t2);
    }

    inline bool isSameLocalWeek(time_t t1, time_t t2)
    {
        return localWeeksFromEpoch(t1) == localWeeksFromEpoch(t2);
    }

    inline bool isSameLocalMonth(time_t t1, time_t t2)
    {
        struct tm tv1;
        struct tm tv2;
        getLocalTime(tv1, t1);
        getLocalTime(tv2, t2);

        return (tv1.tm_year == tv2.tm_year && tv1.tm_mon == tv2.tm_mon);
    }

    inline bool isSameLocalYear(time_t t1, time_t t2)
    {
        struct tm tv1;
        struct tm tv2;
        getLocalTime(tv1, t1);
        getLocalTime(tv2, t2);

        return (tv1.tm_year == tv2.tm_year);
    }

    inline uint16_t getLocalHour(time_t t)
    {
        struct tm tv;
        getLocalTime(tv, t);
        return tv.tm_hour;
    }

    inline uint64_t getLocalHourSeconds(time_t t)
    {
        struct tm tv;
        getLocalTime(tv, t);
        return tv.tm_hour * kHourSeconds;
    }

    // 获取今日 0 点秒数
    inline time_t getLocalDayZero(time_t t)
    {
        return localDaysFromEpoch(t) * kDaySeconds + getTimeZone();
    }

    // 获取本周周一 0 点秒数
    inline time_t getLocalMondayZero(time_t t)
    {
        struct tm tv;
        getLocalTime(tv, t);

        uint32_t days = tv.tm_wday ? tv.tm_wday : 7;
        return getLocalDayZero(t) - (days - 1) * kDaySeconds;
    }

    // 获取下周周一 0 点秒数
    inline time_t getLocalNextMondayZero(time_t t)
    {
        return getLocalMondayZero(t) + 7 * kDaySeconds;
    }

    // retrun [0,1,2,3,4,5,6]
    inline uint8_t getLocalWeekday()
    {
        struct tm tv;
        getLocalTime(tv, getRealTimeSec());
        return tv.tm_wday;
    }

    inline uint32_t getDaySeconds()
    {
        struct tm tv;
        getLocalTime(tv, getRealTimeSec());
        return tv.tm_hour * kHourSeconds + tv.tm_min * kMinuteSeconds + tv.tm_sec;
    }

    static char* timeToStr(const time_t& t)
    {
        static char time_str[32] = {};
        struct tm   tv;
        getLocalTime(tv, t);
        strftime(time_str, 32, "%Y-%m-%d %H:%M:%S", &tv);
        return time_str;
    }

    static char* timeToCHNStr(const time_t& t)
    {
        static char time_str[32] = {};
        struct tm   tv;
        getLocalTime(tv, t);
        strftime(time_str, 32, "%Y年-%m月-%d日 %H:%M:%S", &tv);
        return time_str;
    }

    static char* timeToHMS(const time_t& t)
    {
        static char time_str[32] = {};
        struct tm   tv;
        getLocalTime(tv, t);
        strftime(time_str, 32, "%H:%M:%S", &tv);
        return time_str;
    }
}  // namespace TimeUtils
