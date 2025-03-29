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
    // �� gmtime_r ӳ��Ϊ gmtime_s������������˳��
#define gmtime_r(timep, result) gmtime_s((result), (timep))
#endif

    // ��ȡ��ʵʱ��
    inline uint64_t getRealTimeSec()
    {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    }

    // ��ȡʱ��
    inline int32_t getTimeZone()
    {
        return -8 * 60 * 60;
    }

    // ʱ��� --> ����ʱ��
    inline void getLocalTime(struct tm& tv, time_t t)
    {
        t -= getTimeZone();
        gmtime_r(&t, &tv);
    }

    // ʱ��� --> UTC ʱ��
    inline void getGMTime(struct tm& tv, time_t t)
    {
        gmtime_r(&t, &tv);
    }

    // �Ӽ�Ԫ��ָ��ʱ��㾭���������(����ʱ��)
    inline int32_t localDaysFromEpoch(time_t t)
    {
        return static_cast<int32_t>(t - getTimeZone()) / kDaySeconds;
    }

    // �Ӽ�Ԫ��ָ��ʱ��㾭��������һ�������(����ʱ��)
    inline int32_t localWeeksFromEpoch(time_t t)
    {
        return static_cast<int32_t>(localDaysFromEpoch(t) + 3) / 7;
    }

    // ����ʱ��������������� (����ʱ��)
    inline uint32_t localDaysBetween(time_t t1, time_t t2)
    {
        uint32_t days1 = localDaysFromEpoch(t1);
        uint32_t days2 = localDaysFromEpoch(t2);
        return ((days1 > days2) ? (days1 - days2) : (days2 - days1));
    }

    // ����ʱ����������һ���� (����ʱ��)
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

    // ��ȡ���� 0 ������
    inline time_t getLocalDayZero(time_t t)
    {
        return localDaysFromEpoch(t) * kDaySeconds + getTimeZone();
    }

    // ��ȡ������һ 0 ������
    inline time_t getLocalMondayZero(time_t t)
    {
        struct tm tv;
        getLocalTime(tv, t);

        uint32_t days = tv.tm_wday ? tv.tm_wday : 7;
        return getLocalDayZero(t) - (days - 1) * kDaySeconds;
    }

    // ��ȡ������һ 0 ������
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
        strftime(time_str, 32, "%Y��-%m��-%d�� %H:%M:%S", &tv);
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
