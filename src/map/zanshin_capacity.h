#pragma once

#include "common/cbasetypes.h"

#include <algorithm>

namespace zanshinhelpers
{

constexpr uint8 ResolutionHit  = 0;
constexpr uint8 ReactionCounter = 63;

inline auto ShouldEvaluate(const bool firstSwing, const uint8 remainingSwingCount) -> bool
{
    return firstSwing && remainingSwingCount == 1;
}

// The host assigns the signed sum to uint16 before clamping; preserve that
// conversion, including wrap for negative values.
inline auto ResolveChance(const int16 zanshinMod, const int32 meritValue) -> uint16
{
    const auto converted = static_cast<uint16>(static_cast<int32>(zanshinMod) + meritValue);
    return std::clamp<uint16>(converted, 0, 100);
}

struct ProcResult
{
    bool normal;
    bool hasso;

    constexpr auto addSwing() const -> bool
    {
        return normal || hasso;
    }

    constexpr auto operator==(const ProcResult&) const -> bool = default;
};

template <typename HasHasso, typename Roll>
inline auto ResolveProcs(
    const uint8 resolution,
    const uint8 reaction,
    const uint16 chance,
    const bool hasHassoZanshinBonus,
    HasHasso&& hasHasso,
    Roll&& roll) -> ProcResult
{
    const bool missedOrCountered = resolution != ResolutionHit || reaction == ReactionCounter;
    const bool normalProc        = missedOrCountered && roll() < chance;
    const bool samWithHasso      = hasHassoZanshinBonus && hasHasso();
    const bool hassoProc         = samWithHasso && roll() < chance / 4;
    return { normalProc, hassoProc };
}

} // namespace zanshinhelpers
