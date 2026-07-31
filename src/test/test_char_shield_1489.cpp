#include "test_char_shield_1489.h"

#include "map/char_shield_capacity.h"

#include <iostream>

namespace
{
using charshieldhelpers::ShieldDefense;
using charshieldhelpers::ShieldSize;

auto Check() -> bool
{
    if (ShieldSize(false, true, 3) != 0 || ShieldSize(true, false, 3) != 0 || ShieldSize(true, true, 3) != 3)
    {
        return false;
    }
    if (ShieldDefense(false, true, 12) != 0 || ShieldDefense(true, false, 12) != 0 || ShieldDefense(true, true, 12) != 12)
    {
        return false;
    }
    // Size path requires both presence and IsShield; defense uses has && isShield.
    if (ShieldSize(true, true, 0) != 0 || ShieldSize(true, true, -1) != -1 ||
        ShieldDefense(true, true, 0) != 0 || ShieldDefense(true, true, -1) != -1)
    {
        return false;
    }
    return true;
}
} // namespace

auto runCharShield1489SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "char shield 1489 self-test failed\n";
    }
    return ok;
}
