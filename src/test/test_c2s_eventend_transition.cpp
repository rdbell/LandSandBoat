/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_eventend_transition.h"

#include <cstdint>
#include <iostream>
#include <string_view>

#include "map/packets/c2s/0x05b_eventend.h"

namespace
{

auto expect(const bool condition, const std::string_view label) -> bool
{
    if (!condition)
    {
        std::cerr << "c2s EVENTEND transition self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto testUpdateUsesEndParaWithoutOverride() -> bool
{
    const auto transition = eventendhelpers::MakeTransition(
        GP_CLI_COMMAND_EVENTEND_MODE::UpdatePending, 0x1234, 42, 0, false);

    return expect(transition.eventId == 0x1234, "update event ID") &&
           expect(transition.result == 42, "update EndPara result") &&
           expect(transition.callback == eventendhelpers::Callback::Update, "update callback") &&
           expect(!transition.lockCharacter, "ordinary update does not lock") &&
           expect(!transition.disableEventSkipping, "ordinary update preserves skipping") &&
           expect(transition.emitEventRecvPending, "update emits receive pending") &&
           expect(transition.updateHP, "update marks HP") &&
           expect(!transition.shouldEndCurrentEvent(0x1234), "update never ends event");
}

auto testOptionalCutsceneOverrideLocksAndDisablesSkip() -> bool
{
    const auto transition = eventendhelpers::MakeTransition(
        GP_CLI_COMMAND_EVENTEND_MODE::UpdatePending, 3, 42, 99, true);

    return expect(transition.result == 99, "current option overrides EndPara") &&
           expect(transition.lockCharacter, "optional cutscene locks") &&
           expect(transition.disableEventSkipping, "optional cutscene disables skip");
}

auto testMinusOneDoesNotLockEvenWhenOptional() -> bool
{
    const auto transition = eventendhelpers::MakeTransition(
        GP_CLI_COMMAND_EVENTEND_MODE::UpdatePending, 3, UINT32_MAX, 0, true);

    return expect(transition.result == UINT32_MAX, "unsigned EndPara preserves sentinel") &&
           expect(!transition.lockCharacter, "minus one does not lock") &&
           expect(!transition.disableEventSkipping, "minus one preserves skip");
}

auto testFinishOnlyEndsUnchangedEvent() -> bool
{
    const auto transition = eventendhelpers::MakeTransition(
        GP_CLI_COMMAND_EVENTEND_MODE::End, 0x4321, 7, 0, false);

    return expect(transition.callback == eventendhelpers::Callback::Finish, "finish callback") &&
           expect(transition.shouldEndCurrentEvent(0x4321), "unchanged finished event ends") &&
           expect(!transition.shouldEndCurrentEvent(0x4322), "new event survives finish") &&
           expect(transition.emitEventRecvPending, "finish emits receive pending") &&
           expect(transition.updateHP, "finish marks HP");
}

auto testUnknownModeHasNoCallbackButRetainsTrailingEffects() -> bool
{
    const auto transition = eventendhelpers::MakeTransition(
        static_cast<GP_CLI_COMMAND_EVENTEND_MODE>(99), 1, 2, 0, false);

    return expect(transition.callback == eventendhelpers::Callback::None, "unknown mode has no callback") &&
           expect(transition.emitEventRecvPending, "unknown mode emits receive pending") &&
           expect(transition.updateHP, "unknown mode marks HP");
}

} // namespace

auto runC2SEventEndTransitionSelfTests() -> bool
{
    return testUpdateUsesEndParaWithoutOverride() &&
           testOptionalCutsceneOverrideLocksAndDisablesSkip() &&
           testMinusOneDoesNotLockEvenWhenOptional() &&
           testFinishOnlyEndsUnchangedEvent() &&
           testUnknownModeHasNoCallbackButRetainsTrailingEffects();
}
