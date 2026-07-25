#include "test_temp_item_clear_6872.h"

#include "map/char_temp_item_clear.h"

#include <iostream>

auto runTempItemClear6872SelfTests() -> bool
{
    constexpr auto want = tempitemclearhelpers::Plan{
        .actions = { tempitemclearhelpers::Action::DeletePersistedItems, tempitemclearhelpers::Action::ClearItemContainer },
        .count   = 2,
    };
    const bool ok = tempitemclearhelpers::TempItemLocation == 3 && tempitemclearhelpers::MakePlan() == want;
    if (!ok)
    {
        std::cerr << "temp item clear 6872 self-test failed\n";
    }
    return ok;
}
