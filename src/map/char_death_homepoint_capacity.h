#pragma once

#include <functional>
#include <utility>

namespace chardeathhomepointhelpers
{

template <typename ClearExperienceLoss, typename RequestWarp>
inline void Expire(ClearExperienceLoss&& clearExperienceLoss, RequestWarp&& requestWarp)
{
    std::invoke(std::forward<ClearExperienceLoss>(clearExperienceLoss));
    std::invoke(std::forward<RequestWarp>(requestWarp));
}

template <typename TimePoint>
inline void SetDeathTime(TimePoint& deathTime, const TimePoint timestamp)
{
    deathTime = timestamp;
}

template <typename TimePoint, typename Now, typename Duration>
inline Duration TimeSinceDeath(
    const TimePoint deathTime,
    const TimePoint minimumTime,
    Now&&           now,
    const Duration  zero)
{
    if (deathTime > minimumTime)
    {
        return std::invoke(std::forward<Now>(now)) - deathTime;
    }
    return zero;
}

template <typename Duration, typename TimeSinceDeath>
inline Duration TimeUntilDeathHomepoint(
    const Duration homepointDuration,
    TimeSinceDeath&& timeSinceDeath)
{
    return homepointDuration - std::invoke(std::forward<TimeSinceDeath>(timeSinceDeath));
}

} // namespace chardeathhomepointhelpers
