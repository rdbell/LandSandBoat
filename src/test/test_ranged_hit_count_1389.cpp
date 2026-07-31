#include "test_ranged_hit_count_1389.h"

#include "map/ranged_hit_count_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "ranged hit count 1389 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "ranged hit count 1389 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runRangedHitCount1389SelfTests() -> bool
{
    using namespace rangedhitcounthelpers;
    bool ok = true;

    ok = expectEq(ClampUtsusemiForSange(10), static_cast<uint8>(7), "clamp high") && ok;
    ok = expectEq(ClampUtsusemiForSange(-1), static_cast<uint8>(0), "clamp low") && ok;
    ok = expectEq(ClampUtsusemiForSange(3), static_cast<uint8>(3), "clamp mid") && ok;

    ok = expectEq(CapHitCountByAmmoQuantity(5, true, 3), static_cast<uint8>(3), "cap ammo") && ok;
    ok = expectEq(CapHitCountByAmmoQuantity(5, true, 10), static_cast<uint8>(5), "no cap") && ok;
    ok = expectEq(CapHitCountByAmmoQuantity(5, false, 1), static_cast<uint8>(5), "no ammo") && ok;

    ok = expect(MultiShotProcs(25, 24) && !MultiShotProcs(25, 25), "multi proc") && ok;

    ok = expect(ShouldApplyCharBarrage(true, false, false, true) && !ShouldApplyCharBarrage(true, true, false, true), "char barrage") && ok;
    ok = expect(ShouldApplyNonCharBarrage(false, true) && !ShouldApplyNonCharBarrage(true, true), "nonchar barrage") && ok;
    ok = expect(ShouldApplySange(true, true, true) && !ShouldApplySange(true, false, true), "sange") && ok;
    ok = expect(IsSangeActive(true, 1) && !IsSangeActive(true, 0) && !IsSangeActive(false, 5), "sange active") && ok;
    ok = expect(ShouldTryTripleShot(true, false, true) && ShouldTryTripleShot(false, true, true), "triple try") && ok;
    ok = expect(!ShouldTryTripleShot(false, false, true), "no triple") && ok;

    // Char barrage: base 1 + 3 extras = 4
    ok = expectEq(
        ResolveRangedHitCountWithEffects(true, false, false, false, true, 3, false, 0, false, 0, false, false, false, false),
        static_cast<uint8>(4),
        "char barrage hits") && ok;
    // Non-char barrage
    ok = expectEq(
        ResolveRangedHitCountWithEffects(false, true, false, false, true, 5, false, 0, false, 0, false, false, false, false),
        static_cast<uint8>(6),
        "trust barrage") && ok;
    // Sange with 3 shadows, ammo 10 → 4
    ok = expectEq(
        ResolveRangedHitCountWithEffects(true, false, true, false, false, 0, true, 3, true, 10, false, false, false, false),
        static_cast<uint8>(4),
        "sange hits") && ok;
    // Sange capped by ammo quantity 2 → 2
    ok = expectEq(
        ResolveRangedHitCountWithEffects(true, false, true, false, false, 0, true, 5, true, 2, false, false, false, false),
        static_cast<uint8>(2),
        "sange ammo cap") && ok;
    // Triple shot
    ok = expectEq(
        ResolveRangedHitCountWithEffects(true, false, false, false, false, 0, false, 0, false, 0, true, true, true, true),
        static_cast<uint8>(3),
        "triple") && ok;
    // Double shot (no triple)
    ok = expectEq(
        ResolveRangedHitCountWithEffects(true, false, false, false, false, 0, false, 0, false, 0, false, false, true, true),
        static_cast<uint8>(2),
        "double") && ok;
    // Base
    ok = expectEq(
        ResolveRangedHitCountWithEffects(true, false, false, false, false, 0, false, 0, false, 0, false, false, false, false),
        static_cast<uint8>(1),
        "base") && ok;
    // Barrage beats triple (exclusive ladder)
    ok = expectEq(
        ResolveRangedHitCountWithEffects(true, false, false, false, true, 2, false, 0, false, 0, true, true, true, true),
        static_cast<uint8>(3),
        "barrage over triple") && ok;

    // The host-facing overload receives already-folded triple/double proc
    // results, so it must preserve the same exclusive ladder without the
    // separate effect-presence flags.
    ok = expectEq(
        ResolveRangedHitCount(true, false, false, false, true, 3, false, 0, false, 0, false, false),
        static_cast<uint8>(4),
        "simple char barrage") && ok;
    ok = expectEq(
        ResolveRangedHitCount(true, false, true, false, false, 0, true, 5, true, 2, false, false),
        static_cast<uint8>(2),
        "simple sange ammo cap") && ok;
    ok = expectEq(
        ResolveRangedHitCount(true, false, false, false, false, 0, false, 0, false, 0, true, false),
        static_cast<uint8>(3),
        "simple triple") && ok;
    ok = expectEq(
        ResolveRangedHitCount(true, false, false, false, false, 0, false, 0, false, 0, false, true),
        static_cast<uint8>(2),
        "simple double") && ok;
    ok = expectEq(
        ResolveRangedHitCount(true, false, false, false, true, 2, false, 0, false, 0, true, true),
        static_cast<uint8>(3),
        "simple barrage over multi-shot") && ok;

    ok = expectEq(ResolveRangedWeaponSlot(true), SlotAmmo, "slot ammo") && ok;
    ok = expectEq(ResolveRangedWeaponSlot(false), SlotRanged, "slot ranged") && ok;
    ok = expect(ShouldNullRangedWeaponOnAmmoThrow(true) && !ShouldNullRangedWeaponOnAmmoThrow(false), "null item") && ok;
    ok = expect(ShouldNullAmmoOnRangedThrow(true) && !ShouldNullAmmoOnRangedThrow(false), "null ammo") && ok;

    ok = expectEq(BaseRangedHitCount, static_cast<uint8>(1), "base pin") && ok;
    ok = expectEq(TripleShotHitCount, static_cast<uint8>(3), "triple pin") && ok;
    ok = expectEq(DoubleShotHitCount, static_cast<uint8>(2), "double pin") && ok;
    ok = expectEq(SlotRanged, static_cast<uint8>(2), "slot r") && ok;
    ok = expectEq(SlotAmmo, static_cast<uint8>(3), "slot a") && ok;

    return ok;
}
