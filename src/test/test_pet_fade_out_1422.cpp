#include "test_pet_fade_out_1422.h"

#include "map/pet_fade_out_capacity.h"

#include <iostream>
#include <vector>

auto runPetFadeOut1422SelfTests() -> bool
{
    std::vector<int> calls;
    petfadeouthelpers::Apply(
        [&]() { calls.push_back(1); },
        [&]() { calls.push_back(2); });

    const bool ok = calls == std::vector<int>{ 1, 2 };
    if (!ok)
    {
        std::cerr << "pet fade out 1422 self-test failed\n";
    }
    return ok;
}
