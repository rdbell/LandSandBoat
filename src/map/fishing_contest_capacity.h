#pragma once

#include "common/cbasetypes.h"

// Pure fishing contest stage-due gate (slice 2846).
//
// Production host: fishingcontest::IsStageDue / ProgressContest in fishingcontest.cpp.
// Helpers take host-injected time scalars only (no contest globals or earth_time).

namespace fishingcontesthelpers
{

// IsStageDue mirrors ProgressContest's strict stage boundary:
//   currentTime > changeTime
// Equality is not due (not >=).
inline auto IsStageDue(const uint32 currentTime, const uint32 changeTime) -> bool
{
    return currentTime > changeTime;
}

} // namespace fishingcontesthelpers
