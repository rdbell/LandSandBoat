#pragma once

namespace mobcontrollerillusiondetection
{
struct State
{
    bool hasInvisible;
    bool hasSneak;
};

// Apply returns the effective detection statuses after applying Illusion.
template <typename SeesThroughIllusion>
constexpr auto Apply(
    const bool hasIllusion,
    const bool hasInvisible,
    const bool hasSneak,
    SeesThroughIllusion&& seesThroughIllusion) -> State
{
    if (hasIllusion && !seesThroughIllusion())
    {
        return { true, true };
    }
    return { hasInvisible, hasSneak };
}
} // namespace mobcontrollerillusiondetection
