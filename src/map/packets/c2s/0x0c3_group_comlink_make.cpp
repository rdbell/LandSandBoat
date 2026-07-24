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

#include "0x0c3_group_comlink_make.h"

#include "entities/char_entity.h"
#include "items.h"
#include "items/item_linkshell.h"
#include "utils/charutils.h"
#include "utils/itemutils.h"

// Go host pure half: packetsystem.ValidateGroupComlinkMake / ProcessGroupComlinkMake /
// NewGroupComlinkMakeHandler (6504); plan mappacket.ClientGroupComlinkMakeActionPlanFor.
auto GP_CLI_COMMAND_GROUP_COMLINK_MAKE::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .blockedBy({ BlockedState::InEvent })
        .mustEqual(this->State, 0, "State not 0")
        .oneOf<GP_CLI_COMMAND_GROUP_COMLINK_MAKE_LINKSHELLID>(this->LinkshellId)
        .hasLinkshellRank(this->LinkshellId, LSTYPE_PEARLSACK);
}

void GP_CLI_COMMAND_GROUP_COMLINK_MAKE::process(MapSession* PSession, CCharEntity* PChar) const
{
    const CItemLinkshell* PItemLinkshell = nullptr;

    switch (groupcomlinkmakehelpers::SelectSourceSlot(this->LinkshellId))
    {
        case groupcomlinkmakehelpers::SourceSlot::Linkshell1:
            PItemLinkshell = reinterpret_cast<CItemLinkshell*>(PChar->getEquip(SLOT_LINK1));
            break;
        case groupcomlinkmakehelpers::SourceSlot::Linkshell2:
            PItemLinkshell = reinterpret_cast<CItemLinkshell*>(PChar->getEquip(SLOT_LINK2));
            break;
        case groupcomlinkmakehelpers::SourceSlot::None:
            break;
    }

    if (!PItemLinkshell)
    {
        return;
    }

    // Make a new Linkpearl
    auto PItem = xi::items::spawn(ITEMID::LINKPEARL);
    const auto plan = groupcomlinkmakehelpers::PlanFor(this->LinkshellId, true, static_cast<bool>(PItem));
    if (plan.createLinkpearl)
    {
        auto* PItemLinkPearl = static_cast<CItemLinkshell*>(PItem.get());
        PItemLinkPearl->setQuantity(1);
        std::memcpy(PItemLinkPearl->m_extra, PItemLinkshell->m_extra, 24);
        PItemLinkPearl->SetLSType(LSTYPE_LINKPEARL);
        charutils::AddItem(PChar, LOC_INVENTORY, std::move(PItem));
    }
}
