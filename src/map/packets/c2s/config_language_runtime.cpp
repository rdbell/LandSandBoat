/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "config_language_runtime.h"

auto configlanguage::PlanFor(const std::uint8_t kind, const std::uint32_t (&configSys)[3], const std::uint32_t param, const RuntimeState& state) -> RuntimePlan
{
    auto plan = RuntimePlan{};

    switch (kind)
    {
        case 0: // GP_CLI_COMMAND_CONFIG_LANGUAGE_KIND::SearchLanguage
            plan.playerConfig       = configSys[0];
            plan.updatePlayerConfig = state.playerConfig != plan.playerConfig;
            plan.savePlayerSettings = plan.updatePlayerConfig;

            plan.chatFilter1        = configSys[1];
            plan.chatFilter2        = configSys[2];
            plan.updateChatFilters  = state.chatFilter1 != plan.chatFilter1 || state.chatFilter2 != plan.chatFilter2;
            plan.saveChatFilterFlags = plan.updateChatFilters;
            break;
        case 1: // GP_CLI_COMMAND_CONFIG_LANGUAGE_KIND::PartyLanguages
            plan.partyLanguages       = static_cast<std::uint8_t>(param);
            plan.updatePartyLanguages = state.partyLanguages != param;
            plan.saveLanguages        = plan.updatePartyLanguages;
            break;
    }

    return plan;
}
