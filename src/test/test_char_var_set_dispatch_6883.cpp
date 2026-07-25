#include "test_char_var_set_dispatch_6883.h"

#include "map/char_var_set_dispatch.h"

#include <iostream>

auto runCharVarSetDispatch6883SelfTests() -> bool
{
    using Action = charvarsetdispatchhelpers::Action;

    const bool ok = charvarsetdispatchhelpers::ActionFor(true) == Action::UpdateLocalCache &&
                    charvarsetdispatchhelpers::ActionFor(false) == Action::PersistAndBroadcast;
    if (!ok)
    {
        std::cerr << "char var set dispatch 6883 self-test failed\n";
    }
    return ok;
}
