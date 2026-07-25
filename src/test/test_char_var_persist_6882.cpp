#include "test_char_var_persist_6882.h"

#include "map/char_var_persist.h"

#include <iostream>

auto runCharVarPersist6882SelfTests() -> bool
{
    using Action = charvarpersisthelpers::Action;

    const bool ok = charvarpersisthelpers::ActionFor(0) == Action::Delete &&
                    charvarpersisthelpers::ActionFor(1) == Action::Upsert &&
                    charvarpersisthelpers::ActionFor(-1) == Action::Upsert;
    if (!ok)
    {
        std::cerr << "char var persist 6882 self-test failed\n";
    }
    return ok;
}
