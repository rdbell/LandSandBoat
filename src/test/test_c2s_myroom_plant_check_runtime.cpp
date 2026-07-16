/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_myroom_plant_check_runtime.h"

#include <iostream>

#include "map/items/item_flowerpot.h"
#include "map/packets/c2s/0x0fd_myroom_plant_check.h"

namespace
{
auto expect(const bool condition, const char* const detail) -> bool
{
    if (!condition)
    {
        std::cerr << "c2s MYROOM_PLANT_CHECK runtime self-test failed: " << detail << '\n';
    }
    return condition;
}
} // namespace

auto runC2SMyRoomPlantCheckRuntimeSelfTests() -> bool
{
    using myroomplantcheck::Facts;
    using myroomplantcheck::Message;

    auto ok = true;

    const auto invalid = myroomplantcheck::PlanFor({ .isFlowerpot = false });
    ok                 = expect(!invalid.sendMyRoomOperation && !invalid.markExamined && invalid.outputs.empty(), "non-flowerpot must not emit output, persist, or send operation") && ok;

    const auto emptyPot = myroomplantcheck::PlanFor({
        .isFlowerpot = true,
        .wasExamined = true,
    });
    ok = expect(emptyPot.sendMyRoomOperation && !emptyPot.markExamined && emptyPot.outputs.empty(), "valid empty pot must send operation only") && ok;

    const auto firstCrystal = myroomplantcheck::PlanFor({
        .isFlowerpot        = true,
        .isPlanted          = true,
        .isTree             = true,
        .stage              = FLOWERPOT_STAGE_SECOND_SPROUTS,
        .seedItemID         = 1237,
        .extraCrystalItemID = 4096,
    });
    ok = expect(firstCrystal.sendMyRoomOperation && firstCrystal.markExamined && firstCrystal.outputs.size() == 2, "tree after first crystal stage must send seed and extra crystal then persist examination") && ok;
    ok = expect(firstCrystal.outputs.size() == 2 && firstCrystal.outputs[0].message == Message::SeedSown && firstCrystal.outputs[0].itemID == 1237 && firstCrystal.outputs[1].message == Message::CrystalUsed && firstCrystal.outputs[1].itemID == 4096, "tree first crystal output order or values") && ok;

    const auto firstThreshold = myroomplantcheck::PlanFor({
        .isFlowerpot        = true,
        .isPlanted          = true,
        .isTree             = true,
        .stage              = FLOWERPOT_STAGE_FIRST_SPROUTS_CRYSTAL,
        .seedItemID         = 1237,
        .extraCrystalItemID = 4096,
    });
    ok = expect(firstThreshold.outputs.size() == 1 && firstThreshold.outputs[0].message == Message::SeedSown, "first crystal threshold must not emit extra crystal") && ok;

    const auto nonTree = myroomplantcheck::PlanFor({
        .isFlowerpot        = true,
        .isPlanted          = true,
        .stage              = FLOWERPOT_STAGE_SECOND_SPROUTS,
        .seedItemID         = 572,
        .extraCrystalItemID = 4096,
    });
    ok = expect(nonTree.outputs.size() == 1 && nonTree.outputs[0].message == Message::SeedSown, "non-tree must not emit extra crystal") && ok;

    const auto secondThreshold = myroomplantcheck::PlanFor({
        .isFlowerpot         = true,
        .isPlanted           = true,
        .stage               = FLOWERPOT_STAGE_SECOND_SPROUTS_CRYSTAL,
        .seedItemID          = 572,
        .commonCrystalItemID = 4101,
    });
    ok = expect(secondThreshold.outputs.size() == 1 && secondThreshold.outputs[0].message == Message::SeedSown, "second crystal threshold must not emit common crystal") && ok;

    const auto secondCrystal = myroomplantcheck::PlanFor({
        .isFlowerpot         = true,
        .isPlanted           = true,
        .isTree              = true,
        .wasExamined         = true,
        .stage               = FLOWERPOT_STAGE_SECOND_SPROUTS_3,
        .seedItemID          = 1238,
        .extraCrystalItemID  = 0,
        .commonCrystalItemID = 4101,
    });
    ok = expect(secondCrystal.sendMyRoomOperation && !secondCrystal.markExamined && secondCrystal.outputs.size() == 3, "second crystal stage must preserve examined state and send all messages") && ok;
    ok = expect(secondCrystal.outputs.size() == 3 && secondCrystal.outputs[0].message == Message::SeedSown && secondCrystal.outputs[1].message == Message::CrystalNone && secondCrystal.outputs[1].itemID == 0 && secondCrystal.outputs[2].message == Message::CrystalUsed && secondCrystal.outputs[2].itemID == 4101, "second crystal output order or values") && ok;

    return ok;
}
