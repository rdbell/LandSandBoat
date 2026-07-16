/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_emote_list_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x119_emote_list.h"

auto runC2SEmoteListRuntimeSelfTests() -> bool
{
    if (emotelistpackethelpers::SelectAction() != emotelistpackethelpers::Action::SendEmoteList)
    {
        std::cerr << "c2s EMOTE_LIST runtime self-test failed: action is not SendEmoteList\\n";
        return false;
    }
    return true;
}
