#include "test_char_drop_item_dispatch_6902.h"

#include "map/char_drop_item_dispatch.h"

#include <iostream>

auto runCharDropItemDispatch6902SelfTests() -> bool
{
    using dropitemdispatchhelpers::Action;
    using dropitemdispatchhelpers::Plan;

    const bool ok = dropitemdispatchhelpers::BuildPlan(false) == Plan{} &&
                    dropitemdispatchhelpers::BuildPlan(true) == Plan{ .actions = { Action::Log, Action::ThrowAwayMessage, Action::ItemSame }, .count = 3 };
    if (!ok)
    {
        std::cerr << "drop item dispatch 6902 self-test failed\n";
    }
    return ok;
}
