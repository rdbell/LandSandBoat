#include "test_data_a1_creation_slot_2675.h"

#include "login/data_a1_capacity.h"

#include <array>
#include <iostream>

auto runDataA1CreationSlot2675SelfTests() -> bool
{
    struct case_t { uint32 visible; uint32 content; bool expected; };
    constexpr auto cases = std::array{ case_t{ 0, 0, false }, case_t{ 0, 1, true }, case_t{ 1, 1, false }, case_t{ 15, 16, true }, case_t{ 16, 15, false } };
    for (const auto& test : cases)
    {
        if (loginHelpers::CanAddCharacterCreationSlot(test.visible, test.content) != test.expected)
        {
            std::cerr << "data A1 creation slot 2675 self-test failed\n";
            return false;
        }
    }
    return true;
}
