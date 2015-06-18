#pragma once

#include <chrono>

/* This header defines some shortcurts for types of high resolution clock in std::chrono */

// One second, minute and hour in nanoseconds. You can operate with them to get the desired time
// Example: ONE_SECOND * 5 is 5 seconds, ONE_HOUR / 2 is 30 minutes
#define ONE_MILLISECOND (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::milliseconds(1)))
#define ONE_SECOND (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(1)))
#define ONE_MINUTE (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::minutes(1)))
#define ONE_HOUR (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::hours(1)))

namespace xgsd {
    
    // std::chrono typedefs
    typedef std::chrono::time_point<std::chrono::high_resolution_clock> HiResTime;
    typedef std::chrono::nanoseconds HiResDuration;
    typedef std::chrono::high_resolution_clock HiResClock;
    
} // namespace xgsd
