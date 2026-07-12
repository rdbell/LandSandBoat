#include "test_perpetuation_1613.h"

#include "map/perpetuation_capacity.h"

#include <iostream>

namespace
{
using namespace perpetuationhelpers;

auto Check() -> bool
{
    // Spirits
    if (PerpetuationCost(0, 1) != 1 || PerpetuationCost(7, 19) != 2 || PerpetuationCost(7, 91) != 7)
    {
        return false;
    }
    // Carbuncle / Cait
    if (PerpetuationCost(PetIDCarbuncle, 9) != 1 || PerpetuationCost(PetIDCaitSith, 91) != 11)
    {
        return false;
    }
    // Fenrir not on avatar ladder
    if (PerpetuationCost(PetIDFenrir, 1) != 1 || PerpetuationCost(PetIDFenrir, 99) != 13)
    {
        return false;
    }
    // Elemental avatars + Siren
    if (PerpetuationCost(10, 1) != 3 || PerpetuationCost(PetIDDiabolos, 99) != 15 || PerpetuationCost(PetIDSiren, 75) != 13)
    {
        return false;
    }
    // Zero IDs: Odin / jug / wyvern
    if (PerpetuationCost(18, 99) != 0 || PerpetuationCost(21, 50) != 0 || PerpetuationCost(48, 75) != 0)
    {
        return false;
    }
    return true;
}
} // namespace

auto runPerpetuation1613SelfTests() -> bool
{
    if (!Check())
    {
        std::cerr << "perpetuation_1613 self-tests failed\n";
        return false;
    }
    return true;
}
