#include "test_char_var_fetch_6881.h"

#include "map/char_var_fetch.h"

#include <iostream>

auto runCharVarFetch6881SelfTests() -> bool
{
    using Plan = charvarfetchhelpers::Plan;

    const bool ok = charvarfetchhelpers::MakePlan(false, 77, 90, 100) == Plan{} &&
                    charvarfetchhelpers::MakePlan(true, 77, 0, 100) == Plan{ .value = 77 } &&
                    charvarfetchhelpers::MakePlan(true, 77, 101, 100) == Plan{ .value = 77, .expiry = 101 } &&
                    charvarfetchhelpers::MakePlan(true, 77, 100, 100) == Plan{ .expiry = 100, .deleteExpired = true } &&
                    charvarfetchhelpers::MakePlan(true, 77, 99, 100) == Plan{ .expiry = 99, .deleteExpired = true };
    if (!ok)
    {
        std::cerr << "char var fetch 6881 self-test failed\n";
    }
    return ok;
}
