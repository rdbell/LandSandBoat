#include "test_ranged_ammo_1390.h"

#include "map/ranged_ammo_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "ranged ammo 1390 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "ranged ammo 1390 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runRangedAmmo1390SelfTests() -> bool
{
    using namespace rangedammohelpers;
    bool ok = true;

    ok = expectEq(ResolveRecycleChance(10, false, 5, 3), static_cast<int16>(13), "recycle no trait") && ok;
    ok = expectEq(ResolveRecycleChance(10, true, 5, 3), static_cast<int16>(18), "recycle trait") && ok;
    ok = expectEq(ApplyUnlimitedShotToRecycleChance(18, true), static_cast<int16>(100), "unlimited") && ok;
    ok = expectEq(ApplyUnlimitedShotToRecycleChance(18, false), static_cast<int16>(18), "no unlimited") && ok;

    ok = expect(ShouldConsumeAmmo(true, 25, 26) && !ShouldConsumeAmmo(true, 25, 25), "consume edge") && ok;
    ok = expect(!ShouldConsumeAmmo(false, 0, 99), "no ammo") && ok;
    ok = expect(!ShouldConsumeAmmo(true, 0, 0), "roll 0 not > 0") && ok;
    ok = expect(ShouldConsumeAmmo(true, 0, 1), "consume roll1") && ok;

    ok = expect(ShouldDeleteUnlimitedShot(true, true, 5), "del hit") && ok;
    ok = expect(ShouldDeleteUnlimitedShot(true, false, 0), "del retain0") && ok;
    ok = expect(!ShouldDeleteUnlimitedShot(true, false, 1), "retain") && ok;
    ok = expect(!ShouldDeleteUnlimitedShot(false, true, 0), "no us") && ok;

    ok = expect(ShouldTruncateHitCountOnAmmoDeplete(true, 3, 3), "truncate") && ok;
    ok = expect(!ShouldTruncateHitCountOnAmmoDeplete(true, 5, 3), "no truncate") && ok;
    ok = expect(!ShouldTruncateHitCountOnAmmoDeplete(false, 3, 3), "not consuming") && ok;

    ok = expect(RemoveAmmoEmptiesSlot(true, 1, 1) && RemoveAmmoEmptiesSlot(true, 2, 3), "empty") && ok;
    ok = expect(!RemoveAmmoEmptiesSlot(true, 5, 2), "not empty") && ok;
    ok = expect(!RemoveAmmoEmptiesSlot(false, 0, 1), "no item") && ok;
    ok = expect(RemoveAmmoShouldAct(true) && !RemoveAmmoShouldAct(false), "should act") && ok;

    ok = expectEq(ResolveRangedDistanceMessage(0), RangedDistanceMsg::Pummels, "pummels") && ok;
    ok = expectEq(ResolveRangedDistanceMessage(15), RangedDistanceMsg::Squarely, "squarely edge") && ok;
    ok = expectEq(ResolveRangedDistanceMessage(16), RangedDistanceMsg::Hit, "hit") && ok;
    ok = expectEq(RangedDistanceMsgID(RangedDistanceMsg::Pummels), MsgRangedAttackPummels, "id pummels") && ok;
    ok = expectEq(RangedDistanceMsgID(RangedDistanceMsg::Squarely), MsgRangedAttackSquarely, "id square") && ok;
    ok = expectEq(RangedDistanceMsgID(RangedDistanceMsg::Hit), MsgRangedAttackHit, "id hit") && ok;

    ok = expect(ShouldApplyDistancePenaltyMessage(true, false) && !ShouldApplyDistancePenaltyMessage(true, true), "dist msg") && ok;
    ok = expect(ShouldForceBarrageSangeHitResolution(true, false, true, false), "force hit") && ok;
    ok = expect(!ShouldForceBarrageSangeHitResolution(true, true, true, false), "already hit") && ok;

    ok = expect(ShouldApplyRangedDamageMultiplier(true, true) && !ShouldApplyRangedDamageMultiplier(true, false), "mult") && ok;
    ok = expectEq(ResolveRangedPhysicalAttackType(true), AttackTypeRapidShot, "rapid") && ok;
    ok = expectEq(ResolveRangedPhysicalAttackType(false), AttackTypeRanged, "ranged") && ok;

    ok = expect(ShouldDeleteBarrageStatus(true) && ShouldDeleteSangeStatus(true), "del status") && ok;
    ok = expect(ShouldUseSangeDisplayMessage(true) && !ShouldUseSangeDisplayMessage(false), "sange msg") && ok;

    ok = expectEq(MsgRangedAttackHit, static_cast<uint16>(352), "pin hit") && ok;
    ok = expectEq(MsgRangedAttackCrit, static_cast<uint16>(353), "pin crit") && ok;
    ok = expectEq(MsgRangedAttackSquarely, static_cast<uint16>(576), "pin square") && ok;
    ok = expectEq(MsgRangedAttackPummels, static_cast<uint16>(577), "pin pummels") && ok;
    ok = expectEq(MsgUsesSangeTakesDamage, static_cast<uint16>(77), "pin sange") && ok;
    ok = expectEq(MsgUsesBarrageTakesDamage, static_cast<uint16>(157), "pin barrage") && ok;

    return ok;
}
