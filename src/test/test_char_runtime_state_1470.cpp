#include "test_char_runtime_state_1470.h"

#include "map/char_runtime_state_capacity.h"

#include <iostream>

auto runCharRuntimeState1470SelfTests() -> bool
{
    bool flag = false;
    bool ok   = !charruntimestatehelpers::Get(flag);
    charruntimestatehelpers::Set(flag, true);
    ok = charruntimestatehelpers::Get(flag) && ok;
    charruntimestatehelpers::Set(flag, false);
    ok = !charruntimestatehelpers::Get(flag) && ok;

    int proposalTime = 0;
    charruntimestatehelpers::Set(proposalTime, 123);
    ok = charruntimestatehelpers::Get(proposalTime) == 123 && ok;
    ok = !charruntimestatehelpers::InMogHouse(0) && charruntimestatehelpers::InMogHouse(1) &&
         charruntimestatehelpers::InMogHouse(65535) && ok;

    if (!ok)
    {
        std::cerr << "char runtime state 1470 self-test failed\n";
    }
    return ok;
}
