#include "test_char_activity_1488.h"

#include "map/char_activity_capacity.h"

#include <iostream>

namespace
{
using charactivityhelpers::IsCrafting;
using charactivityhelpers::IsFishing;

auto CheckCrafting() -> bool
{
    if (!IsCrafting(charactivityhelpers::AnimationSynth, false) ||
        !IsCrafting(0, true) ||
        IsCrafting(0, false) ||
        IsCrafting(1, false) ||
        !IsCrafting(99, true))
    {
        return false;
    }
    return true;
}

auto CheckFishing() -> bool
{
    // Inclusive new fishing range 57..62.
    if (!IsFishing(57) || !IsFishing(62) || IsFishing(63) || IsFishing(55))
    {
        return false;
    }
    // Explicit start markers outside that range.
    if (!IsFishing(50) || !IsFishing(56))
    {
        return false;
    }
    // Old mid-range animations are NOT covered by isFishing.
    if (IsFishing(38) || IsFishing(43) || IsFishing(44))
    {
        return false;
    }
    return true;
}
} // namespace

auto runCharActivity1488SelfTests() -> bool
{
    const bool ok = CheckCrafting() && CheckFishing();
    if (!ok)
    {
        std::cerr << "char activity 1488 self-test failed\n";
    }
    return ok;
}
