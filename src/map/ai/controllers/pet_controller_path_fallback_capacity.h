#pragma once

namespace petcontrollerpathfallback
{
inline auto ShouldWarp(bool pathAroundSucceeded, bool pathInRangeSucceeded) -> bool
{
    return !pathAroundSucceeded && !pathInRangeSucceeded;
}
} // namespace petcontrollerpathfallback
