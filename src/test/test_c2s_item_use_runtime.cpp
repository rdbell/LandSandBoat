#include "test_c2s_item_use_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x037_item_use.h"

auto runC2SItemUseRuntimeSelfTests() -> bool
{
    using itemusehelpers::Action;
    using itemusehelpers::DecideAction;
    using itemusehelpers::RuntimeFacts;

    const auto valid = RuntimeFacts{ true, 12.0f, true, false, false, false, false, false, false };
    const auto ok    =
        DecideAction({ false, 0.0f, true, false, false, false, false, false, false }) == Action::NoOp &&
        DecideAction({ true, 12.01f, true, false, false, false, false, false, false }) == Action::TooFarAway &&
        DecideAction({ true, 12.0f, false, false, false, false, false, false, false }) == Action::NoOp &&
        DecideAction({ true, 12.0f, true, false, true, false, false, false, false }) == Action::InvalidItem &&
        DecideAction({ true, 12.0f, true, true, true, true, false, false, false }) == Action::BeginUse &&
        DecideAction({ true, 12.0f, true, true, true, false, false, false, false }) == Action::InvalidItem &&
        DecideAction({ true, 12.0f, true, false, false, false, true, false, false }) == Action::InvalidItem &&
        DecideAction({ true, 12.0f, true, false, false, false, false, true, false }) == Action::InvalidItem &&
        DecideAction(valid) == Action::BeginUse &&
        DecideAction({ true, 12.0f, true, false, false, false, false, false, true }) == Action::UnableToUseItem;
    if (!ok)
    {
        std::cerr << "c2s ITEM_USE runtime self-test failed\n";
    }
    return ok;
}
