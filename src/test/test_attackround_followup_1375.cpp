#include "test_attackround_followup_1375.h"

#include "map/attackround_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "attackround followup 1375 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "attackround followup 1375 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runAttackRoundFollowUp1375SelfTests() -> bool
{
    using namespace attackroundhelpers;
    bool ok = true;

    // Kick
    ok = expect(ShouldCreateKickAttacks(true) && !ShouldCreateKickAttacks(false), "kick h2h") && ok;
    ok = expect(ShouldAddMNKKickMerit(true, true) && !ShouldAddMNKKickMerit(true, false), "mnk merit") && ok;
    ok = expectEq(ClampKickAttackRate(150), static_cast<uint16>(100), "kick clamp") && ok;
    ok = expectEq(ClampKickAttackRate(40), static_cast<uint16>(40), "kick under") && ok;
    ok = expect(ShouldProcKickAttack(true) && !ShouldProcKickAttack(false), "kick proc") && ok;
    ok = expect(ShouldProcExtraKick(true, true) && !ShouldProcExtraKick(false, true), "extra kick") && ok;

    // Daken
    ok = expect(ShouldCreateDakenAttack(true) && !ShouldCreateDakenAttack(false), "daken pc") && ok;
    ok = expect(ShouldProcDakenThrow(true, true) && !ShouldProcDakenThrow(false, true), "daken throw") && ok;

    // Follow-up eligibility
    ok = expect(IsAmmoSwingEligibleAttackType(AttackTypeNormal), "elig normal") && ok;
    ok = expect(IsAmmoSwingEligibleAttackType(AttackTypeDouble), "elig double") && ok;
    ok = expect(IsAmmoSwingEligibleAttackType(AttackTypeTriple), "elig triple") && ok;
    ok = expect(IsAmmoSwingEligibleAttackType(AttackTypeSamba), "elig samba") && ok;
    ok = expect(IsAmmoSwingEligibleAttackType(AttackTypeQuad), "elig quad") && ok;
    ok = expect(!IsAmmoSwingEligibleAttackType(AttackTypeKick), "elig kick no") && ok;
    ok = expect(!IsAmmoSwingEligibleAttackType(AttackTypeDaken), "elig daken no") && ok;
    ok = expect(!IsAmmoSwingEligibleAttackType(AttackTypeFollowUp), "elig follow no") && ok;
    ok = expect(IsAttackTypeEligibleForFollowUp(true, AttackTypeNormal), "eligible") && ok;
    ok = expect(!IsAttackTypeEligibleForFollowUp(false, AttackTypeNormal), "no mod") && ok;
    ok = expect(!IsAttackTypeEligibleForFollowUp(true, AttackTypeKick), "kick not elig") && ok;

    // Weapon selection / ammo
    ok = expect(ShouldUseMainWeaponForFollowUp(true, false), "main h2h") && ok;
    ok = expect(ShouldUseMainWeaponForFollowUp(false, true), "main right") && ok;
    ok = expect(ShouldUseSubWeaponForFollowUp(true) && !ShouldUseSubWeaponForFollowUp(false), "sub") && ok;
    ok = expect(ShouldProcAmmoSwing(true, true) && !ShouldProcAmmoSwing(false, true), "ammo swing") && ok;
    ok = expect(IsVirtueStoneAmmo(VirtueStoneItemID, 1) && !IsVirtueStoneAmmo(1, 1), "virtue") && ok;
    ok = expect(!IsVirtueStoneAmmo(VirtueStoneItemID, 0), "virtue empty") && ok;
    ok = expect(ShouldUnequipAmmoAfterConsume(1) && !ShouldUnequipAmmoAfterConsume(2), "unequip") && ok;

    // Store / append
    ok = expect(CanStoreFollowUpSwing(0, true, false), "store empty") && ok;
    ok = expect(CanStoreFollowUpSwing(1, false, true), "store differ") && ok;
    ok = expect(!CanStoreFollowUpSwing(1, false, false), "store same dir") && ok;
    ok = expect(!CanStoreFollowUpSwing(2, false, true), "store full") && ok;
    ok = expect(!CanStoreFollowUpSwing(-1, true, false), "negative count wraps full") && ok;
    ok = expect(ShouldAppendStoredFollowUps(true) && !ShouldAppendStoredFollowUps(false), "append") && ok;
    ok = expect(ShouldProcFollowUpForChar(true, true) && !ShouldProcFollowUpForChar(false, true), "char follow") && ok;

    return ok;
}
