#pragma once

#include "common/cbasetypes.h"

// Pure gardenutils result-map key packing dual-wired from gardenutils.cpp:
// - detail::AppendResult / LoadResultList row insert (slice 2838)
// - CalculateResults resultUid lookup (slice 2838)
//
// Helpers take host-injected seed + crystal-feed element IDs only (no DB,
// flowerpot, or map storage).

namespace gardenutilshelpers
{

// ResultKey packs plant seed + common/extra crystal feeds into the
// gardening_results map key used by LoadResultList and CalculateResults:
//
//   uid = (seed << 8) + (element1 << 4) + element2
//
// seed is FLOWERPOT_PLANT_TYPE; element1/element2 are FLOWERPOT_ELEMENT_TYPE.
inline auto ResultKey(const uint8 seed, const uint8 element1, const uint8 element2) -> uint32
{
    return (seed << 8) + (element1 << 4) + element2;
}

} // namespace gardenutilshelpers
