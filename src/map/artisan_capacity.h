#pragma once

#include "common/cbasetypes.h"

// Pure Artisan Moogle helpers shared by dual-wire slices:
//   - 2879: CanBuySack (artisan.lua moogleOnUpdate option 1 gate)
//   - 2890: CanExpand (artisan.lua moogleOnUpdate option 2 expand gate)
//   - 2912: GobbieCanUpgradeFlag (option 2 expand-failure event param)
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
//   elseif option == 2 then -- Expand sack
//       local sackSize = player:getContainerSize(xi.inv.MOGSACK)
//       local gobbieSize = player:getContainerSize(xi.inv.INVENTORY)
//       local gobbieCanUpgrade = gobbieSize < 80 and 1 or 0
//       if sackSize < gobbieSize and sackSize > 0 then
//           player:changeContainerSize(xi.inv.MOGSACK, gobbieSize - sackSize)
//           ...
//       else
//           player:updateEvent(0, 0, 0, 0, 0, 0, gobbieCanUpgrade, 0)
//       end
//   end
//
// Host injects scalars only (no player / entity pointers):
//   gil        — player:getGil()
//   sackSize   — player:getContainerSize(xi.inv.MOGSACK)
//   gobbieSize — player:getContainerSize(xi.inv.INVENTORY)
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

// CanExpand is the pure gate for option 2 (Expand sack):
//
//   sackSize < gobbieSize && sackSize > 0
//
// Future Lua host injects scalars into this helper instead of re-inlining
// the sack / inventory size comparison. changeContainerSize and updateEvent
// remain host-owned after a true gate.
inline auto CanExpand(const int32 sackSize, const int32 gobbieSize) -> bool
{
    return sackSize < gobbieSize && sackSize > 0;
}

// GobbieUpgradeCap is the inventory size threshold for gobbieCanUpgrade
// (artisan.lua: gobbieSize < 80 and 1 or 0).
inline constexpr int32 GobbieUpgradeCap = 80;

// GobbieCanUpgradeFlag is the pure gobbieCanUpgrade event param when expand
// fails (option 2 else branch):
//
//   gobbieSize < GobbieUpgradeCap ? 1 : 0
//
// Future Lua host injects gobbieSize into this helper instead of re-inlining
// the cap comparison. updateEvent remains host-owned.
inline auto GobbieCanUpgradeFlag(const int32 gobbieSize) -> int32
{
    return gobbieSize < GobbieUpgradeCap ? 1 : 0;
}

} // namespace artisanhelpers
