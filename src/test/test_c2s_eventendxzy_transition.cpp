/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_eventendxzy_transition.h"

#include <cstdint>
#include <iostream>
#include <string_view>

#include "map/packets/c2s/0x05c_eventendxzy.h"

namespace
{

auto expect(const bool condition, const std::string_view label) -> bool
{
    if (!condition)
    {
        std::cerr << "c2s EVENTENDXZY transition self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto samePosition(const position_t& left, const position_t& right) -> bool
{
    return left.x == right.x && left.y == right.y && left.z == right.z &&
           left.moving == right.moving && left.rotation == right.rotation;
}

auto testUpdateMovesCharacterAndLivingPet() -> bool
{
    const auto current    = position_t{ 1, 2, 3, 4, 5 };
    const auto transition = eventendxzyhelpers::MakeTransition(
        1, 0, current, 10, 20, 30, -5, true, false, 0x11223344);

    return expect(transition.updatePosition, "update result one updates position") &&
           expect(transition.resetNoPositionUpdate, "always resets noPosUpdate") &&
           expect(samePosition(transition.newPosition, position_t{ 10, 20, 30, 0, 251 }), "requested position and uint8 direction") &&
           expect(transition.emitWPos2 && transition.wpos2Mode == eventendxzyhelpers::PositionMode::Event, "update emits event WPOS2") &&
           expect(transition.emitWPos && transition.wposMode == eventendxzyhelpers::PositionMode::Normal, "update emits normal WPOS") &&
           expect(transition.repositionPet && transition.disengagePet, "living pet moves and disengages") &&
           expect(transition.clearEnmityForId == 0x11223344, "living pet clear enmity ID") &&
           expect(transition.emitEventRecvPending, "always emits receive pending");
}

auto testNoPositionUpdateOverridesLuaResult() -> bool
{
    const auto current    = position_t{ 1, 2, 3, 4, 5 };
    const auto transition = eventendxzyhelpers::MakeTransition(
        1, 1, current, 10, 20, 30, 6, false, false, 0);

    return expect(!transition.updatePosition, "noPosUpdate suppresses update") &&
           expect(samePosition(transition.newPosition, current), "suppressed update preserves position") &&
           expect(transition.emitWPos2 && transition.wpos2Mode == eventendxzyhelpers::PositionMode::Clear, "suppressed update emits clear WPOS2") &&
           expect(!transition.emitWPos, "suppressed update has no WPOS") &&
           expect(!transition.repositionPet && !transition.disengagePet && !transition.clearEnmityForId.has_value(), "no pet has no pet work") &&
           expect(transition.emitEventRecvPending, "suppressed update emits receive pending");
}

auto testDeadPetDoesNotReceiveEffects() -> bool
{
    const auto transition = eventendxzyhelpers::MakeTransition(
        2, 0, position_t{ 1, 2, 3, 4, 5 }, 10, 20, 30, 6, true, true, 0x99);

    return expect(!transition.updatePosition, "non-one Lua result preserves position") &&
           expect(transition.wpos2Mode == eventendxzyhelpers::PositionMode::Clear, "non-update clears WPOS2") &&
           expect(!transition.repositionPet && !transition.disengagePet && !transition.clearEnmityForId.has_value(), "dead pet has no pet work");
}

} // namespace

auto runC2SEventEndXZYTransitionSelfTests() -> bool
{
    return testUpdateMovesCharacterAndLivingPet() &&
           testNoPositionUpdateOverridesLuaResult() &&
           testDeadPetDoesNotReceiveEffects();
}
