#include "test_char_death_plan_1479.h"

#include "map/char_death_plan_capacity.h"

#include <iostream>

auto runCharDeathPlan1479SelfTests() -> bool
{
    using M = chardeathplanhelpers::Message;
    const auto empty = chardeathplanhelpers::Build({ .expLossLevel = 1 });
    bool ok = empty.message == M::FallsToGround && !empty.despawnPet && !empty.loseEXP;
    const auto attacker = chardeathplanhelpers::Build({ .lastAttackerMatches = true, .hasPet = true, .expLossLevel = 1 });
    ok = attacker.message == M::DefeatedBy && attacker.despawnPet && ok;
    const auto eligible = chardeathplanhelpers::Build({ .mainLevel = 10, .expLossLevel = 10, .experienceRetainedMod = 25 });
    ok = eligible.loseEXP && eligible.retainPercent == 0.25F && ok;
    const auto battlefield = chardeathplanhelpers::Build({ .hasBattlefield = true, .battlefieldLosesEXP = true, .mainLevel = 10, .expLossLevel = 10 });
    ok = battlefield.loseEXP && ok;
    const auto blocked = chardeathplanhelpers::Build({ .hasBattlefield = true, .mainLevel = 10, .expLossLevel = 10 });
    ok = !blocked.loseEXP && ok;
    const auto mijin = chardeathplanhelpers::Build({ .mijinGakure = true, .mainLevel = 10, .expLossLevel = 10 });
    ok = !mijin.loseEXP && ok;
    const auto floor = chardeathplanhelpers::Build({ .mainLevel = 10, .expLossLevel = 10, .experienceRetainedMod = -200 });
    const auto cap = chardeathplanhelpers::Build({ .mainLevel = 10, .expLossLevel = 10, .expRetain = 1, .experienceRetainedMod = 200 });
    ok = floor.retainPercent == 0.0F && cap.retainPercent == 1.0F && ok;
    if (!ok)
    {
        std::cerr << "char death plan 1479 self-test failed\n";
    }
    return ok;
}
