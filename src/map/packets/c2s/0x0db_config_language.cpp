/*
===========================================================================

  Copyright (c) 2025 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "0x0db_config_language.h"

#include "config_language_runtime.h"

#include "entities/char_entity.h"
#include "packets/char_status.h"
#include "packets/s2c/0x0b4_config.h"
#include "utils/charutils.h"

// Go host pure half: packetsystem.ValidateConfigLanguage / ProcessConfigLanguage / NewConfigLanguageHandler (6543); plan mappacket.ClientConfigLanguageRuntimePlanFor.
auto GP_CLI_COMMAND_CONFIG_LANGUAGE::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .oneOf<GP_CLI_COMMAND_CONFIG_LANGUAGE_KIND>(this->Kind)
        .mustEqual(this->unknown00, 0, "unknown00 must be 0")
        .mustEqual(this->unknown01, 0, "unknown00 must be 0");
}

void GP_CLI_COMMAND_CONFIG_LANGUAGE::process(MapSession* PSession, CCharEntity* PChar) const
{
    uint32_t oldPlayerConfig = {};
    uint32_t oldChatFilter1  = {};
    uint32_t oldChatFilter2  = {};

    std::memcpy(&oldPlayerConfig, &PChar->playerConfig, sizeof(uint32_t));
    std::memcpy(&oldChatFilter1, &PChar->playerConfig.MessageFilter, sizeof(uint32_t));
    std::memcpy(&oldChatFilter2, &PChar->playerConfig.MessageFilter2, sizeof(uint32_t));

    const auto plan = configlanguage::PlanFor(this->Kind,
                                              this->ConfigSys,
                                              this->Param,
                                              configlanguage::RuntimeState{
                                                  .playerConfig   = oldPlayerConfig,
                                                  .chatFilter1    = oldChatFilter1,
                                                  .chatFilter2    = oldChatFilter2,
                                                  .partyLanguages = PChar->search.language,
                                              });

    if (plan.updatePlayerConfig)
    {
        std::memcpy(&PChar->playerConfig, &plan.playerConfig, sizeof(uint32_t));
        charutils::SavePlayerSettings(PChar);
    }

    if (plan.updateChatFilters)
    {
        std::memcpy(&PChar->playerConfig.MessageFilter, &plan.chatFilter1, sizeof(uint32_t));
        std::memcpy(&PChar->playerConfig.MessageFilter2, &plan.chatFilter2, sizeof(uint32_t));
        // It's probably not necessary to save the flags as they are sent by the client on login.
        charutils::SaveChatFilterFlags(PChar);
    }

    if (plan.updatePartyLanguages)
    {
        PChar->search.language = plan.partyLanguages;
        charutils::SaveLanguages(PChar);
    }

    PChar->pushPacket<GP_SERV_COMMAND_CONFIG>(PChar);
    PChar->pushPacket<CCharStatusPacket>(PChar);
}
