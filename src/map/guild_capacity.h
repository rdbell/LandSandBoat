#pragma once

#include "common/cbasetypes.h"

#include <algorithm>
#include <cstdint>
#include <utility>

// Pure CGuild daily-points trade math extracted for native tests and OmegaXI.

namespace guildhelpers
{

// GPItemRanks mirrors GP_ITEM_RANKS (skill-rank tiers 3..9).
constexpr size_t GPItemRanks = 7;

// SkillRankMin / SkillRankMax clamp for RealSkills.rank[guildId+48].
constexpr uint8 SkillRankMin = 3;
constexpr uint8 SkillRankMax = 9;

// SkillRankOffset is added to guild id for RealSkills.rank index.
constexpr uint8 SkillRankOffset = 48;

// DailyPointsIneligible is curPoints == 1 (recent guild change).
constexpr int32 DailyPointsIneligible = 1;

// ClampSkillRank mirrors std::clamp(rank, 3, 9).
inline auto ClampSkillRank(const uint8 rank) -> uint8
{
    return std::clamp(rank, SkillRankMin, SkillRankMax);
}

// SkillRankToTier mirrors rank - 3 for m_GPItems[rank - 3] indexing.
inline auto SkillRankToTier(const uint8 clampedRank) -> uint8
{
    return static_cast<uint8>(clampedRank - SkillRankMin);
}

// SkillRankCharIndex mirrors m_id + 48 into RealSkills.rank.
inline auto SkillRankCharIndex(const uint8 guildID) -> uint8
{
    return static_cast<uint8>(guildID + SkillRankOffset);
}

// IsDailyPointsEligible mirrors curPoints != 1.
inline auto IsDailyPointsEligible(const int32 curPoints) -> bool
{
    return curPoints != DailyPointsIneligible;
}

// PatternRankAfterAdvance mirrors (rank + 1) % (tier + 4).
inline auto PatternRankAfterAdvance(const uint8 currentRank, const size_t tierIndex) -> uint8
{
    return static_cast<uint8>((currentRank + 1) % (tierIndex + 4));
}

// PatternRankFromElapsedDays mirrors elapsedDays % (i + 4).
inline auto PatternRankFromElapsedDays(const uint32 elapsedDays, const size_t tierIndex) -> uint8
{
    return static_cast<uint8>(elapsedDays % static_cast<uint32>(tierIndex + 4));
}

// RemainingDailyCapacity mirrors maxpoints - clamp(curPoints, 0, maxpoints).
inline auto RemainingDailyCapacity(const uint16 curPoints, const uint16 maxPoints) -> uint16
{
    return static_cast<uint16>(maxPoints - std::clamp(curPoints, static_cast<uint16>(0), maxPoints));
}

// TradeQuantity mirrors
// min(((maxpoints - clamp(cur,0,max)) / points) + 1, reserve).
// When points is 0, returns 0 to avoid division by zero (defensive; production
// guild_item_points rows always have points > 0).
inline auto TradeQuantity(const uint16 curPoints, const uint16 maxPoints, const uint16 pointsPerItem, const uint16 reserve) -> uint16
{
    if (pointsPerItem == 0)
    {
        return 0;
    }
    const uint16 remaining = RemainingDailyCapacity(curPoints, maxPoints);
    const uint16 fromCap   = static_cast<uint16>((remaining / pointsPerItem) + 1);
    return std::min(fromCap, reserve);
}

// ClampPointsToAdd mirrors the curPoints <= maxpoints branch:
// pointsToAdd = clamp(points * quantity, 0, maxpoints - curPoints) when under
// cap; else 0.
inline auto ClampPointsToAdd(const uint16 curPoints, const uint16 maxPoints, const uint16 pointsPerItem, const uint16 quantity) -> uint16
{
    if (curPoints > maxPoints)
    {
        return 0;
    }
    const uint32 raw = static_cast<uint32>(pointsPerItem) * static_cast<uint32>(quantity);
    const uint16 room = static_cast<uint16>(maxPoints - curPoints);
    if (raw > room)
    {
        return room;
    }
    return static_cast<uint16>(raw);
}

// AddGuildPointsResult is pure (quantity, pointsToAdd) before host side effects.
struct AddGuildPointsResult
{
    uint8  quantity;
    int16  pointsToAdd;
};

// ComputeAddGuildPoints mirrors the matched-item path of addGuildPoints.
// ineligible when curPoints == 1; itemMatched when GP item id matches trade.
inline auto ComputeAddGuildPoints(
    const bool   ineligible,
    const bool   itemMatched,
    const uint16 curPoints,
    const uint16 maxPoints,
    const uint16 pointsPerItem,
    const uint16 reserve) -> AddGuildPointsResult
{
    if (ineligible || !itemMatched)
    {
        return { 0, 0 };
    }
    const uint16 quantity    = TradeQuantity(curPoints, maxPoints, pointsPerItem, reserve);
    const uint16 pointsToAdd = ClampPointsToAdd(curPoints, maxPoints, pointsPerItem, quantity);
    return { static_cast<uint8>(std::min<uint16>(quantity, 255)), static_cast<int16>(pointsToAdd) };
}

// DailyGPItemResult is pure (itemId, remainingPoints) for getDailyGPItem.
struct DailyGPItemResult
{
    uint16 itemId;
    uint16 remainingPoints;
};

// ComputeDailyGPItem mirrors getDailyGPItem after rank clamp and first GP item.
// ineligible → remaining 0; else remaining capacity.
inline auto ComputeDailyGPItem(
    const bool   ineligible,
    const uint16 itemId,
    const uint16 curPoints,
    const uint16 maxPoints) -> DailyGPItemResult
{
    if (ineligible)
    {
        return { itemId, 0 };
    }
    return { itemId, RemainingDailyCapacity(curPoints, maxPoints) };
}

// NewDailyPointsTotal mirrors curPoints + pointsToAdd for char-var write.
inline auto NewDailyPointsTotal(const uint16 curPoints, const uint16 pointsToAdd) -> uint16
{
    return static_cast<uint16>(curPoints + pointsToAdd);
}

} // namespace guildhelpers

// ---------------------------------------------------------------------------
// Guild Points (Union Representative / Lua crafting catalogs) dual-wire
// helpers. Separate from CGuild daily-points math above (guildhelpers).
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 1016: CanAfford pure port (CanBuy* compose; internal/guildpoints)
//   - 2944: CanAfford residual dual-wire suite (getCurrency >= cost)
//   - 3221: CanAfford prior dedicated dual-wire (retained; expand residual 2944)
//   - 3368: CanAfford prior dedicated dual-wire (retained; expand residual 2944)
//   - 3404: CanAfford prior dedicated dual-wire (retained; expand residual 2944)
//   - 3458: CanAfford prior dedicated dual-wire (retained; expand residual 2944)
//   - 3507: CanAfford prior dedicated dual-wire (retained; expand residual 2944)
//   - 3556: CanAfford prior dedicated dual-wire (retained; expand residual 2944)
//   - 3601: CanAfford prior dedicated dual-wire (retained; expand residual 2944)
//   - 3646: CanAfford prior dedicated dual-wire (retained; expand residual 2944)
//   - 3691: CanAfford prior dedicated dual-wire (retained; expand residual 2944)
//   - 3736: CanAfford prior dedicated dual-wire (retained; expand residual 2944)
//   - 3781: CanAfford prior dedicated dual-wire (retained; expand residual 2944)
//   - 3826: CanAfford prior dedicated dual-wire (retained; expand residual 2944)
//   - 3871: CanAfford dedicated dual-wire (can_afford.go; expand residual 2944)
//
// Dual-wire index:
//   - 2944: CanAfford residual dual-wire suite
//   - 3221: CanAfford prior dedicated dual-wire (retained)
//   - 3368: CanAfford prior dedicated dual-wire (retained)
//   - 3404: CanAfford prior dedicated dual-wire (retained)
//   - 3458: CanAfford prior dedicated dual-wire (retained)
//   - 3507: CanAfford prior dedicated dual-wire (retained)
//   - 3556: CanAfford prior dedicated dual-wire (retained)
//   - 3601: CanAfford prior dedicated dual-wire (retained)
//   - 3646: CanAfford prior dedicated dual-wire (retained)
//   - 3691: CanAfford prior dedicated dual-wire (retained)
//   - 3736: CanAfford prior dedicated dual-wire (retained)
//   - 3781: CanAfford prior dedicated dual-wire (retained)
//   - 3826: CanAfford prior dedicated dual-wire (retained)
//   - 3871: CanAfford = currency >= cost
//
// Lua production host: scripts/globals/hobbies/crafting/guild_points.lua
//   player:getCurrency(currency) >= keyItem.cost / cost  (purchase gates)
//
// Host injects scalars only (no player / entity pointers):
//   currency — player:getCurrency(...)
//   cost     — keyItem.cost / item.cost*qty / crystal.cost*qty
//
// delCurrency / giveKeyItem / addItem writeback remains host-owned.
// Prior pure port: OmegaXI slice 1016 (internal/guildpoints).
// Residual dual-wire suite: 2944 (test_guildpoints_can_afford_2944).
// Prior dedicated dual-wire suites: 3221 (test_guildpoints_can_afford_3221),
//   3368 (test_guildpoints_can_afford_3368),
//   3404 (test_guildpoints_can_afford_3404),
//   3458 (test_guildpoints_can_afford_3458),
//   3507 (test_guildpoints_can_afford_3507),
//   3556 (test_guildpoints_can_afford_3556),
//   3601 (test_guildpoints_can_afford_3601),
//   3646 (test_guildpoints_can_afford_3646),
//   3691 (test_guildpoints_can_afford_3691),
//   3736 (test_guildpoints_can_afford_3736),
//   3781 (test_guildpoints_can_afford_3781),
//   3826 (test_guildpoints_can_afford_3826).
// Dedicated dual-wire suite: 3871 (test_guildpoints_can_afford_3871).
// Go dual-wire: guildpoints.CanAfford (internal/guildpoints/can_afford.go).
// Future Lua host injects free function then delCurrency / give writeback.
// ---------------------------------------------------------------------------

namespace guildpointshelpers
{

// ---------------------------------------------------------------------------
// Slice 2944 residual / 3221 / 3368 / 3404 / 3458 / 3507 / 3556 / 3601 / 3646 /
// 3691 / 3736 / 3781 / 3826 prior dedicated / 3871 dedicated — guild_points getCurrency afford gate
// (dedicated expand residual 2944; prior dedicated 3826 / 3781 / 3736 / 3691 / 3646 / 3601 /
// 3556 / 3507 / 3458 / 3404 / 3368 / 3221 / pure 1016)
// ---------------------------------------------------------------------------

// CanAfford is the pure free-function form of the Lua guild_points currency
// gate:
//
//   currency >= cost
//
// Formula (slice 3871 dedicated dual-wire expand residual 2944; prior
// dedicated 3826 / 3781 / 3736 / 3691 / 3646 / 3601 / 3556 / 3507 / 3458 / 3404 / 3368 / 3221 / pure 1016 —
// formula unchanged):
//
//   CanAfford(currency, cost) = currency >= cost
//
// Future Lua host injects getCurrency / catalog cost scalars into this helper
// instead of re-inlining the comparison. Matches Go guildpoints.CanAfford.
// Residual dual-wire suite: 2944 / test_guildpoints_can_afford_2944.
// Prior dedicated dual-wire suites: 3221 / test_guildpoints_can_afford_3221,
//   3368 / test_guildpoints_can_afford_3368,
//   3404 / test_guildpoints_can_afford_3404,
//   3458 / test_guildpoints_can_afford_3458,
//   3507 / test_guildpoints_can_afford_3507,
//   3556 / test_guildpoints_can_afford_3556,
//   3601 / test_guildpoints_can_afford_3601,
//   3646 / test_guildpoints_can_afford_3646,
//   3691 / test_guildpoints_can_afford_3691,
//   3736 / test_guildpoints_can_afford_3736,
//   3781 / test_guildpoints_can_afford_3781,
//   3826 / test_guildpoints_can_afford_3826.
// Dedicated dual-wire suite is test_guildpoints_can_afford_3871.
inline auto CanAfford(const int32 currency, const int32 cost) -> bool
{
    return currency >= cost;
}

} // namespace guildpointshelpers
