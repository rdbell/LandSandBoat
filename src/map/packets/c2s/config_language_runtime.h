/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#pragma once

#include <cstdint>

namespace configlanguage
{

struct RuntimeState
{
    std::uint32_t playerConfig   = 0;
    std::uint32_t chatFilter1    = 0;
    std::uint32_t chatFilter2    = 0;
    std::uint8_t  partyLanguages = 0;
};

struct RuntimePlan
{
    bool          updatePlayerConfig   = false;
    std::uint32_t playerConfig         = 0;
    bool          savePlayerSettings   = false;
    bool          updateChatFilters    = false;
    std::uint32_t chatFilter1          = 0;
    std::uint32_t chatFilter2          = 0;
    bool          saveChatFilterFlags  = false;
    bool          updatePartyLanguages = false;
    std::uint8_t  partyLanguages       = 0;
    bool          saveLanguages        = false;
    bool          sendConfig           = true;
    bool          sendCharStatus       = true;
};

// PlanFor returns CONFIG_LANGUAGE's host-independent post-validation
// mutations. kind uses GP_CLI_COMMAND_CONFIG_LANGUAGE_KIND's wire values.
auto PlanFor(std::uint8_t kind, const std::uint32_t (&configSys)[3], std::uint32_t param, const RuntimeState& state) -> RuntimePlan;

} // namespace configlanguage
