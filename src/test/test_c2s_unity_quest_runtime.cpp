/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_unity_quest_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x117_unity_quest.h"

auto runC2SUnityQuestRuntimeSelfTests() -> bool
{
    if (unityquesthelpers::SelectAction() != unityquesthelpers::Action::SendUnityAndUnityPackets)
    {
        std::cerr << "c2s UNITY_QUEST runtime self-test failed: action is not SendUnityAndUnityPackets\\n";
        return false;
    }

    return true;
}
