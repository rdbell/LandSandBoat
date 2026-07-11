#include "test_pet_jug_timer_1429.h"

#include "map/pet_jug_timer_capacity.h"

#include <iostream>
#include <vector>

auto runPetJugTimer1429SelfTests() -> bool
{
    std::vector<int> calls;
    bool ok = petjugtimerhelpers::GetSpawnTime(true, 7, [&]() { calls.push_back(1); }) == 7 && calls.empty();
    calls.clear();
    ok = ok && petjugtimerhelpers::GetSpawnTime(false, 7, [&]() { calls.push_back(1); }) == 7 && calls == std::vector<int>{ 1 };
    calls.clear();
    petjugtimerhelpers::SetSpawnTime(true, 8, [&]() { calls.push_back(1); }, [&](int value) { calls.push_back(value); });
    ok = ok && calls == std::vector<int>{ 8 };
    calls.clear();
    petjugtimerhelpers::SetSpawnTime(false, 8, [&]() { calls.push_back(1); }, [&](int value) { calls.push_back(value); });
    ok = ok && calls == std::vector<int>{ 1 };
    calls.clear();
    ok = ok && petjugtimerhelpers::GetDuration(true, 9, [&]() { calls.push_back(1); }) == 9 && calls.empty();
    calls.clear();
    ok = ok && petjugtimerhelpers::GetDuration(false, 9, [&]() { calls.push_back(1); }) == 0 && calls == std::vector<int>{ 1 };
    calls.clear();
    petjugtimerhelpers::SetDuration(true, 10, [&]() { calls.push_back(1); }, [&](int value) { calls.push_back(value); });
    ok = ok && calls == std::vector<int>{ 10 };
    calls.clear();
    petjugtimerhelpers::SetDuration(false, 10, [&]() { calls.push_back(1); }, [&](int value) { calls.push_back(value); });
    ok = ok && calls == std::vector<int>{ 1 };
    if (!ok)
    {
        std::cerr << "pet jug timer 1429 self-test failed\n";
    }
    return ok;
}
