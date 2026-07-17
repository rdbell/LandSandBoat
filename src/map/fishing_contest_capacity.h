#pragma once

#include "common/cbasetypes.h"

// Pure fishing contest helpers (slices 2846, 2851).
//
// Production hosts: fishingcontest::IsStageDue / ProgressContest,
// fishingcontest::ScoreFish (future Lua host bridge) in fishingcontest.cpp.
// Helpers take host-injected scalars only (no contest globals, earth_time, or Lua).

namespace fishingcontesthelpers
{

// IsStageDue mirrors ProgressContest's strict stage boundary:
//   currentTime > changeTime
// Equality is not due (not >=).
inline auto IsStageDue(const uint32 currentTime, const uint32 changeTime) -> bool
{
    return currentTime > changeTime;
}

// ScoreFish mirrors Lua local scoreFish (scripts/globals/fishing_contest.lua):
//   SIZE   (0) → length
//   WEIGHT (1) → weight
//   else       → length + weight   (BOTH and any other criteria value)
// criteria is FISHING_CONTEST_CRITERIA as uint8 (SIZE=0, WEIGHT=1, BOTH=2).
inline auto ScoreFish(const uint32 length, const uint32 weight, const uint8 criteria) -> uint32
{
    if (criteria == 0) // SIZE
    {
        return length;
    }
    if (criteria == 1) // WEIGHT
    {
        return weight;
    }
    return length + weight;
}

} // namespace fishingcontesthelpers
