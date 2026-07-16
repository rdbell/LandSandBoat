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

#pragma once

#include <array>

#include "base.h"

// GAMEOK's ordered initialization actions. Host state decides the optional
// treasure and pet actions; packet delivery remains owned by the map host.
namespace gameokhelpers
{

enum class Action : uint8
{
    EnterZone,
    ItemMax,
    Config,
    GrapList,
    JobInfo,
    AlterEgoPoints,
    CharacterStatus,
    Monstrosity,
    Homepoints,
    ExtendedJobPackets,
    UnityPackets,
    StatusIcons,
    KeyItems,
    QuestMissionLog,
    RecordsOfEminenceLog,
    MagicData,
    MountData,
    Dungeon,
    CommandData,
    CharacterSync,
    InspectMessage,
    Merit,
    Inventory,
    Blacklist,
    PendingGMCallResponse,
    UpdateTreasurePool,
    SpawnTransport,
    SpawnPersistedPet,
    ResetPetZoningInfo,
};

struct RuntimeFacts
{
    bool hasTreasurePool;
    bool canUsePets;
    bool inMogHouse;
    bool petPersists;
};

struct ActionPlan
{
    std::array<Action, 29> actions{};
    uint8                  count{};
};

[[nodiscard]] constexpr auto BuildActionPlan(const RuntimeFacts facts) -> ActionPlan
{
    auto plan = ActionPlan{ { Action::EnterZone, Action::ItemMax, Action::Config, Action::GrapList, Action::JobInfo, Action::AlterEgoPoints, Action::CharacterStatus, Action::Monstrosity, Action::Homepoints, Action::ExtendedJobPackets, Action::UnityPackets, Action::StatusIcons, Action::KeyItems, Action::QuestMissionLog, Action::RecordsOfEminenceLog, Action::MagicData, Action::MountData, Action::Dungeon, Action::CommandData, Action::CharacterSync, Action::InspectMessage, Action::Merit, Action::Inventory, Action::Blacklist, Action::PendingGMCallResponse }, 25 };
    if (facts.hasTreasurePool)
        plan.actions[plan.count++] = Action::UpdateTreasurePool;
    plan.actions[plan.count++] = Action::SpawnTransport;
    if (facts.canUsePets && !facts.inMogHouse)
    {
        if (facts.petPersists)
            plan.actions[plan.count++] = Action::SpawnPersistedPet;
        plan.actions[plan.count++] = Action::ResetPetZoningInfo;
    }
    return plan;
}

} // namespace gameokhelpers

// https://github.com/atom0s/XiPackets/tree/main/world/client/0x000C
// This packet is sent by the client when the client is in a state, after zoning, that is ready to begin obtaining additional packets from the server.
GP_CLI_PACKET(GP_CLI_COMMAND_GAMEOK,
              uint32_t ClientState;        // PS2: ClientState
              uint32_t DebugClientFlg : 1; // PS2: DebugClientFlg
              uint32_t unused : 31;        // PS2: unused
);
