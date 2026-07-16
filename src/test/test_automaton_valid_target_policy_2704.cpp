#include "test_automaton_valid_target_policy_2704.h"

#include "map/entities/automaton_valid_target_policy.h"

#include <iostream>

auto runAutomatonValidTargetPolicy2704SelfTests() -> bool
{
    int petCalls = 0;
    const auto pet = [&](const bool result) { return [&, result] { ++petCalls; return result; }; };
    const bool ok =
        automatonvalidtargethelpers::ValidTarget(true, true, pet(false)) &&
        petCalls == 0 &&
        !automatonvalidtargethelpers::ValidTarget(true, false, pet(false)) &&
        petCalls == 1 &&
        automatonvalidtargethelpers::ValidTarget(false, true, pet(true)) &&
        petCalls == 2;
    if (!ok)
    {
        std::cerr << "automaton valid target policy 2704 failed\n";
    }
    return ok;
}
