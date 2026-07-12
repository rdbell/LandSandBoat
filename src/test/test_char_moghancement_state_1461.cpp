#include "test_char_moghancement_state_1461.h"

#include "map/char_moghancement_state_capacity.h"

#include <cstdint>
#include <iostream>
#include <vector>

auto runCharMoghancementState1461SelfTests() -> bool
{
    bool ok = charmoghancementstatehelpers::Has(std::uint16_t{ 123 }, std::uint16_t{ 123 }) &&
              !charmoghancementstatehelpers::Has(std::uint16_t{ 123 }, std::uint16_t{ 124 });

    std::uint16_t current = 100;
    std::vector<int> calls{};
    charmoghancementstatehelpers::Set(
        current,
        std::uint16_t{ 200 },
        [&](const std::uint16_t id, const bool adding)
        {
            calls.push_back(current == 100 ? static_cast<int>(id) * (adding ? 1 : -1) : 999);
        });
    ok = current == 200 && calls == std::vector<int>{ -100, 200 } && ok;

    calls.clear();
    charmoghancementstatehelpers::Set(
        current,
        std::uint16_t{ 200 },
        [&](const std::uint16_t id, const bool adding)
        {
            calls.push_back(current == 200 ? static_cast<int>(id) * (adding ? 1 : -1) : 999);
        });
    ok = current == 200 && calls == std::vector<int>{ -200, 200 } && ok;

    calls.clear();
    charmoghancementstatehelpers::Set(
        current,
        std::uint16_t{ 0 },
        [&](const std::uint16_t id, const bool adding)
        {
            calls.push_back(current == 200 ? static_cast<int>(id) * (adding ? 1 : -1) : 999);
        });
    ok = current == 0 && calls == std::vector<int>{ -200, 0 } && ok;

    if (!ok)
    {
        std::cerr << "char moghancement state 1461 self-test failed\n";
    }
    return ok;
}
