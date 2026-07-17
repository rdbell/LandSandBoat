#pragma once

#include "common/cbasetypes.h"

#include <cstddef>
#include <cstdint>

// Pure CTreasurePool add/lot/eviction policy halves.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 1367: free-slot / rare / eviction / lot policy suite
//   - 2772 / 2777 / 2779 / 2780 / 2781: lot/pass/update/post-lot/flush plans
//   - 2938: ShouldAutoResolveSolo residual dual-wire suite (memberCount == 1)
//   - 2957: CanLotWithInventory residual dual-wire suite (freeSlots != 0 inventory lot gate)
//   - 2981: ShouldForceCheckOnFullPoolInsert residual dual-wire suite (SlotID == PoolSize after free scan)
//   - 2998: CanLotRareItem residual dual-wire suite (!(rare && alreadyHas))
//   - 3060: ShouldRejectNullMember (charNull || poolMismatch null-member gate)
//   - 3067: ShouldRejectNullItem (itemNull identity null-item gate)
//   - 3094: ShouldSkipRareCheck (!isSoloPool && itemHasNoRareCheck skip-rare gate)
//   - 3112: ShouldUpdatePoolForChar (!isDisappear UpdatePool visibility gate)
//   - 3127: ShouldFlushPool (itemCount != 0 flush entry gate)
//   - 3201: ShouldAutoResolveSolo dedicated dual-wire (auto_solo.go; expand residual 2938)
//   - 3260: CanLotRareItem prior dedicated dual-wire (lot_rare.go; expand residual 2998)
//   - 3291: CanLotRareItem prior dedicated dual-wire expand residual 2998 (prior ~3260)
//   - 3321: CanLotRareItem dedicated dual-wire expand residual 2998 (prior ~3291)
//   - 3367: CanLotWithInventory dedicated dual-wire expand residual 2957
//   - 3379: ShouldForceCheckOnFullPoolInsert prior dedicated dual-wire expand residual 2981
//   - 3423: ShouldForceCheckOnFullPoolInsert prior dedicated dual-wire expand residual 2981 (prior 3379)
//   - 3477: ShouldForceCheckOnFullPoolInsert prior dedicated dual-wire expand residual 2981 (prior 3423 / 3379)
//   - 3533: ShouldForceCheckOnFullPoolInsert prior dedicated dual-wire expand residual 2981 (prior 3477 / 3423 / 3379)
//   - 3565: ShouldForceCheckOnFullPoolInsert prior dedicated dual-wire expand residual 2981 (prior 3533 / 3477 / 3423 / 3379)
//   - 3610: ShouldForceCheckOnFullPoolInsert prior dedicated dual-wire expand residual 2981 (prior 3565 / 3533 / 3477 / 3423 / 3379)
//   - 3655: ShouldForceCheckOnFullPoolInsert prior dedicated dual-wire expand residual 2981 (prior 3610 / 3565 / 3533 / 3477 / 3423 / 3379)
//   - 3700: ShouldForceCheckOnFullPoolInsert dedicated dual-wire expand residual 2981 (prior 3655 / 3610 / 3565 / 3533 / 3477 / 3423 / 3379)
//
// Dual-wire index:
//   - 2938: ShouldAutoResolveSolo residual dual-wire suite
//   - 2957: CanLotWithInventory residual dual-wire suite
//   - 2981: ShouldForceCheckOnFullPoolInsert residual dual-wire suite
//   - 2998: CanLotRareItem residual dual-wire suite
//   - 3201: ShouldAutoResolveSolo = memberCount == 1
//   - 3260: CanLotRareItem prior dedicated (!itemIsRare || !alreadyHasItem)
//   - 3291: CanLotRareItem prior dedicated (!itemIsRare || !alreadyHasItem)
//   - 3321: CanLotRareItem = !itemIsRare || !alreadyHasItem
//   - 3367: CanLotWithInventory = freeSlots != 0
//   - 3379: ShouldForceCheckOnFullPoolInsert prior dedicated = slotAfterFreeScan == PoolSize
//   - 3423: ShouldForceCheckOnFullPoolInsert prior dedicated = slotAfterFreeScan == PoolSize
//   - 3477: ShouldForceCheckOnFullPoolInsert prior dedicated = slotAfterFreeScan == PoolSize
//   - 3533: ShouldForceCheckOnFullPoolInsert prior dedicated = slotAfterFreeScan == PoolSize
//   - 3565: ShouldForceCheckOnFullPoolInsert prior dedicated = slotAfterFreeScan == PoolSize
//   - 3610: ShouldForceCheckOnFullPoolInsert prior dedicated = slotAfterFreeScan == PoolSize
//   - 3655: ShouldForceCheckOnFullPoolInsert prior dedicated = slotAfterFreeScan == PoolSize
//   - 3700: ShouldForceCheckOnFullPoolInsert = slotAfterFreeScan == PoolSize
//
// Production host: CTreasurePool::addItem (treasure_pool.cpp) injects
// memberCount() into ShouldAutoResolveSolo after trophy list packets.
// Go dual-wire: treasurepool.ShouldAutoResolveSolo
// (internal/treasurepool/auto_solo.go).
// Residual dual-wire suite: 2938 (test_treasure_auto_solo_2938).
// Dedicated dual-wire suite: 3201 (test_treasurepool_auto_resolve_solo_3201).
//
// Production host: CTreasurePool::lotItem / PlanLotItemPreflight injects
// getStorage(LOC_INVENTORY)->GetFreeSlotsCount() into CanLotWithInventory.
// Go dual-wire: treasurepool.CanLotWithInventory
// (internal/treasurepool/lot_inventory.go).
// Residual dual-wire suite: 2957 (test_treasure_lot_inventory_2957).
// Dedicated dual-wire suite: 3367 (test_treasure_lot_inventory_3367).
//
// Production host: CTreasurePool::addItem injects SlotID after free-slot /
// eviction selection into ShouldForceCheckOnFullPoolInsert (SlotID ends at
// PoolSize when free-slot loop completes without break).
// Go dual-wire: treasurepool.ShouldForceCheckOnFullPoolInsert
// (internal/treasurepool/force_check_full.go).
// Residual dual-wire suite: 2981 (test_treasure_force_check_full_2981).
// Prior dedicated dual-wire suites: 3379 (test_treasure_force_check_full_3379),
// 3423 (test_treasure_force_check_full_3423),
// 3477 (test_treasure_force_check_full_3477),
// 3533 (test_treasure_force_check_full_3533),
// 3565 (test_treasure_force_check_full_3565),
// 3610 (test_treasure_force_check_full_3610),
// 3655 (test_treasure_force_check_full_3655).
// Dedicated dual-wire suite: 3700 (test_treasure_force_check_full_3700).
//
// Production host: CTreasurePool::lotItem / PlanLotItemPreflight injects
// item rare flag + already-has lookup into CanLotRareItem.
// Go dual-wire: treasurepool.CanLotRareItem
// (internal/treasurepool/lot_rare.go).
// Residual dual-wire suite: 2998 (test_treasure_lot_rare_2998).
// Prior dedicated dual-wire suites: 3260 (test_treasurepool_lot_rare_3260),
// 3291 (test_treasurepool_lot_rare_3291).
// Dedicated dual-wire suite: 3321 (test_treasurepool_lot_rare_3321).
//
// Production host: CTreasurePool::{lotItem,passItem,UpdatePool} /
// PlanLotItemPreflight / PlanPassItemPreflight / PlanUpdatePool inject
// (PChar == nullptr) and (PChar->PTreasurePool != this) into
// ShouldRejectNullMember.
// Go dual-wire: treasurepool.ShouldRejectNullMember
// (internal/treasurepool/reject_null_member.go).
//
// Production host: CTreasurePool::lotItem / PlanLotItemPreflight injects
// (PItem / PNewItem == nullptr) into ShouldRejectNullItem.
// Go dual-wire: treasurepool.ShouldRejectNullItem
// (internal/treasurepool/reject_null_item.go).
//
// Production host: CTreasurePool::addItem injects
// (m_TreasurePoolType == Solo) and PNewItem->hasFlag(NoRareCheck) into
// ShouldSkipRareCheck before ShouldApplyRareMemberCheck.
// Go dual-wire: treasurepool.ShouldSkipRareCheck
// (internal/treasurepool/skip_rare_check.go).
// Sibling dual-wires (leave alone): ShouldRejectNullMember (3060),
// ShouldRejectNullItem (3067).
//
// Production host: CTreasurePool::UpdatePool / PlanUpdatePool injects
// (PChar->status == STATUS_TYPE::DISAPPEAR) into ShouldUpdatePoolForChar
// after the null-member early gate.
// Go dual-wire: treasurepool.ShouldUpdatePoolForChar
// (internal/treasurepool/update_pool_for_char.go).
// Sibling dual-wires (leave alone): ShouldRejectNullMember (3060),
// ShouldRejectNullItem (3067), ShouldSkipRareCheck (3094).
//
// Production host: CTreasurePool::flush / PlanFlush injects m_count into
// ShouldFlushPool before advancing the flush tick and checkTreasureItem loop.
// Go dual-wire: treasurepool.ShouldFlushPool
// (internal/treasurepool/flush_pool.go).
// Sibling dual-wires (leave alone): ShouldRejectNullMember (3060),
// ShouldRejectNullItem (3067), ShouldSkipRareCheck (3094),
// ShouldUpdatePoolForChar (3112).

namespace treasurepoolhelpers
{

// TREASUREPOOL_SIZE mirror.
constexpr uint8 PoolSize = 10;

// FreeSlotID starts as uint8(-1) == 255; "unset" is FreeSlotID > PoolSize.
constexpr uint8 FreeSlotUnset = static_cast<uint8>(-1);

// Pass lot mask FF FF.
constexpr uint16 PassedLot = 65535;

// treasure_checktime mirror (seconds).
constexpr uint32 TreasureCheckTimeSeconds = 3;

// IsFreeSlotUnset mirrors FreeSlotID > TREASUREPOOL_SIZE after uint8(-1) init.
inline auto IsFreeSlotUnset(const uint8 freeSlotID) -> bool
{
    return freeSlotID > PoolSize;
}

// FirstEmptySlotFound when freeSlotID was set from an empty slot (ID==0).
inline auto IsValidFreeSlot(const uint8 freeSlotID) -> bool
{
    return freeSlotID < PoolSize;
}

// ShouldSkipRareCheck mirrors non-solo pool + NoRareCheck item flag.
//
// Formula (slice 3094 dual-wire):
//   !isSoloPool && itemHasNoRareCheck
//
// isSoloPool         — host-evaluated (m_TreasurePoolType == TreasurePoolType::Solo)
// itemHasNoRareCheck — host-evaluated PNewItem->hasFlag(ItemFlag::NoRareCheck)
// true  → host skips the rare-member ownership scan on addItem
// false → host may still apply rare-member check via ShouldApplyRareMemberCheck
//
// Dual-wire of Go treasurepool.ShouldSkipRareCheck.
// Call site: CTreasurePool::addItem before ShouldApplyRareMemberCheck /
// ShouldRejectRareAllHave.
// Sibling dual-wires (leave alone): ShouldRejectNullMember (3060),
// ShouldRejectNullItem (3067).
inline auto ShouldSkipRareCheck(const bool isSoloPool, const bool itemHasNoRareCheck) -> bool
{
    return !isSoloPool && itemHasNoRareCheck;
}

// ShouldApplyRareMemberCheck mirrors rare item and not skip.
inline auto ShouldApplyRareMemberCheck(const bool itemIsRare, const bool skipRareCheck) -> bool
{
    return itemIsRare && !skipRareCheck;
}

// ShouldRejectRareAllHave mirrors everyone already owns the rare item.
inline auto ShouldRejectRareAllHave(const bool applyRareCheck, const bool anyMemberMissingRare) -> bool
{
    return applyRareCheck && !anyMemberMissingRare;
}

// CanEvictNonRareNonExclusive mirrors !Rare && !Exclusive for oldest pass 1.
inline auto CanEvictNonRareNonExclusive(const bool hasRare, const bool hasExclusive) -> bool
{
    return !hasRare && !hasExclusive;
}

// CanEvictNonExclusive mirrors !Exclusive for oldest pass 2.
inline auto CanEvictNonExclusive(const bool hasExclusive) -> bool
{
    return !hasExclusive;
}

// PreferOlderTimestamp mirrors TimeStamp < oldest for eviction candidate.
// Times are host-normalized comparable units.
inline auto PreferOlderTimestamp(const int64 candidateStamp, const int64 oldestStamp) -> bool
{
    return candidateStamp < oldestStamp;
}

// DefaultFallbackSlot mirrors FreeSlotID = 0 when all eviction passes fail.
inline auto DefaultFallbackSlot() -> uint8
{
    return 0;
}

// ---------------------------------------------------------------------------
// Slice 3700 — addItem full-pool force-check gate (dedicated expand residual 2981;
// prior dedicated 3655 / 3610 / 3565 / 3533 / 3477 / 3423 / 3379 retained)
// ---------------------------------------------------------------------------

// ShouldForceCheckOnFullPoolInsert mirrors SlotID == 10 after free-slot scan.
//
// Formula (slice 3700 dedicated dual-wire expand residual 2981; prior dedicated
// 3655 / 3610 / 3565 / 3533 / 3477 / 3423 / 3379 / pure 1367 — formula unchanged):
//   slotAfterFreeScan == PoolSize
//
// slotAfterFreeScan — host-evaluated SlotID after free-slot scan
// (SlotID ends at PoolSize when free-slot loop completes without break)
// true  → force checkTreasureItem on FreeSlotID before insert
// false → skip force-check; proceed to normal insert bookkeeping
//
// Dual-wire of Go treasurepool.ShouldForceCheckOnFullPoolInsert.
// Call site: CTreasurePool::addItem after free-slot / eviction selection.
// Prior pure port: slice 1367. Residual dual-wire suite: 2981 /
// test_treasure_force_check_full_2981. Prior dedicated dual-wire suites: 3379 /
// test_treasure_force_check_full_3379, 3423 / test_treasure_force_check_full_3423,
// 3477 / test_treasure_force_check_full_3477, 3533 /
// test_treasure_force_check_full_3533, 3565 /
// test_treasure_force_check_full_3565, 3610 /
// test_treasure_force_check_full_3610, 3655 /
// test_treasure_force_check_full_3655.
// Dedicated dual-wire suite is test_treasure_force_check_full_3700. Sibling
// dual-wire gates: CanLotWithInventory (3367), CanLotRareItem (3321),
// ShouldAutoResolveSolo (3201), ShouldRejectNullMember (3060),
// ShouldRejectNullItem (3067), ShouldSkipRareCheck (3094),
// ShouldUpdatePoolForChar (3112), ShouldFlushPool (3127) — left residual.
inline auto ShouldForceCheckOnFullPoolInsert(const uint8 slotAfterFreeScan) -> bool
{
    return slotAfterFreeScan == PoolSize;
}

// ---------------------------------------------------------------------------
// Slice 3201 — addItem solo auto-resolve gate (dedicated expand residual 2938)
// ---------------------------------------------------------------------------

// ShouldAutoResolveSolo mirrors memberCount() == 1 after insert.
//
// Formula (slice 3201 dedicated dual-wire; residual expand 2938 / pure 1367 —
// formula unchanged):
//   memberCount == 1
//
// memberCount — host-evaluated memberCount() (current pool members after insert)
// true  → auto-resolve the inserted slot via checkTreasureItem immediately
// false → leave the item for lot/pass/timeout resolution
//
// Dual-wire of Go treasurepool.ShouldAutoResolveSolo.
// Call site: CTreasurePool::addItem after trophy list packets.
// Prior pure port: slice 1367. Residual dual-wire suite: 2938 /
// test_treasure_auto_solo_2938. Dedicated dual-wire suite is
// test_treasurepool_auto_resolve_solo_3201. Sibling dual-wire gates:
// CanLotWithInventory (2957), ShouldForceCheckOnFullPoolInsert (2981),
// CanLotRareItem (2998), ShouldRejectNullMember (3060),
// ShouldRejectNullItem (3067), ShouldSkipRareCheck (3094),
// ShouldUpdatePoolForChar (3112), ShouldFlushPool (3127) — left residual.
inline auto ShouldAutoResolveSolo(const std::size_t memberCount) -> bool
{
    return memberCount == 1;
}

// IsSlotInRange mirrors SlotID >= TREASUREPOOL_SIZE reject for lot/pass.
inline auto IsSlotOutOfRange(const uint8 slotID) -> bool
{
    return slotID >= PoolSize;
}

// ---------------------------------------------------------------------------
// Slice 3367 — lot free-inventory gate (dedicated expand residual 2957)
// ---------------------------------------------------------------------------

// CanLotWithInventory mirrors freeSlots != 0.
//
// Formula (slice 3367 dedicated dual-wire expand residual 2957; prior pure
// 1367 — formula unchanged):
//   freeSlots != 0
//
// freeSlots — host-evaluated GetFreeSlotsCount() on the lotting character's
// inventory storage (LOC_INVENTORY)
// true  → host may proceed past the full-inventory lot preflight gate
// false → host rejects the lot (RejectFullInventory / packet injection)
//
// Dual-wire of Go treasurepool.CanLotWithInventory.
// Call site: PlanLotItemPreflight / CTreasurePool::lotItem after item lookup.
// Prior pure port: slice 1367. Residual dual-wire suite: 2957 /
// test_treasure_lot_inventory_2957. Dedicated dual-wire suite is
// test_treasure_lot_inventory_3367. Sibling dual-wire gates:
// ShouldForceCheckOnFullPoolInsert (2981), CanLotRareItem (3321),
// ShouldAutoResolveSolo (3201), ShouldRejectNullMember (3060),
// ShouldRejectNullItem (3067), ShouldSkipRareCheck (3094),
// ShouldUpdatePoolForChar (3112), ShouldFlushPool (3127) — left residual.
inline auto CanLotWithInventory(const uint8 freeSlots) -> bool
{
    return freeSlots != 0;
}

// ---------------------------------------------------------------------------
// Slice 3321 — lot rare-owned gate (dedicated expand residual 2998; prior ~3291)
// ---------------------------------------------------------------------------

// CanLotRareItem mirrors !(rare && alreadyHas).
//
// Formula (slice 3321 dedicated dual-wire expand residual 2998; prior dedicated
// ~3291 / ~3260 / pure 1367 — formula unchanged):
//   !itemIsRare || !alreadyHasItem
//   // equivalent pin form: !(itemIsRare && alreadyHasItem)
//
// itemIsRare     — host-evaluated item rare flag
// alreadyHasItem — host-evaluated whether the lotting character already holds
//                  the rare item
// true  → host may proceed past the rare-owned lot preflight gate
// false → host rejects the lot (RejectRareOwned / packet injection)
//
// Dual-wire of Go treasurepool.CanLotRareItem.
// Call site: PlanLotItemPreflight / CTreasurePool::lotItem after inventory gate.
// Prior pure port: slice 1367. Residual dual-wire suite: 2998 /
// test_treasure_lot_rare_2998. Prior dedicated dual-wire suites: 3260 /
// test_treasurepool_lot_rare_3260, 3291 / test_treasurepool_lot_rare_3291.
// Dedicated dual-wire suite is test_treasurepool_lot_rare_3321. Sibling dual-wire
// gates: CanLotWithInventory (2957), ShouldForceCheckOnFullPoolInsert (2981),
// ShouldAutoResolveSolo (3201), ShouldRejectNullMember (3060),
// ShouldRejectNullItem (3067), ShouldSkipRareCheck (3094),
// ShouldUpdatePoolForChar (3112), ShouldFlushPool (3127) — left residual.
inline auto CanLotRareItem(const bool itemIsRare, const bool alreadyHasItem) -> bool
{
    return !(itemIsRare && alreadyHasItem);
}

// IsHigherLot mirrors lotInfo.lot > highestLot.
inline auto IsHigherLot(const uint16 candidateLot, const uint16 highestLot) -> bool
{
    return candidateLot > highestLot;
}

// HigherLotSelection returns candidateLot when it strictly exceeds currentHighestLot,
// otherwise keeps currentHighestLot. Mirrors the highest-lot scan update in lotItem.
inline auto HigherLotSelection(const uint16 currentHighestLot, const uint16 candidateLot) -> uint16
{
    if (IsHigherLot(candidateLot, currentHighestLot))
    {
        return candidateLot;
    }
    return currentHighestLot;
}

// ShouldEvaluateTreasureImmediately mirrors Lotters.size() == memberCount() after lot/pass.
inline auto ShouldEvaluateTreasureImmediately(const std::size_t lotterCount, const std::size_t memberCount) -> bool
{
    return lotterCount == memberCount;
}

// PostLotPlan is the pure post-emplace disposition of CTreasurePool::lotItem after
// LotInfo is recorded. Trophy-solution packet push is always host-side; plan only
// gates immediate checkTreasureItem when every member has lotted.
struct PostLotPlan
{
    bool evaluateImmediately; // Lotters.size() == memberCount()
};

// PlanPostLot composes ShouldEvaluateTreasureImmediately after a lotter is added.
// Host always pushes trophy solution packets; evaluateImmediately alone is pure.
inline auto PlanPostLot(const std::size_t lotterCountAfterAdd, const std::size_t memberCount) -> PostLotPlan
{
    return PostLotPlan{ ShouldEvaluateTreasureImmediately(lotterCountAfterAdd, memberCount) };
}

// IsPassedLot mirrors lot == 65535.
inline auto IsPassedLot(const uint16 lot) -> bool
{
    return lot == PassedLot;
}

// ShouldRejectNullMember mirrors PChar null || pool mismatch.
//
// Formula (slice 3060 dual-wire):
//   charNull || poolMismatch
//
// charNull     — host-evaluated (PChar == nullptr)
// poolMismatch — host-evaluated (PChar->PTreasurePool != this)
// true  → host rejects (warn / return) before slot / item / inventory gates
// false → host may proceed past the null-member early gate
//
// Dual-wire of Go treasurepool.ShouldRejectNullMember.
// Call sites: PlanLotItemPreflight / PlanPassItemPreflight / PlanUpdatePool
// and CTreasurePool::{lotItem,passItem,UpdatePool}.
// Sibling dual-wire (slice 3067): ShouldRejectNullItem.
inline auto ShouldRejectNullMember(const bool charNull, const bool poolMismatch) -> bool
{
    return charNull || poolMismatch;
}

// ShouldRejectNullItem mirrors !PNewItem / !PItem for lot.
//
// Formula (slice 3067 dual-wire):
//   itemNull
//
// itemNull — host-evaluated (PItem / PNewItem == nullptr after slot lookup)
// true  → host rejects (warn) before inventory / rare gates
// false → host may proceed past the null-item early gate
//
// Dual-wire of Go treasurepool.ShouldRejectNullItem.
// Call site: PlanLotItemPreflight / CTreasurePool::lotItem.
// Sibling dual-wire (slice 3060): ShouldRejectNullMember.
inline auto ShouldRejectNullItem(const bool itemNull) -> bool
{
    return itemNull;
}

// LotItemPreflight is the pure early-gate disposition of CTreasurePool::lotItem
// before LotInfo is recorded. Host keeps logging/side effects; plan is pure.
enum class LotItemPreflight : uint8
{
    Proceed = 0,
    RejectMember,
    RejectSlot,
    RejectItem,
    RejectFullInventory,
    RejectRareOwned,
};

// PlanLotItemPreflight short-circuits in production lotItem order:
// 1) null char or pool mismatch
// 2) slot out of range
// 3) null item lookup
// 4) free inventory slots == 0
// 5) rare && already has
// 6) proceed to record lot
// Composes ShouldRejectNullMember / IsSlotOutOfRange host flag / ShouldRejectNullItem /
// CanLotWithInventory / CanLotRareItem.
inline auto PlanLotItemPreflight(
    const bool charNull,
    const bool poolMismatch,
    const bool slotOutOfRange,
    const bool itemNull,
    const uint8 freeSlots,
    const bool itemIsRare,
    const bool alreadyHasItem) -> LotItemPreflight
{
    if (ShouldRejectNullMember(charNull, poolMismatch))
    {
        return LotItemPreflight::RejectMember;
    }
    if (slotOutOfRange)
    {
        return LotItemPreflight::RejectSlot;
    }
    if (ShouldRejectNullItem(itemNull))
    {
        return LotItemPreflight::RejectItem;
    }
    if (!CanLotWithInventory(freeSlots))
    {
        return LotItemPreflight::RejectFullInventory;
    }
    if (!CanLotRareItem(itemIsRare, alreadyHasItem))
    {
        return LotItemPreflight::RejectRareOwned;
    }
    return LotItemPreflight::Proceed;
}

// PassItemPreflight is the pure early-gate disposition of CTreasurePool::passItem
// before LotInfo is recorded or mutated. Host keeps logging/side effects; plan is pure.
enum class PassItemPreflight : uint8
{
    Proceed = 0,
    RejectMember,
    RejectSlot,
};

// PlanPassItemPreflight short-circuits in production passItem order:
// 1) null char or pool mismatch
// 2) slot out of range
// 3) proceed to record/mutate the pass
// Composes ShouldRejectNullMember / host slotOutOfRange (from IsSlotOutOfRange).
inline auto PlanPassItemPreflight(
    const bool charNull,
    const bool poolMismatch,
    const bool slotOutOfRange) -> PassItemPreflight
{
    if (ShouldRejectNullMember(charNull, poolMismatch))
    {
        return PassItemPreflight::RejectMember;
    }
    if (slotOutOfRange)
    {
        return PassItemPreflight::RejectSlot;
    }
    return PassItemPreflight::Proceed;
}

// ShouldUpdatePoolForChar mirrors status != DISAPPEAR.
//
// Formula (slice 3112 dual-wire):
//   !isDisappear
//
// isDisappear — host-evaluated (PChar->status == STATUS_TYPE::DISAPPEAR)
// true  → host may push trophy list packets for this character
// false → host skips trophy push for a disappeared character
//
// Dual-wire of Go treasurepool.ShouldUpdatePoolForChar.
// Call site: PlanUpdatePool / CTreasurePool::UpdatePool after
// ShouldRejectNullMember.
// Sibling dual-wires (leave alone): ShouldRejectNullMember (3060),
// ShouldRejectNullItem (3067), ShouldSkipRareCheck (3094).
inline auto ShouldUpdatePoolForChar(const bool isDisappear) -> bool
{
    return !isDisappear;
}

// UpdatePoolPlan is the pure visibility disposition of CTreasurePool::UpdatePool
// before host warns or pushes trophy list packets.
struct UpdatePoolPlan
{
    bool reject;          // null char or pool mismatch
    bool pushTrophyLists; // !reject && status != DISAPPEAR
};

// PlanUpdatePool composes ShouldRejectNullMember and ShouldUpdatePoolForChar.
// Host keeps warning and packet push; plan is pure disposition only.
inline auto PlanUpdatePool(
    const bool charNull,
    const bool poolMismatch,
    const bool isDisappear) -> UpdatePoolPlan
{
    if (ShouldRejectNullMember(charNull, poolMismatch))
    {
        return UpdatePoolPlan{ true, false };
    }
    return UpdatePoolPlan{ false, ShouldUpdatePoolForChar(isDisappear) };
}

// ShouldFlushPool mirrors m_count != 0.
//
// Formula (slice 3127 dual-wire):
//   itemCount != 0
//
// itemCount — host-evaluated m_count (current filled treasure pool slots)
// true  → host may advance flush tick and checkTreasureItem each slot
// false → host no-ops flush when the pool is empty
//
// Dual-wire of Go treasurepool.ShouldFlushPool.
// Call site: PlanFlush / CTreasurePool::flush.
// Sibling dual-wires (leave alone): ShouldRejectNullMember (3060),
// ShouldRejectNullItem (3067), ShouldSkipRareCheck (3094),
// ShouldUpdatePoolForChar (3112).
inline auto ShouldFlushPool(const uint8 itemCount) -> bool
{
    return itemCount != 0;
}

// FlushPlan is the pure disposition of CTreasurePool::flush before the host
// advances tick (now + treasure_checktime + 1s) and checkTreasureItem each slot.
struct FlushPlan
{
    bool runChecks; // itemCount != 0
};

// PlanFlush wraps ShouldFlushPool for the flush entry gate.
// Host keeps timer math and checkTreasureItem loop; plan is pure disposition only.
inline auto PlanFlush(const uint8 itemCount) -> FlushPlan
{
    return FlushPlan{ ShouldFlushPool(itemCount) };
}

// SelectEvictionSlot: pure multi-pass selection over host-provided slot metadata.
// Returns freeSlot if already set; else scans passes; else default 0.
// For each slot index 0..count-1, host provides:
//   empty (ID==0), stamp, hasRare, hasExclusive, itemPresent (lookup non-null).
// This helper only evaluates the three eviction passes when freeSlot is unset.
struct EvictionSlotView
{
    bool  empty;
    bool  itemPresent;
    bool  hasRare;
    bool  hasExclusive;
    int64 stamp;
};

inline auto SelectFreeOrEvictionSlot(
    const uint8 freeSlotIfEmpty,
    const EvictionSlotView* slots,
    const uint8 slotCount) -> uint8
{
    if (!IsFreeSlotUnset(freeSlotIfEmpty))
    {
        return freeSlotIfEmpty;
    }
    if (slots == nullptr || slotCount == 0)
    {
        return DefaultFallbackSlot();
    }

    // Pass 1: oldest non-rare non-ex
    uint8 freeSlot = FreeSlotUnset;
    int64 oldest   = 0;
    bool  hasOld   = false;
    for (uint8 i = 0; i < slotCount; ++i)
    {
        const auto& s = slots[i];
        if (s.itemPresent && CanEvictNonRareNonExclusive(s.hasRare, s.hasExclusive) &&
            (!hasOld || PreferOlderTimestamp(s.stamp, oldest)))
        {
            freeSlot = i;
            oldest   = s.stamp;
            hasOld   = true;
        }
    }
    if (!IsFreeSlotUnset(freeSlot))
    {
        return freeSlot;
    }

    // Pass 2: oldest non-ex
    freeSlot = FreeSlotUnset;
    hasOld   = false;
    for (uint8 i = 0; i < slotCount; ++i)
    {
        const auto& s = slots[i];
        if (s.itemPresent && CanEvictNonExclusive(s.hasExclusive) &&
            (!hasOld || PreferOlderTimestamp(s.stamp, oldest)))
        {
            freeSlot = i;
            oldest   = s.stamp;
            hasOld   = true;
        }
    }
    if (!IsFreeSlotUnset(freeSlot))
    {
        return freeSlot;
    }

    // Pass 3: oldest any
    freeSlot = FreeSlotUnset;
    hasOld   = false;
    for (uint8 i = 0; i < slotCount; ++i)
    {
        const auto& s = slots[i];
        if (!hasOld || PreferOlderTimestamp(s.stamp, oldest))
        {
            freeSlot = i;
            oldest   = s.stamp;
            hasOld   = true;
        }
    }
    if (!IsFreeSlotUnset(freeSlot))
    {
        return freeSlot;
    }
    return DefaultFallbackSlot();
}

// FindFirstEmptySlot returns first empty index or FreeSlotUnset.
inline auto FindFirstEmptySlot(const bool* emptyFlags, const uint8 slotCount) -> uint8
{
    if (emptyFlags == nullptr)
    {
        return FreeSlotUnset;
    }
    for (uint8 i = 0; i < slotCount; ++i)
    {
        if (emptyFlags[i])
        {
            return i;
        }
    }
    return FreeSlotUnset;
}

} // namespace treasurepoolhelpers
