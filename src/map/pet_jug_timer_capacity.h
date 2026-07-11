#pragma once

namespace petjugtimerhelpers
{

template <typename TimePoint, typename Warn>
inline auto GetSpawnTime(const bool isJug, const TimePoint& stored, Warn&& warn) -> TimePoint
{
    if (!isJug)
    {
        warn();
    }
    return stored;
}

template <typename TimePoint, typename Warn, typename Store>
inline void SetSpawnTime(const bool isJug, const TimePoint& value, Warn&& warn, Store&& store)
{
    if (!isJug)
    {
        warn();
        return;
    }
    store(value);
}

template <typename Duration, typename Warn>
inline auto GetDuration(const bool isJug, const Duration& stored, Warn&& warn) -> Duration
{
    if (!isJug)
    {
        warn();
        return Duration{};
    }
    return stored;
}

template <typename Duration, typename Warn, typename Store>
inline void SetDuration(const bool isJug, const Duration& value, Warn&& warn, Store&& store)
{
    if (!isJug)
    {
        warn();
        return;
    }
    store(value);
}

} // namespace petjugtimerhelpers
