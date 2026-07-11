#pragma once

namespace charplaytimehelpers
{

template <typename Duration, typename TimePoint, typename Now>
inline void Set(const Duration playTime, Duration& storedPlayTime, TimePoint& saveTime, Now&& now)
{
    storedPlayTime = playTime;
    saveTime       = now();
}

template <typename Duration, typename TimePoint, typename Now>
inline auto Get(const bool needUpdate, Duration& storedPlayTime, TimePoint& saveTime, Now&& now) -> Duration
{
    if (needUpdate)
    {
        const auto currentTime = now();
        storedPlayTime += currentTime - saveTime;
        saveTime = currentTime;
    }
    return storedPlayTime;
}

} // namespace charplaytimehelpers
