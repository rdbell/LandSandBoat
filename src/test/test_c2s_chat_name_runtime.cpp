/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_chat_name_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x0b6_chat_name.h"

auto runC2SChatNameRuntimeSelfTests() -> bool
{
    using chatnamehelpers::Action;

    const auto lengthOK = chatnamehelpers::BoundedMessageLength(0) == 0 &&
                          chatnamehelpers::BoundedMessageLength(-1) == chatnamehelpers::MaxMessageLength &&
                          chatnamehelpers::BoundedMessageLength(chatnamehelpers::MessageOffset) == 0 &&
                          chatnamehelpers::BoundedMessageLength(chatnamehelpers::MessageOffset + 7) == 7 &&
                          chatnamehelpers::BoundedMessageLength(chatnamehelpers::MessageOffset + 999) == chatnamehelpers::MaxMessageLength;
    const auto actionsOK = chatnamehelpers::SelectAction(true, true, true) == Action::RejectJailed &&
                           chatnamehelpers::SelectAction(false, true, true) == Action::HandleCustomMenu &&
                           chatnamehelpers::SelectAction(false, true, false) == Action::ForwardTell &&
                           chatnamehelpers::SelectAction(false, false, true) == Action::ForwardTell;

    if (!lengthOK || !actionsOK)
    {
        std::cerr << "c2s CHAT_NAME runtime self-test failed\n";
    }

    return lengthOK && actionsOK;
}
