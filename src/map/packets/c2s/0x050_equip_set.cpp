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

#include "0x050_equip_set.h"

#include "entities/char_entity.h"
#include "lua/luautils.h"
#include "utils/charutils.h"

// Go host pure half: packetsystem.ValidateEquipSet / ProcessEquipSet / NewEquipSetHandler (6778); plan mappacket.ClientEquipSetDispatchPlanFor.

namespace
{

const auto validContainers = [](const CCharEntity* PChar) -> std::set<CONTAINER_ID>
{
    return equipsethelpers::ValidContainers({
        .wardrobesAvailable = {
            PChar->getStorage(LOC_WARDROBE3)->GetSize() > 0,
            PChar->getStorage(LOC_WARDROBE4)->GetSize() > 0,
            PChar->getStorage(LOC_WARDROBE5)->GetSize() > 0,
            PChar->getStorage(LOC_WARDROBE6)->GetSize() > 0,
            PChar->getStorage(LOC_WARDROBE7)->GetSize() > 0,
            PChar->getStorage(LOC_WARDROBE8)->GetSize() > 0,
        },
        .equipFromOtherContainers = settings::get<bool>("main.EQUIP_FROM_OTHER_CONTAINERS"),
        .mogSatchelAvailable      = PChar->getStorage(LOC_MOGSATCHEL)->GetSize() > 0,
        .mogSackAvailable         = PChar->getStorage(LOC_MOGSACK)->GetSize() > 0,
        .mogCaseAvailable         = PChar->getStorage(LOC_MOGCASE)->GetSize() > 0,
    });
};

} // namespace

auto equipsethelpers::ValidContainers(const ContainerFacts& facts) -> std::set<CONTAINER_ID>
{
    std::set allowedContainers = {
        LOC_INVENTORY,
        LOC_WARDROBE,
        LOC_WARDROBE2,
    };

    constexpr std::array unlockableContainers = {
        LOC_WARDROBE3,
        LOC_WARDROBE4,
        LOC_WARDROBE5,
        LOC_WARDROBE6,
        LOC_WARDROBE7,
        LOC_WARDROBE8,
    };
    for (size_t index = 0; index < unlockableContainers.size(); ++index)
    {
        if (facts.wardrobesAvailable[index])
        {
            allowedContainers.insert(unlockableContainers[index]);
        }
    }

    if (facts.equipFromOtherContainers)
    {
        if (facts.mogSatchelAvailable)
            allowedContainers.insert(LOC_MOGSATCHEL);
        if (facts.mogSackAvailable)
            allowedContainers.insert(LOC_MOGSACK);
        if (facts.mogCaseAvailable)
            allowedContainers.insert(LOC_MOGCASE);
    }

    return allowedContainers;
}

auto GP_CLI_COMMAND_EQUIP_SET::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .blockedBy({ BlockedState::InEvent, BlockedState::AbnormalStatus })
        .oneOf<SLOTTYPE>(this->EquipKind)
        .oneOf("Category", static_cast<CONTAINER_ID>(this->Category), validContainers(PChar));
}

void GP_CLI_COMMAND_EQUIP_SET::process(MapSession* PSession, CCharEntity* PChar) const
{
    const auto plan = equipsethelpers::MakeDispatchPlan();
    if (!plan.equip) return;
    charutils::EquipItem(PChar, this->PropertyItemIndex, this->EquipKind, this->Category);
    if (plan.persist) PChar->RequestPersist(CHAR_PERSIST::EQUIP);
    if (plan.checkGearSet) luautils::CheckForGearSet(PChar);
    if (plan.updateHealth) PChar->UpdateHealth();
    if (plan.retriggerLatents) PChar->retriggerLatents = true;
    // TODO: Sort out above logic and ensure the following packets are emitted synchronously as a response
    // EQUIP_LIST
    // GRAP_LIST
    // MAGIC_DATA
    // COMMAND_DATA
}
