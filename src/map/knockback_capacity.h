#pragma once

#include <algorithm>
#include <cstdint>

// Pure xi.combat.knockback.calculate.
// Parity: internal/knockback (slice 0911); production wire slice 1581.

namespace knockbackhelpers
{

constexpr int LevelNone = 0;
constexpr int Level7    = 7;

// clamp(skillKnockback - reductionMod, NONE, LEVEL7)
constexpr auto Calculate(const int skillKnockback, const int knockbackReductionMod) -> int
{
    const int v = skillKnockback - knockbackReductionMod;
    if (v < LevelNone)
    {
        return LevelNone;
    }
    if (v > Level7)
    {
        return Level7;
    }
    return v;
}

} // namespace knockbackhelpers
