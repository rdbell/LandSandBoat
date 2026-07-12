#include "test_char_ability_response_1493.h"

#include "map/char_ability_response_capacity.h"

#include <iostream>

namespace
{
using charabilityresponsehelpers::ShouldNotify;

auto Check() -> bool
{
    if (!ShouldNotify(true, true, true))
    {
        return false;
    }
    if (ShouldNotify(false, true, true) || ShouldNotify(true, false, true) || ShouldNotify(true, true, false))
    {
        return false;
    }
    return true;
}
} // namespace

auto runCharAbilityResponse1493SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "char ability response 1493 self-test failed\n";
    }
    return ok;
}
