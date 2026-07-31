#include "test_char_gender_1490.h"

#include "map/char_gender_capacity.h"

#include <iostream>

namespace
{
using chargenderhelpers::FromRace;

auto Check() -> bool
{
    // Hume M/F, Elvaan M/F, Taru M/F
    if (FromRace(1) != 1 || FromRace(2) != 0 || FromRace(3) != 1 || FromRace(4) != 0 || FromRace(5) != 1 || FromRace(6) != 0)
    {
        return false;
    }
    // Mithra female (7→0), Galka male (8→1) after flip
    if (FromRace(7) != 0 || FromRace(8) != 1)
    {
        return false;
    }
    // Race 0: (0%2)^(0>6) = 0; uint8 upper bound still flips parity.
    if (FromRace(0) != 0 || FromRace(255) != 0)
    {
        return false;
    }
    return true;
}
} // namespace

auto runCharGender1490SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "char gender 1490 self-test failed\n";
    }
    return ok;
}
