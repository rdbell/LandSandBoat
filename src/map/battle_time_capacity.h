#pragma once

namespace battletimehelpers
{

template <typename TimePoint>
inline auto Elapsed(const TimePoint now, const TimePoint start) -> decltype(now - start)
{
    return now - start;
}

} // namespace battletimehelpers
