#include "test_char_automaton_1435.h"

#include "map/char_automaton_capacity.h"

#include <array>
#include <iostream>

auto runCharAutomaton1435SelfTests() -> bool
{
    uint8 bonus = 2;
    std::array<uint8, 8> capacities{ 10, 11, 12, 13, 14, 15, 16, 17 };

    bool ok = !charautomatonhelpers::ApplyElementalCapacityBonus(2, bonus, capacities) &&
              capacities == std::array<uint8, 8>{ 10, 11, 12, 13, 14, 15, 16, 17 };
    ok = charautomatonhelpers::ApplyElementalCapacityBonus(5, bonus, capacities) && ok;
    ok = bonus == 5 && capacities == std::array<uint8, 8>{ 13, 14, 15, 16, 17, 18, 19, 20 } && ok;
    ok = !charautomatonhelpers::ApplyElementalCapacityBonus(5, bonus, capacities) &&
         capacities == std::array<uint8, 8>{ 13, 14, 15, 16, 17, 18, 19, 20 } && ok;
    ok = charautomatonhelpers::ApplyElementalCapacityBonus(1, bonus, capacities) && ok;
    ok = bonus == 1 && capacities == std::array<uint8, 8>{ 9, 10, 11, 12, 13, 14, 15, 16 } && ok;

    std::array<uint8, 3> wrapped{ 0, 1, 255 };
    uint8 wrappedBonus = 1;
    ok = charautomatonhelpers::ApplyElementalCapacityBonus(0, wrappedBonus, wrapped) && ok;
    ok = wrapped == std::array<uint8, 3>{ 255, 0, 254 } && ok;

    const std::array<uint8, 12> attachments{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 255 };
    ok = charautomatonhelpers::HasAttachment(attachments, 0) && ok;
    ok = charautomatonhelpers::HasAttachment(attachments, 255) && ok;
    ok = !charautomatonhelpers::HasAttachment(attachments, 42) && ok;

    if (!ok)
    {
        std::cerr << "char automaton 1435 self-test failed\n";
    }
    return ok;
}
