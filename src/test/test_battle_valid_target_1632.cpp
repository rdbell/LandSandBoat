#include "test_battle_valid_target_1632.h"

#include "map/battle_valid_target_capacity.h"

#include <iostream>

auto runBattleValidTarget1632SelfTests() -> bool
{
    using namespace battlevalidtargethelpers;

    bool ok = true;

    ok = ok && AllegiancePlayer == 1 && AllegianceWyverns == 5;
    ok = ok && TargetSelf == 0x0001 && TargetEnemy == 0x0004;
    ok = ok && PetTypeAutomaton == 4 && TypePet == 0x08;

    ok = ok && IsTeamPVPAllegiance(AllegianceWyverns);
    ok = ok && !IsTeamPVPAllegiance(AllegiancePlayer);
    ok = ok && IsNationPVPAllegiance(AllegianceSanDoria);
    ok = ok && IsNationPVPAllegiance(AllegianceWindurst);
    ok = ok && !IsNationPVPAllegiance(AllegiancePlayer);
    ok = ok && IsPVEAllegiance(AllegiancePlayer);
    ok = ok && !IsPVEAllegiance(AllegianceSanDoria);

    // Team PVP different allegiances
    ok = ok && ResolveEnemyValidTarget(AllegianceWyverns, AllegianceGriffons, false, false);
    ok = ok && !ResolveEnemyValidTarget(AllegianceWyverns, AllegianceWyverns, false, false);

    // Nation PVP
    ok = ok && ResolveEnemyValidTarget(AllegianceSanDoria, AllegianceBastok, false, false);
    ok = ok && !ResolveEnemyValidTarget(AllegianceSanDoria, AllegianceSanDoria, false, false);

    // PVE different allegiances
    ok = ok && ResolveEnemyValidTarget(AllegianceMob, AllegiancePlayer, false, false);
    // PVE same allegiance without skip
    ok = ok && !ResolveEnemyValidTarget(AllegianceMob, AllegianceMob, true, false);
    // PVE same with skip allegiance
    ok = ok && ResolveEnemyValidTarget(AllegianceMob, AllegianceMob, true, true);
    // PVE same skip but initiator not mob
    ok = ok && !ResolveEnemyValidTarget(AllegianceMob, AllegianceMob, false, true);

    ok = ok && ResolveSelfValidTarget(true, false);
    ok = ok && ResolveSelfValidTarget(false, true);
    ok = ok && !ResolveSelfValidTarget(false, false);

    ok = ok && IsAutomatonMasterTarget(true, PetTypeAutomaton, true);
    ok = ok && !IsAutomatonMasterTarget(true, 0, true);
    ok = ok && !IsAutomatonMasterTarget(false, PetTypeAutomaton, true);

    // Full tree: enemy dead without SELF flag falls through and fails
    ok = ok && !ResolveValidTarget(TargetEnemy, true, AllegianceMob, AllegiancePlayer, false, false, true, false);
    // Full tree: enemy dead with SELF flag uses self path
    ok = ok && ResolveValidTarget(static_cast<uint16>(TargetEnemy | TargetSelf), true, AllegianceMob, AllegiancePlayer, false, false, true, false);
    // Full tree: enemy alive PVE
    ok = ok && ResolveValidTarget(TargetEnemy, false, AllegianceMob, AllegiancePlayer, false, false, false, false);
    // Full tree: self only
    ok = ok && ResolveValidTarget(TargetSelf, false, AllegiancePlayer, AllegiancePlayer, false, false, true, false);
    // Full tree: no flags
    ok = ok && !ResolveValidTarget(0, false, AllegiancePlayer, AllegiancePlayer, false, false, true, false);

    if (!ok)
    {
        std::cerr << "battle valid target 1632 self-test failed\n";
    }
    return ok;
}
