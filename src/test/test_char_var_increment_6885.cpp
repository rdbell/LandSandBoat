#include "test_char_var_increment_6885.h"

#include "map/char_var_increment.h"

#include <iostream>

auto runCharVarIncrement6885SelfTests() -> bool
{
    const bool ok = charvarincrementhelpers::MakePlan(false) == charvarincrementhelpers::Plan{} &&
                    charvarincrementhelpers::MakePlan(true) == charvarincrementhelpers::Plan{
                                                               .persistIncrement = true,
                                                               .evictLocalCache = true,
                                                           };
    if (!ok)
    {
        std::cerr << "char var increment 6885 self-test failed\n";
    }
    return ok;
}
