#pragma once

#include "common/cbasetypes.h"

// Pure chocobo-raising helpers (slice 2862).
//
// Production host is Lua scripts/globals/hobbies/chocobo_raising/constants.lua
// (xi.chocoboRaising.numberToRank / affectionToAffectionRank / packStats1 /
// getPlayerRidingSpeedAndTime). Capacity is for future Lua/C++ inject so hosts
// dual-wire the pure intended upper-bound mapping instead of re-inlining band
// comparisons (or relying on the stock Lua ipairs no-op over string keys).
//
// Helpers take host-injected scalars only (no player / chocoState entity).

namespace chocoboraisinghelpers
{

// Inclusive upper edges for skill ranks 0..7 (F..SS).
// Mirrors xi.chocoboRaising.skillRankBoundaries / Go SkillRankBoundaries.
//
//   0–31 F, 32–63 E, 64–95 D, 96–127 C,
//   128–159 B, 160–191 A, 192–223 S, 224–255 SS
inline constexpr int32 kSkillRankBoundaries[8] = {
    31,  // F_POOR
    63,  // E_SUBSTANDARD
    95,  // D_A_BIT_DEFICIENT
    127, // C_AVERAGE
    159, // B_BETTER_THAN_AVERAGE
    191, // A_IMPRESSIVE
    223, // S_OUTSTANDING
    255, // SS_FIRST_CLASS
};

// NumberToRank maps a raw skill (or affection) value to rank 0..7 via the
// intended upper-bound bands.
//
// Negative skill is treated as F (0). Values above 255 clamp to SS (7).
//
// Dual-wire of Go chocoboraising.NumberToRank (internal/chocoboraising).
// Note: stock Lua numberToRank uses ipairs on a string-keyed table and always
// returns F_POOR; this capacity implements the boundary-documented mapping.
inline auto NumberToRank(const int32 skill) -> uint8
{
    if (skill < 0)
    {
        return 0; // F_POOR
    }
    for (uint8 rank = 0; rank < 8; ++rank)
    {
        if (skill <= kSkillRankBoundaries[rank])
        {
            return rank;
        }
    }
    return 7; // SS_FIRST_CLASS
}

// AffectionToAffectionRank reuses the skill-rank upper bounds onto affection
// rank 0..7. Dual-wire compose: NumberToRank(affection).
inline auto AffectionToAffectionRank(const int32 affection) -> uint8
{
    return NumberToRank(affection);
}

} // namespace chocoboraisinghelpers
