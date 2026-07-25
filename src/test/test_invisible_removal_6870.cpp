#include "test_invisible_removal_6870.h"

#include "map/char_invisible_removal.h"

#include <iostream>

auto runInvisibleRemoval6870SelfTests() -> bool
{
    const bool ok = !invisibleremovalhelpers::ShouldRemoveInvisible(false, true) &&
                    !invisibleremovalhelpers::ShouldRemoveInvisible(true, false) &&
                    invisibleremovalhelpers::ShouldRemoveInvisible(true, true);
    if (!ok)
    {
        std::cerr << "invisible removal 6870 self-test failed\n";
    }
    return ok;
}
