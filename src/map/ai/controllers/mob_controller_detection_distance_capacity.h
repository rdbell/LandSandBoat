#pragma once

namespace mobcontrollerdetectiondistance
{
// EffectiveDistance applies the target's stealth modifier to geometric distance.
constexpr auto EffectiveDistance(const float geometricDistance, const float stealthModifier) -> float
{
    return geometricDistance + stealthModifier;
}
} // namespace mobcontrollerdetectiondistance
