#pragma once

#include "common/cbasetypes.h"

// Pure Artisan Moogle helpers shared by dual-wire slices:
//   - 2879: CanBuySack (artisan.lua moogleOnUpdate option 1 gate)
//
// Lua production host: scripts/globals/artisan.lua moogleOnUpdate:
//
//   if option == 1 then -- Buy sack
//       if player:getGil() >= 9980
//          and player:getContainerSize(xi.inv.MOGSACK) == 0 then
//           player:delGil(9980)
//           player:changeContainerSize(xi.inv.MOGSACK, 30)
//           ...
//       end
//   end
//
// Host injects scalars only (no player / entity pointers):
//   gil      — player:getGil()
//   sackSize — player:getContainerSize(xi.inv.MOGSACK)
//
// delGil, changeContainerSize, setCharVar, and updateEvent remain host-owned.
// Prior pure port: OmegaXI slice 0948 (internal/artisan).

namespace artisanhelpers
{

// BuySackGilCost is the gil required to purchase a Mog Sack.
// Documented from artisan.lua option 1: getGil() >= 9980 / delGil(9980).
inline constexpr int32 BuySackGilCost = 9980;

// CanBuySack is the pure gate for option 1 (Buy sack):
//
//   gil >= BuySackGilCost && sackSize == 0
//
// Future Lua host injects scalars into this helper instead of re-inlining
// the gil / empty-sack comparison.
inline auto CanBuySack(const int32 gil, const int32 sackSize) -> bool
{
    return gil >= BuySackGilCost && sackSize == 0;
}

} // namespace artisanhelpers
