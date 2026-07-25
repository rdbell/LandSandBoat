#include "test_char_var_clear_all_6884.h"

#include "map/char_var_clear_all.h"

#include <iostream>

auto runCharVarClearAll6884SelfTests() -> bool
{
    const bool ok = charvarclearallhelpers::MakePlan(false) == charvarclearallhelpers::Plan{
                                                               .deletePersisted  = true,
                                                               .refreshLocalCaches = true,
                                                           } &&
                    charvarclearallhelpers::MakePlan(true) == charvarclearallhelpers::Plan{
                                                              .deletePersisted  = false,
                                                              .refreshLocalCaches = true,
                                                          };
    if (!ok)
    {
        std::cerr << "char var clear all 6884 self-test failed\n";
    }
    return ok;
}
