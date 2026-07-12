#include "test_char_event_lock_1454.h"

#include "map/char_event_lock_capacity.h"

#include <iostream>
#include <vector>

auto runCharEventLock1454SelfTests() -> bool
{
    bool ok = !chareventlockhelpers::IsInEvent(-1) &&
              chareventlockhelpers::IsInEvent(0) &&
              chareventlockhelpers::IsInEvent(65535);
    ok = !chareventlockhelpers::IsNpcLocked(false, false) &&
         chareventlockhelpers::IsNpcLocked(true, false) &&
         chareventlockhelpers::IsNpcLocked(false, true) && ok;

    bool             locked = true;
    std::vector<int> calls{};
    chareventlockhelpers::SetLocked(
        locked,
        false,
        [&]() { calls.push_back(1); },
        [&]()
        {
            calls.push_back(2);
            return true;
        },
        [&]() { calls.push_back(3); },
        [&]() { calls.push_back(4); });
    ok = !locked && calls.empty() && ok;

    calls.clear();
    chareventlockhelpers::SetLocked(
        locked,
        true,
        [&]() { calls.push_back(locked ? 5 : 50); },
        [&]()
        {
            calls.push_back(6);
            return true;
        },
        [&]() { calls.push_back(7); },
        [&]() { calls.push_back(8); });
    ok = locked && calls == std::vector<int>{ 5, 6, 7, 8 } && ok;

    calls.clear();
    chareventlockhelpers::SetLocked(
        locked,
        true,
        [&]() { calls.push_back(9); },
        [&]()
        {
            calls.push_back(10);
            return false;
        },
        [&]() { calls.push_back(11); },
        [&]() { calls.push_back(12); });
    ok = locked && calls == std::vector<int>{ 9, 10, 12 } && ok;

    if (!ok)
    {
        std::cerr << "char event lock 1454 self-test failed\n";
    }
    return ok;
}
