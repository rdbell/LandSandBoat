#pragma once

#include "common/cbasetypes.h"

// Pure Artisan Moogle helpers shared by dual-wire slices:
//   - 2879: CanBuySack residual dual-wire suite (buy_sack)
//   - 3090: CanBuySack prior dedicated dual-wire (buy_sack.go)
//   - 3242: CanBuySack prior dedicated dual-wire expand residual 2879
//           (prior dedicated 3090; formula unchanged)
//   - 3282: CanBuySack prior dedicated dual-wire expand residual 2879
//           (prior dedicated 3242 / 3090; formula unchanged)
//   - 3312: CanBuySack prior dedicated dual-wire expand residual 2879
//           (prior dedicated 3282 / 3242 / 3090; formula unchanged)
//   - 3493: CanBuySack prior dedicated dual-wire expand residual 2879
//           (prior dedicated 3312 / 3282 / 3242 / 3090; formula unchanged)
//   - 3550: CanBuySack prior dedicated dual-wire expand residual 2879
//           (prior dedicated 3493 / 3312 / 3282 / 3242 / 3090; formula unchanged)
//   - 3595: CanBuySack prior dedicated dual-wire expand residual 2879
//           (prior dedicated 3550 / 3493 / 3312 / 3282 / 3242 / 3090; formula unchanged)
//   - 3640: CanBuySack prior dedicated dual-wire expand residual 2879
//           (prior dedicated 3595 / 3550 / 3493 / 3312 / 3282 / 3242 / 3090; formula unchanged)
//   - 3685: CanBuySack prior dedicated dual-wire expand residual 2879
//           (prior dedicated 3640 / 3595 / 3550 / 3493 / 3312 / 3282 / 3242 / 3090; formula unchanged)
//   - 3730: CanBuySack prior dedicated dual-wire expand residual 2879
//           (prior dedicated 3685 / 3640 / 3595 / 3550 / 3493 / 3312 / 3282 / 3242 / 3090; formula unchanged)
//   - 3775: CanBuySack prior dedicated dual-wire expand residual 2879
//           (prior dedicated 3730 / 3685 / 3640 / 3595 / 3550 / 3493 / 3312 / 3282 / 3242 / 3090; formula unchanged)
//   - 3820: CanBuySack prior dedicated dual-wire expand residual 2879
//           (prior dedicated 3775 / 3730 / 3685 / 3640 / 3595 / 3550 / 3493 / 3312 / 3282 / 3242 / 3090; formula unchanged)
//   - 3865: CanBuySack dedicated dual-wire expand residual 2879
//           (prior dedicated 3820 / 3775 / 3730 / 3685 / 3640 / 3595 / 3550 / 3493 / 3312 / 3282 / 3242 / 3090; formula unchanged)
//   - 2890: CanExpand residual dual-wire suite (can_expand)
//   - 3106: CanExpand prior dedicated dual-wire (can_expand.go)
//   - 3363: CanExpand dedicated dual-wire expand residual 2890
//           (prior dedicated 3106; formula unchanged)
//   - 2912: GobbieCanUpgradeFlag residual dual-wire suite (gobbie_upgrade)
//   - 3147: GobbieCanUpgradeFlag dedicated dual-wire (gobbie_upgrade.go)
//   - 2916: CanClaimScroll residual dual-wire suite (claim_scroll)
//   - 3132: CanClaimScroll prior dedicated dual-wire (claim_scroll.go)
//   - 3396: CanClaimScroll prior dedicated dual-wire expand residual 2916
//           (prior dedicated 3132; formula unchanged)
//   - 3444: CanClaimScroll dedicated dual-wire expand residual 2916
//           (prior dedicated 3396 / 3132; formula unchanged)
//
// Dual-wire index:
//   - 2879: CanBuySack residual dual-wire suite
//   - 3090: CanBuySack prior dedicated dual-wire
//   - 3242: CanBuySack prior dedicated dual-wire expand residual 2879
//     (prior dedicated 3090)
//   - 3282: CanBuySack prior dedicated dual-wire expand residual 2879
//     (prior dedicated 3242 / 3090)
//   - 3312: CanBuySack prior dedicated dual-wire expand residual 2879
//     (prior dedicated 3282 / 3242 / 3090)
//   - 3493: CanBuySack prior dedicated dual-wire expand residual 2879
//     (prior dedicated 3312 / 3282 / 3242 / 3090)
//   - 3550: CanBuySack = gil >= BuySackGilCost && sackSize == 0
//     prior dedicated dual-wire expand residual 2879 (prior dedicated 3493 / 3312 / 3282 / 3242 / 3090)
//   - 3595: CanBuySack = gil >= BuySackGilCost && sackSize == 0
//     prior dedicated dual-wire expand residual 2879 (prior dedicated 3550 / 3493 / 3312 / 3282 / 3242 / 3090)
//   - 3640: CanBuySack = gil >= BuySackGilCost && sackSize == 0
//     prior dedicated dual-wire expand residual 2879 (prior dedicated 3595 / 3550 / 3493 / 3312 / 3282 / 3242 / 3090)
//   - 3685: CanBuySack = gil >= BuySackGilCost && sackSize == 0
//     prior dedicated dual-wire expand residual 2879 (prior dedicated 3640 / 3595 / 3550 / 3493 / 3312 / 3282 / 3242 / 3090)
//   - 3730: CanBuySack = gil >= BuySackGilCost && sackSize == 0
//     prior dedicated dual-wire expand residual 2879 (prior dedicated 3685 / 3640 / 3595 / 3550 / 3493 / 3312 / 3282 / 3242 / 3090)
//   - 3775: CanBuySack = gil >= BuySackGilCost && sackSize == 0
//     prior dedicated dual-wire expand residual 2879 (prior dedicated 3730 / 3685 / 3640 / 3595 / 3550 / 3493 / 3312 / 3282 / 3242 / 3090)
//   - 3820: CanBuySack = gil >= BuySackGilCost && sackSize == 0
//     prior dedicated dual-wire expand residual 2879 (prior dedicated 3775 / 3730 / 3685 / 3640 / 3595 / 3550 / 3493 / 3312 / 3282 / 3242 / 3090)
//   - 3865: CanBuySack = gil >= BuySackGilCost && sackSize == 0
//     dedicated dual-wire expand residual 2879 (prior dedicated 3820 / 3775 / 3730 / 3685 / 3640 / 3595 / 3550 / 3493 / 3312 / 3282 / 3242 / 3090)
//   - 2890: CanExpand residual dual-wire suite
//   - 3106: CanExpand prior dedicated dual-wire
//   - 3363: CanExpand = sackSize < gobbieSize && sackSize > 0
//     dedicated dual-wire expand residual 2890 (prior dedicated 3106)
//   - 2912: GobbieCanUpgradeFlag residual dual-wire suite
//   - 3147: GobbieCanUpgradeFlag = gobbieSize < GobbieUpgradeCap ? 1 : 0
//   - 2916: CanClaimScroll residual dual-wire suite
//   - 3132: CanClaimScroll prior dedicated dual-wire
//   - 3396: CanClaimScroll = nextScroll < jstMidnight
//     prior dedicated dual-wire expand residual 2916 (prior dedicated 3132)
//   - 3444: CanClaimScroll = nextScroll < jstMidnight
//     dedicated dual-wire expand residual 2916 (prior dedicated 3396 / 3132)
//
// Lua production host: scripts/globals/artisan.lua moogleOnUpdate /
// moogleOnFinish:
// Go dual-wire: artisan.CanBuySack / artisan.BuySackGilCost
// (internal/artisan/buy_sack.go); artisan.CanExpand
// (internal/artisan/can_expand.go); artisan.GobbieCanUpgradeFlag /
// artisan.GobbieUpgradeCap (internal/artisan/gobbie_upgrade.go);
// artisan.CanClaimScroll (internal/artisan/claim_scroll.go). Future Lua host
// injects free functions then delGil / changeContainerSize / setCharVar /
// updateEvent / giveItem.
//
// Prior pure port: OmegaXI slice 0948 (internal/artisan).
// Residual dual-wire suite: 2879 / 2890 / 2912 / 2916.
// Prior dedicated dual-wire suite: 3090 / 3106 / 3132 / 3147.
// Prior dedicated dual-wire expand residual: 3242 (CanBuySack residual 2879).
// Prior dedicated dual-wire expand residual: 3282 (CanBuySack residual 2879;
// prior dedicated 3242).
// Prior dedicated dual-wire expand residual: 3312 (CanBuySack residual 2879;
// prior dedicated 3282).
// Prior dedicated dual-wire expand residual: 3493 (CanBuySack residual 2879;
// prior dedicated 3312).
// Prior dedicated dual-wire expand residual: 3550 (CanBuySack residual 2879;
// prior dedicated 3493).
// Prior dedicated dual-wire expand residual: 3595 (CanBuySack residual 2879;
// prior dedicated 3550).
// Prior dedicated dual-wire expand residual: 3640 (CanBuySack residual 2879;
// prior dedicated 3595).
// Prior dedicated dual-wire expand residual: 3685 (CanBuySack residual 2879;
// prior dedicated 3640).
// Prior dedicated dual-wire expand residual: 3730 (CanBuySack residual 2879;
// prior dedicated 3685).
// Prior dedicated dual-wire expand residual: 3775 (CanBuySack residual 2879;
// prior dedicated 3730).
// Prior dedicated dual-wire expand residual: 3820 (CanBuySack residual 2879;
// prior dedicated 3775).
// Dedicated dual-wire expand residual: 3865 (CanBuySack residual 2879;
// prior dedicated 3820).
// Dedicated dual-wire expand residual: 3363 (CanExpand residual 2890;
// prior dedicated 3106).
// Prior dedicated dual-wire expand residual: 3396 (CanClaimScroll residual 2916;
// prior dedicated 3132).
// Dedicated dual-wire expand residual: 3444 (CanClaimScroll residual 2916;
// prior dedicated 3396 / 3132).
//
// Coverage: test_artisan_buy_sack_2879 (residual),
// test_artisan_can_buy_sack_3090 (prior dedicated dual-wire; not in CMake/main),
// test_artisan_can_buy_sack_3242 (prior expand residual 2879; not in CMake/main),
// test_artisan_can_buy_sack_3282 (prior expand residual 2879; not in CMake/main),
// test_artisan_can_buy_sack_3312 (prior expand residual 2879; not in CMake/main),
// test_artisan_can_buy_sack_3493 (prior expand residual 2879; not in CMake/main),
// test_artisan_can_buy_sack_3550 (prior expand residual 2879; not in CMake/main),
// test_artisan_can_buy_sack_3595 (prior expand residual 2879; not in CMake/main),
// test_artisan_can_buy_sack_3640 (prior expand residual 2879; not in CMake/main),
// test_artisan_can_buy_sack_3685 (prior expand residual 2879; not in CMake/main),
// test_artisan_can_buy_sack_3730 (prior expand residual 2879; not in CMake/main),
// test_artisan_can_buy_sack_3775 (prior expand residual 2879; not in CMake/main),
// test_artisan_can_buy_sack_3820 (prior expand residual 2879; not in CMake/main),
// test_artisan_can_buy_sack_3865 (dedicated expand residual 2879; not in CMake/main).
// test_artisan_can_expand_2890 (residual),
// test_artisan_can_expand_3106 (prior dedicated dual-wire; not in CMake/main),
// test_artisan_can_expand_3363 (dedicated expand residual 2890; not in CMake/main).
// test_artisan_claim_scroll_2916 (residual),
// test_artisan_can_claim_scroll_3132 (prior dedicated dual-wire; not in CMake/main),
// test_artisan_can_claim_scroll_3396 (prior expand residual 2916; not in CMake/main),
// test_artisan_can_claim_scroll_3444 (dedicated expand residual 2916; not in CMake/main).
//
//   if option == 1 then -- Buy sack (2879 residual / 3090 prior / 3242 / 3282 / 3312 / 3493 / 3550 / 3595 / 3640 / 3685 / 3730 / 3775 / 3820 / 3865 expand)
//       if player:getGil() >= 9980
//          and player:getContainerSize(xi.inv.MOGSACK) == 0 then
//           player:delGil(9980)
//           player:changeContainerSize(xi.inv.MOGSACK, 30)
//           ...
//       end
//   elseif option == 2 then -- Expand sack (2890 residual / 3106 prior / 3363 expand)
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
//   -- moogleOnFinish option 99 Get Scroll (2916 residual / 3132 prior / 3396 / 3444 expand):
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
// Slice 2879 residual / 3090 prior dedicated / 3242 prior expand residual
// 2879 / 3282 prior expand residual 2879 / 3312 prior expand residual 2879 /
// 3493 prior expand residual 2879 / 3550 prior expand residual 2879 /
// 3595 prior expand residual 2879 / 3640 prior expand residual 2879 /
// 3685 prior expand residual 2879 / 3730 prior expand residual 2879 /
// 3775 prior expand residual 2879 / 3820 prior expand residual 2879 /
// 3865 dedicated expand residual 2879
// — moogleOnUpdate option 1 buy-sack gate
// ---------------------------------------------------------------------------

// BuySackGilCost is the gil required to purchase a Mog Sack.
// Documented from artisan.lua option 1: getGil() >= 9980 / delGil(9980).
// Prior pure port: slice 0948. Residual dual-wire suite: 2879.
// Prior dedicated dual-wire suite: 3090.
// Prior dedicated dual-wire expand residual 2879: 3242.
// Prior dedicated dual-wire expand residual 2879: 3282.
// Prior dedicated dual-wire expand residual 2879: 3312.
// Prior dedicated dual-wire expand residual 2879: 3493.
// Prior dedicated dual-wire expand residual 2879: 3550.
// Prior dedicated dual-wire expand residual 2879: 3595.
// Prior dedicated dual-wire expand residual 2879: 3640.
// Prior dedicated dual-wire expand residual 2879: 3685.
// Prior dedicated dual-wire expand residual 2879: 3730.
// Prior dedicated dual-wire expand residual 2879: 3775.
// Prior dedicated dual-wire expand residual 2879: 3820.
// Dedicated dual-wire expand residual 2879: 3865.
inline constexpr int32 BuySackGilCost = 9980;

// CanBuySack is the pure gate for option 1 (Buy sack):
//
// Formula (slice 3865 dedicated dual-wire expand residual 2879; prior
// dedicated 3820 / 3775 / 3730 / 3685 / 3640 / 3595 / 3550 / 3493 / 3312 / 3282 / 3242 / 3090 / pure 0948 — formula unchanged):
//   CanBuySack(gil, sackSize) = gil >= BuySackGilCost && sackSize == 0
//
// Future Lua host injects scalars into this helper instead of re-inlining
// the gil / empty-sack comparison. Dual-wire of Go artisan.CanBuySack
// (buy_sack.go). Call site: future Lua moogleOnUpdate option 1 inject.
// Prior pure port: slice 0948. Residual dual-wire suite: 2879 /
// test_artisan_buy_sack_2879. Prior dedicated dual-wire suite is
// test_artisan_can_buy_sack_3090. Prior dedicated expand residual suites are
// test_artisan_can_buy_sack_3242 / test_artisan_can_buy_sack_3282 /
// test_artisan_can_buy_sack_3312 / test_artisan_can_buy_sack_3493 /
// test_artisan_can_buy_sack_3550 / test_artisan_can_buy_sack_3595 /
// test_artisan_can_buy_sack_3640 / test_artisan_can_buy_sack_3685 /
// test_artisan_can_buy_sack_3730 / test_artisan_can_buy_sack_3775 /
// test_artisan_can_buy_sack_3820.
// Dedicated expand residual suite is test_artisan_can_buy_sack_3865.
// Host still owns delGil, changeContainerSize, setCharVar, and updateEvent
// after a true gate.
// Coverage: test_artisan_can_buy_sack_3865 (not in CMake/main); residual 2879 /
// prior dedicated 3090 / prior expand 3242 / prior expand 3282 / prior expand
// 3312 / prior expand 3493 / prior expand 3550 / prior expand 3595 /
// prior expand 3640 / prior expand 3685 / prior expand 3730 /
// prior expand 3775 / prior expand 3820 suites retained.
inline auto CanBuySack(const int32 gil, const int32 sackSize) -> bool
{
    return gil >= BuySackGilCost && sackSize == 0;
}

// ---------------------------------------------------------------------------
// Slice 2890 residual / 3106 prior dedicated / 3363 dedicated expand residual
// 2890 — moogleOnUpdate option 2 expand-sack gate
// ---------------------------------------------------------------------------

// CanExpand is the pure gate for option 2 (Expand sack):
//
// Formula (slice 3363 dedicated dual-wire expand residual 2890; prior
// dedicated 3106 / pure 0948 — formula unchanged):
//   CanExpand(sackSize, gobbieSize) = sackSize < gobbieSize && sackSize > 0
//
// Future Lua host injects scalars into this helper instead of re-inlining
// the sack / inventory size comparison. Dual-wire of Go artisan.CanExpand
// (can_expand.go). Call site: future Lua moogleOnUpdate option 2 inject.
// Prior pure port: slice 0948. Residual dual-wire suite: 2890 /
// test_artisan_can_expand_2890. Prior dedicated dual-wire suite is
// test_artisan_can_expand_3106. Dedicated expand residual suite is
// test_artisan_can_expand_3363. Host still owns changeContainerSize,
// updateEvent, and the gobbieCanUpgrade failure param after a true/false gate.
// Coverage: test_artisan_can_expand_3363 (not in CMake/main); residual 2890 /
// prior dedicated 3106 suites retained.
inline auto CanExpand(const int32 sackSize, const int32 gobbieSize) -> bool
{
    return sackSize < gobbieSize && sackSize > 0;
}

// ---------------------------------------------------------------------------
// Slice 2912 / 3147 — moogleOnUpdate option 2 gobbieCanUpgrade event param
// ---------------------------------------------------------------------------

// GobbieUpgradeCap is the inventory size threshold for gobbieCanUpgrade
// (artisan.lua: gobbieSize < 80 and 1 or 0).
// Prior pure port: slice 0948. Residual dual-wire suite: 2912.
// Dedicated dual-wire suite: 3147.
inline constexpr int32 GobbieUpgradeCap = 80;

// GobbieCanUpgradeFlag is the pure gobbieCanUpgrade event param when expand
// fails (option 2 else branch):
//
// Formula (slice 3147 dedicated dual-wire; residual expand 2912 / pure 0948 —
// formula unchanged):
//   GobbieCanUpgradeFlag(gobbieSize) = gobbieSize < GobbieUpgradeCap ? 1 : 0
//
// Future Lua host injects gobbieSize into this helper instead of re-inlining
// the cap comparison. Dual-wire of Go artisan.GobbieCanUpgradeFlag
// (gobbie_upgrade.go). Call site: future Lua moogleOnUpdate option 2 inject
// expand-failure else branch. Prior pure port: slice 0948. Residual dual-wire
// suite: 2912 / test_artisan_gobbie_upgrade_2912. Dedicated dual-wire suite is
// test_artisan_gobbie_upgrade_flag_3147. Host still owns updateEvent after
// CanExpand returns false.
inline auto GobbieCanUpgradeFlag(const int32 gobbieSize) -> int32
{
    return gobbieSize < GobbieUpgradeCap ? 1 : 0;
}

// ---------------------------------------------------------------------------
// Slice 2916 residual / 3132 prior dedicated / 3396 prior dedicated expand /
// 3444 dedicated expand residual 2916 — moogleOnFinish option 99 Get Scroll gate
// ---------------------------------------------------------------------------

// CanClaimScroll is the pure gate for option 99 (Get Scroll):
//
// Formula (slice 3444 dedicated dual-wire expand residual 2916; prior
// dedicated 3396 / 3132 / pure 0948 — formula unchanged):
//   CanClaimScroll(nextScroll, jstMidnight) = nextScroll < jstMidnight
//
// nextScroll is getCharVar('[artisan]nextScroll'); jstMidnight is JstMidnight().
// Future Lua host injects both timestamps into this helper instead of
// re-inlining the comparison. Dual-wire of Go artisan.CanClaimScroll
// (claim_scroll.go). Call site: future Lua moogleOnFinish option 99 inject.
// Prior pure port: slice 0948. Residual dual-wire suite: 2916 /
// test_artisan_claim_scroll_2916. Prior dedicated dual-wire suite is
// test_artisan_can_claim_scroll_3132. Prior dedicated expand residual suite is
// test_artisan_can_claim_scroll_3396. Dedicated expand residual suite is
// test_artisan_can_claim_scroll_3444. giveItem(SCROLL_OF_INSTANT_WARP) and
// setCharVar('[artisan]nextScroll', JstMidnight()) remain host-owned.
// Coverage: test_artisan_can_claim_scroll_3444 (not in CMake/main); residual 2916 /
// prior dedicated 3396 / 3132 suites retained.
inline auto CanClaimScroll(const int64 nextScroll, const int64 jstMidnight) -> bool
{
    return nextScroll < jstMidnight;
}

} // namespace artisanhelpers
