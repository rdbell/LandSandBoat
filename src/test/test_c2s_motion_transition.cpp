/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_motion_transition.h"

#include <iostream>
#include <string_view>

#include "map/packets/c2s/0x05d_motion.h"

namespace
{

auto expect(const bool condition, const std::string_view label) -> bool
{
    if (!condition)
    {
        std::cerr << "c2s MOTION transition self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto isDispatch(const motionhelpers::DispatchPlan& plan) -> bool
{
    return plan.action == motionhelpers::DispatchAction::Dispatch &&
           plan.broadcastMotionPacket && plan.invokePlayerEmoteLua &&
           !plan.sendCannotUseInArea;
}

auto testPrisonIsTheOnlyRejectionThatPlansABattleMessage() -> bool
{
    const auto prison = motionhelpers::MakeDispatchPlan(true, 18863, 0, 73, 6, 0);
    const auto bell = motionhelpers::MakeDispatchPlan(false, 0, 0, 73, 6, 0);

    return expect(prison.action == motionhelpers::DispatchAction::RejectInPrison, "prison rejects") &&
           expect(prison.sendCannotUseInArea, "prison plans CannotUseInArea") &&
           expect(!prison.broadcastMotionPacket && !prison.invokePlayerEmoteLua, "prison suppresses dispatch") &&
           expect(bell.action == motionhelpers::DispatchAction::Reject, "ordinary rejection") &&
           expect(!bell.sendCannotUseInArea, "ordinary rejection is silent");
}

auto testBellStylePrecedenceAndInclusiveNotes() -> bool
{
    const auto styleBell = motionhelpers::MakeDispatchPlan(false, 18863, 0, 73, 6, 0);
    const auto equippedBell = motionhelpers::MakeDispatchPlan(false, 0, 18869, 73, 0x1E, 0);
    const auto hiddenBell = motionhelpers::MakeDispatchPlan(false, 12345, 18863, 73, 6, 0);
    const auto invalidNote = motionhelpers::MakeDispatchPlan(false, 18864, 0, 73, 5, 0);
    const auto highNote = motionhelpers::MakeDispatchPlan(false, 18868, 0, 73, 0x1F, 0);

    return expect(isDispatch(styleBell), "style bell dispatches") &&
           expect(isDispatch(equippedBell), "equipped bell dispatches without style") &&
           expect(hiddenBell.action == motionhelpers::DispatchAction::Reject, "style takes precedence over equipped bell") &&
           expect(invalidNote.action == motionhelpers::DispatchAction::Reject, "low note rejects") &&
           expect(highNote.action == motionhelpers::DispatchAction::Reject, "high note rejects");
}

auto testJobEmoteUnlockBits() -> bool
{
    const auto zero = motionhelpers::MakeDispatchPlan(false, 0, 0, 74, 0, 0);
    const auto first = motionhelpers::MakeDispatchPlan(false, 0, 0, 74, 0x1E, 1);
    const auto second = motionhelpers::MakeDispatchPlan(false, 0, 0, 74, 0x1F, 2);
    const auto locked = motionhelpers::MakeDispatchPlan(false, 0, 0, 74, 0x1F, 1);
    const auto badLow = motionhelpers::MakeDispatchPlan(false, 0, 0, 74, 1, UINT32_MAX);
    const auto badHigh = motionhelpers::MakeDispatchPlan(false, 0, 0, 74, 0x3E, UINT32_MAX);

    return expect(isDispatch(zero), "zero job Param is accepted") &&
           expect(isDispatch(first), "first unlocked bit dispatches") &&
           expect(isDispatch(second), "second unlocked bit dispatches") &&
           expect(locked.action == motionhelpers::DispatchAction::Reject, "locked bit rejects") &&
           expect(badLow.action == motionhelpers::DispatchAction::Reject, "invalid low shift rejects") &&
           expect(badHigh.action == motionhelpers::DispatchAction::Reject, "invalid high shift rejects");
}

auto testOrdinaryEmoteDispatches() -> bool
{
    return expect(isDispatch(motionhelpers::MakeDispatchPlan(false, 0, 0, 0, 0, 0)),
                  "ordinary emote broadcasts and invokes Lua");
}

} // namespace

auto runC2SMotionTransitionSelfTests() -> bool
{
    return testPrisonIsTheOnlyRejectionThatPlansABattleMessage() &&
           testBellStylePrecedenceAndInclusiveNotes() &&
           testJobEmoteUnlockBits() &&
           testOrdinaryEmoteDispatches();
}
