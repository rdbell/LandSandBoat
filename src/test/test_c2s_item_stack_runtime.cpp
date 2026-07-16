/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_item_stack_runtime.h"

#include <iostream>
#include <vector>

#include "map/packets/c2s/0x03a_item_stack.h"

auto runC2SItemStackRuntimeSelfTests() -> bool
{
    const std::vector<itemstackhelpers::StackSlot> slots = {
        { 100, 3, 10, true, false, false },
        { 100, 4, 10, true, false, false },
        { 100, 8, 10, true, false, false },
        { 100, 2, 10, true, false, true },
        { 100, 2, 10, true, true, false },
        { 101, 2, 10, true, false, false },
        { 100, 10, 10, true, false, false },
    };
    const std::vector<itemstackhelpers::StackTransfer> expected = {
        { 1, 2, 4 },
        { 1, 3, 3 },
    };
    const auto actual = itemstackhelpers::BuildStackTransfers(slots);

    const bool orderedMerge = actual.size() == expected.size() &&
                              actual[0].destinationSlot == expected[0].destinationSlot && actual[0].sourceSlot == expected[0].sourceSlot && actual[0].quantity == expected[0].quantity &&
                              actual[1].destinationSlot == expected[1].destinationSlot && actual[1].sourceSlot == expected[1].sourceSlot && actual[1].quantity == expected[1].quantity;
    const auto fullDestination = itemstackhelpers::BuildStackTransfers({
        { 100, 10, 10, true, false, false },
        { 100, 1, 10, true, false, false },
        { 100, 9, 10, true, false, false },
        { 100, 1, 0, true, false, false },
    });
    const bool skipsFullAndInvalid = fullDestination.size() == 1 && fullDestination[0].destinationSlot == 2 && fullDestination[0].sourceSlot == 3 && fullDestination[0].quantity == 9;

    if (!orderedMerge || !skipsFullAndInvalid)
    {
        std::cerr << "c2s ITEM_STACK runtime self-test failed\n";
    }
    return orderedMerge && skipsFullAndInvalid;
}
