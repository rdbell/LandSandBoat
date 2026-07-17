#pragma once

#include "common/cbasetypes.h"

// Pure Artisan Moogle helpers shared by dual-wire slices:
//   - 2879: CanBuySack residual dual-wire suite (buy_sack)
//   - 3090: CanBuySack dedicated dual-wire (buy_sack.go)
//   - 2890: CanExpand residual dual-wire suite (can_expand)
//   - 3106: CanExpand dedicated dual-wire (can_expand.go)
//   - 2912: GobbieCanUpgradeFlag (option 2 expand-failure event param)
//   - 2916: CanClaimScroll residual dual-wire suite (claim_scroll)
//   - 3132: CanClaimScroll dedicated dual-wire (claim_scroll.go)
//
// Dual-wire index:
//   - 2879: CanBuySack residual dual-wire suite
//   - 3090: CanBuySack = gil >= BuySackGilCost && sackSize == 0
//   - 2890: CanExpand residual dual-wire suite
//   - 3106: CanExpand = sackSize < gobbieSize && sackSize > 0
//   - 2912: GobbieCanUpgradeFlag
//   - 2916: CanClaimScroll residual dual-wire suite
//   - 3132: CanClaimScroll = nextScroll < jstMidnight
//
// Lua production host: scripts/globals/artisan.lua moogleOnUpdate /
// moogleOnFinish:
// Go dual-wire: artisan.CanBuySack / artisan.BuySackGilCost
// (internal/artisan/buy_sack.go); artisan.CanExpand
// (internal/artisan/can_expand.go); artisan.CanClaimScroll
// (internal/artisan/claim_scroll.go). Future Lua host injects free functions
// then delGil / changeContainerSize / setCharVar / updateEvent / giveItem.
//
// Prior pure port: OmegaXI slice 0948 (internal/artisan).
// Residual dual-wire suite: 2879 / 2890 / 2916.
// Dedicated dual-wire suite: 3090 / 3106 / 3132.
//
//   if option == 1 then -- Buy sack (2879 residual / 3090 dedicated)
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
//   -- moogleOnFinish option 99 Get Scroll (2916 residual / 3132 dedicated):
//   if option == 99 then
//       if player:getCharVar('[artisan]nextScroll') < JstMidnight() then
//           if npcUtil.giveItem(player, xi.item.SCROLL_OF_INSTANT_WARP) then
//               player:setCharVar('[artisan]nextScroll', JstMidnight())
//           end
//       end
//   end
//
// Host injects scalars only (no player / entity pointers):
//   gil         — player:getGil()
//   sackSize    — player:getContainerSize(xi.inv.MOGSACK)
//   gobbieSize  — player:getContainerSize(xi.inv.INVENTORY)
//   nextScroll  — player:getCharVar('[artisan]nextScroll')
//   jstMidnight — JstMidnight()
//
// delGil, changeContainerSize, setCharVar, giveItem, and updateEvent remain
// host-owned. Prior pure port: OmegaXI slice 0948 (internal/artisan).

namespace artisanhelpers
{

// ---------------------------------------------------------------------------
// Slice 2879 / 3090 — moogleOnUpdate option 1 buy-sack gate
// ---------------------------------------------------------------------------

// BuySackGilCost is the gil required to purchase a Mog Sack.
// Documented from artisan.lua option 1: getGil() >= 9980 / delGil(9980).
// Prior pure port: slice 0948. Residual dual-wire suite: 2879.
// Dedicated dual-wire suite: 3090.
inline constexpr int32 BuySackGilCost = 9980;

// CanBuySack is the pure gate for option 1 (Buy sack):
//
// Formula (slice 3090 dedicated dual-wire; residual expand 2879 / pure 0948 —
// formula unchanged):
//   CanBuySack(gil, sackSize) = gil >= BuySackGilCost && sackSize == 0
//
// Future Lua host injects scalars into this helper instead of re-inlining
// the gil / empty-sack comparison. Dual-wire of Go artisan.CanBuySack
// (buy_sack.go). Call site: future Lua moogleOnUpdate option 1 inject.
// Prior pure port: slice 0948. Residual dual-wire suite: 2879 /
// test_artisan_buy_sack_2879. Dedicated dual-wire suite is
// test_artisan_can_buy_sack_3090. Host still owns delGil, changeContainerSize,
// setCharVar, and updateEvent after a true gate.
inline auto CanBuySack(const int32 gil, const int32 sackSize) -> bool
{
    return gil >= BuySackGilCost && sackSize == 0;
}

// ---------------------------------------------------------------------------
// Slice 2890 / 3106 — moogleOnUpdate option 2 expand-sack gate
// ---------------------------------------------------------------------------

// CanExpand is the pure gate for option 2 (Expand sack):
//
// Formula (slice 3106 dedicated dual-wire; residual expand 2890 / pure 0948 —
// formula unchanged):
//   CanExpand(sackSize, gobbieSize) = sackSize < gobbieSize && sackSize > 0
//
// Future Lua host injects scalars into this helper instead of re-inlining
// the sack / inventory size comparison. Dual-wire of Go artisan.CanExpand
// (can_expand.go). Call site: future Lua moogleOnUpdate option 2 inject.
// Prior pure port: slice 0948. Residual dual-wire suite: 2890 /
// test_artisan_can_expand_2890. Dedicated dual-wire suite is
// test_artisan_can_expand_3106. Host still owns changeContainerSize,
// updateEvent, and the gobbieCanUpgrade failure param after a true/false gate.
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

// ---------------------------------------------------------------------------
// Slice 2916 / 3132 — moogleOnFinish option 99 Get Scroll gate
// ---------------------------------------------------------------------------

// CanClaimScroll is the pure gate for option 99 (Get Scroll):
//
// Formula (slice 3132 dedicated dual-wire; residual expand 2916 / pure 0948 —
// formula unchanged):
//   CanClaimScroll(nextScroll, jstMidnight) = nextScroll < jstMidnight
//
// nextScroll is getCharVar('[artisan]nextScroll'); jstMidnight is JstMidnight().
// Future Lua host injects both timestamps into this helper instead of
// re-inlining the comparison. Dual-wire of Go artisan.CanClaimScroll
// (claim_scroll.go). Call site: future Lua moogleOnFinish option 99 inject.
// Prior pure port: slice 0948. Residual dual-wire suite: 2916 /
// test_artisan_claim_scroll_2916. Dedicated dual-wire suite is
// test_artisan_can_claim_scroll_3132. giveItem(SCROLL_OF_INSTANT_WARP) and
// setCharVar('[artisan]nextScroll', JstMidnight()) remain host-owned.
inline auto CanClaimScroll(const int64 nextScroll, const int64 jstMidnight) -> bool
{
    return nextScroll < jstMidnight;
}

} // namespace artisanhelpers
