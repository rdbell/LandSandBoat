#include "test_pet_ability_result_1432.h"

#include "map/pet_ability_capacity.h"

#include <iostream>
#include <vector>

auto runPetAbilityResult1432SelfTests() -> bool
{
    std::vector<int> calls;
    const auto finalize = [&](int previous, int current, int ability, int value)
    {
        return petabilityhelpers::FinalizeResult(
            previous, current, ability, 0, 100, value,
            [&](int message) { calls.push_back(message); return message + 1000; });
    };

    auto result = finalize(7, 7, 55, 12);
    bool ok = result.message == 55 && result.param == 12 && calls.empty();
    result  = finalize(7, 7, 0, 12);
    ok = ok && result.message == 100 && result.param == 12 && calls.empty();
    result = finalize(7, 44, 55, 12);
    ok = ok && result.message == 44 && result.param == 12 && calls.empty();
    result = finalize(7, 44, 55, -12);
    ok = ok && result.message == 1044 && result.param == 12 && calls == std::vector<int>{ 44 };
    if (!ok)
    {
        std::cerr << "pet ability result 1432 self-test failed\n";
    }
    return ok;
}
