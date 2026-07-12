#include "test_char_moghancement_update_1463.h"

#include "map/char_moghancement_update_capacity.h"

#include <iostream>
#include <vector>

namespace
{
auto RunUpdate(std::uint16_t newID, std::uint16_t currentID) -> std::vector<int>
{
    std::vector<int> calls{};
    charmoghancementupdatehelpers::Update(
        newID,
        currentID,
        [&](const std::uint16_t id) { calls.push_back(10000 + id); },
        [&](const std::uint16_t id) { calls.push_back(-static_cast<int>(id)); },
        [&](const std::uint16_t id) { calls.push_back(id); },
        [&](const std::uint8_t table) { calls.push_back(20000 + table); },
        [&]() { calls.push_back(30000); },
        [&](const std::uint16_t id) { calls.push_back(40000 + id); },
        [&]() { calls.push_back(50000); });
    return calls;
}
} // namespace

auto runCharMoghancementUpdate1463SelfTests() -> bool
{
    bool ok = RunUpdate(0, 0).empty();
    ok = RunUpdate(514, 514) == std::vector<int>{ 10514 } && ok;
    ok = RunUpdate(514, 513) == std::vector<int>{ 10514, -513, 514, 20001, 30000, 40514, 50000 } && ok;
    ok = RunUpdate(1025, 513) == std::vector<int>{ 11025, -513, 1025, 20002, 20001, 30000, 41025, 50000 } && ok;
    ok = RunUpdate(2, 1) == std::vector<int>{ 10002, -1, 2, 20000, 30000, 40002, 50000 } && ok;
    ok = RunUpdate(0, 513) == std::vector<int>{ -513, 20001, 30000, 40000, 50000 } && ok;

    if (!ok)
    {
        std::cerr << "char moghancement update 1463 self-test failed\n";
    }
    return ok;
}
