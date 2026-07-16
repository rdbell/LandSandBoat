#include "test_automaton_burden_decay_2710.h"

#include "map/entities/automaton_burden_decay.h"

#include <array>
#include <iostream>

auto runAutomatonBurdenDecay2710SelfTests() -> bool
{
    std::array<uint8, 8> normal{ 0, 1, 2, 10, 255, 3, 1, 100 };
    automatonburdenhelpers::Decay(normal, 0, 0);
    std::array<uint8, 8> positive{ 0, 1, 6, 10, 255, 3, 1, 100 };
    automatonburdenhelpers::Decay(positive, 2, 3); // 1 + 2 + 3 = 6
    std::array<uint8, 8> negativeOne{ 0, 1, 2, 10, 255, 3, 1, 100 };
    automatonburdenhelpers::Decay(negativeOne, -1, 0); // candidate zero clamps to one
    std::array<uint8, 8> negativeWrap{ 0, 1, 2, 10, 255, 3, 1, 100 };
    automatonburdenhelpers::Decay(negativeWrap, -2, 0); // candidate wraps to 255, then caps at burden

    const bool ok =
        normal == std::array<uint8, 8>{ 0, 0, 1, 9, 254, 2, 0, 99 } &&
        positive == std::array<uint8, 8>{ 0, 0, 0, 4, 249, 0, 0, 94 } &&
        negativeOne == std::array<uint8, 8>{ 0, 0, 1, 9, 254, 2, 0, 99 } &&
        negativeWrap == std::array<uint8, 8>{};
    if (!ok)
    {
        std::cerr << "automaton burden decay 2710 failed\n";
    }
    return ok;
}
