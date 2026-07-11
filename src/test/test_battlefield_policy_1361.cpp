#include "test_battlefield_policy_1361.h"

#include "map/battlefield_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "battlefield policy 1361 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "battlefield policy 1361 self-test failed: " << label << " got "
                  << static_cast<long long>(actual) << " expected " << static_cast<long long>(expected) << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runBattlefieldPolicy1361SelfTests() -> bool
{
    bool ok = true;

    // Insert gates
    ok = expect(battlefieldhelpers::ShouldRejectNullInsert(true), "null insert") && ok;
    ok = expect(battlefieldhelpers::ShouldRejectAlreadyInBattlefield(true), "already bf") && ok;
    ok = expect(battlefieldhelpers::ShouldAcceptPCUnderCapacity(2, 6), "under cap") && ok;
    ok = expect(!battlefieldhelpers::ShouldAcceptPCUnderCapacity(6, 6), "at cap") && ok;
    ok = expect(battlefieldhelpers::ShouldEnterPC(true), "enter") && ok;
    ok = expect(battlefieldhelpers::ShouldRegisterPC(false, false), "register") && ok;
    ok = expect(!battlefieldhelpers::ShouldRegisterPC(false, true), "already reg") && ok;
    ok = expect(!battlefieldhelpers::ShouldRegisterPC(true, false), "enter not reg") && ok;
    ok = expect(battlefieldhelpers::ShouldNPCDisappearAtStart(0x04), "npc disappear") && ok;
    ok = expect(!battlefieldhelpers::ShouldNPCDisappearAtStart(0x01), "npc normal") && ok;
    ok = expect(battlefieldhelpers::FormatInsertEntityNullWarning() ==
                    "CBattlefield::InsertEntity() - PEntity is null.",
                "null warn") &&
         ok;
    ok = expect(battlefieldhelpers::ShouldInsertPetWithPC(true, true), "pet insert") && ok;
    ok = expect(battlefieldhelpers::ShouldSendTimerPacket(true), "timer") && ok;

    // Level cap resolution
    ok = expect(battlefieldhelpers::ShouldApplyLevelCap(75), "apply cap") && ok;
    ok = expect(!battlefieldhelpers::ShouldApplyLevelCap(0), "no cap") && ok;
    ok = expectEq(battlefieldhelpers::ResolveLevelCap(75, 0, true, false, 99), static_cast<uint8>(75), "cap plain") && ok;
    ok = expectEq(battlefieldhelpers::ResolveLevelCap(75, 5, true, false, 99), static_cast<uint8>(80), "cap tweak") && ok;
    ok = expectEq(battlefieldhelpers::ResolveLevelCap(75, 0, false, true, 99), static_cast<uint8>(99), "mission uncap") && ok;
    ok = expectEq(battlefieldhelpers::ResolveLevelCap(75, 0, true, true, 99), static_cast<uint8>(75), "mission still cap") && ok;
    ok = expectEq(battlefieldhelpers::ResolveLevelCap(0, 5, false, true, 99), static_cast<uint8>(0), "zero no tweak") && ok;
    ok = expect(battlefieldhelpers::ShouldAddSjRestriction(0x00), "sj restrict") && ok;
    ok = expect(!battlefieldhelpers::ShouldAddSjRestriction(0x01), "sj allow") && ok;
    ok = expect(battlefieldhelpers::ShouldClearLevelRestriction(0), "clear lv") && ok;

    // Cleanup / leave
    ok = expect(battlefieldhelpers::CanCleanupResult(true, false), "cleanup sticky") && ok;
    ok = expect(battlefieldhelpers::CanCleanupResult(false, true), "cleanup empty") && ok;
    ok = expect(!battlefieldhelpers::CanCleanupResult(false, false), "no cleanup") && ok;
    ok = expect(battlefieldhelpers::ShouldSetCleanupSticky(true), "set sticky") && ok;
    ok = expect(battlefieldhelpers::ShouldDeferCleanup(false, false, true), "defer") && ok;
    ok = expect(!battlefieldhelpers::ShouldDeferCleanup(true, false, true), "force no defer") && ok;
    ok = expectEq(battlefieldhelpers::LeaveCodeFromStatus(2), static_cast<uint8>(2), "leave win") && ok;
    ok = expectEq(battlefieldhelpers::LeaveCodeFromStatus(3), static_cast<uint8>(4), "leave lose") && ok;
    ok = expect(battlefieldhelpers::ShouldSchedulePlayerCleanupDelay(false), "delay") && ok;
    ok = expect(!battlefieldhelpers::ShouldSchedulePlayerCleanupDelay(true), "no delay force") && ok;
    ok = expect(battlefieldhelpers::ShouldUpdateRecordOnWin(2, true), "record win") && ok;
    ok = expect(!battlefieldhelpers::ShouldUpdateRecordOnWin(2, false), "record worse") && ok;

    // Tick
    ok = expect(battlefieldhelpers::ShouldCheckInProgress(false), "check progress") && ok;
    ok = expect(battlefieldhelpers::ShouldAdvanceBattlefieldTick(true), "advance tick") && ok;
    ok = expect(battlefieldhelpers::ShouldHoldFightTick(1), "hold fight") && ok;
    ok = expect(!battlefieldhelpers::ShouldHoldFightTick(0), "update fight") && ok;
    ok = expect(battlefieldhelpers::ShouldCaptureFinishTime(2), "finish won") && ok;
    ok = expect(battlefieldhelpers::ShouldCaptureFinishTime(3), "finish lost") && ok;
    ok = expect(!battlefieldhelpers::ShouldCaptureFinishTime(1), "no finish locked") && ok;

    // Occupancy
    ok = expect(battlefieldhelpers::IsOccupied(false), "occupied") && ok;
    ok = expect(battlefieldhelpers::IsEntered(true), "entered") && ok;
    ok = expect(battlefieldhelpers::IsRegistered(true), "registered") && ok;

    // Cleanup mobs
    ok = expect(battlefieldhelpers::ShouldClearNoDespawnBehavior(true), "clear nodespawn") && ok;
    ok = expect(battlefieldhelpers::ShouldDespawnMobOnCleanup(true, true), "despawn") && ok;
    ok = expect(!battlefieldhelpers::ShouldDespawnMobOnCleanup(false, true), "dead no despawn") && ok;

    // Constants
    ok = expectEq(battlefieldhelpers::RulesAllowSubjobs, static_cast<uint16>(0x01), "rules sj") && ok;
    ok = expectEq(battlefieldhelpers::StatusWon, static_cast<uint8>(2), "status won") && ok;
    ok = expectEq(battlefieldhelpers::LeaveCodeWin, static_cast<uint8>(2), "leave win const") && ok;

    return ok;
}
