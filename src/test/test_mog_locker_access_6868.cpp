#include "test_mog_locker_access_6868.h"

#include "map/char_mog_locker_access.h"

#include <iostream>

auto runMogLockerAccess6868SelfTests() -> bool
{
    bool ok = true;

    ok = !moglockeraccesshelpers::HasMogLockerAccess(100, 100, moglockeraccesshelpers::MogLockerAccessAllAreas, 0, true, false) && ok;
    ok = moglockeraccesshelpers::HasMogLockerAccess(99, 100, moglockeraccesshelpers::MogLockerAccessAllAreas, 0, true, false) && ok;
    ok = moglockeraccesshelpers::HasMogLockerAccess(99, 100, moglockeraccesshelpers::MogLockerAccessAllAreas, 0, false, true) && ok;
    ok = moglockeraccesshelpers::HasMogLockerAccess(99, 100, moglockeraccesshelpers::MogLockerAccessAlZahbi, moglockeraccesshelpers::ZoneAlZahbi, false, true) && ok;
    ok = moglockeraccesshelpers::HasMogLockerAccess(99, 100, moglockeraccesshelpers::MogLockerAccessAlZahbi, moglockeraccesshelpers::ZoneAhtUrhganWhitegate, false, true) && ok;
    ok = moglockeraccesshelpers::HasMogLockerAccess(99, 100, moglockeraccesshelpers::MogLockerAccessAlZahbi, moglockeraccesshelpers::ZoneNashmau, false, false) && ok;
    ok = !moglockeraccesshelpers::HasMogLockerAccess(99, 100, moglockeraccesshelpers::MogLockerAccessAlZahbi, 1, false, true) && ok;

    if (!ok)
    {
        std::cerr << "mog locker access 6868 self-test failed\n";
    }
    return ok;
}
