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

#include "0x03b_subcontainer.h"

#include "entities/char_entity.h"
#include "enums/item_lockflg.h"
#include "items/item_furnishing.h"
#include "packets/s2c/0x01d_item_same.h"
#include "packets/s2c/0x01f_item_list.h"
#include "packets/s2c/0x020_item_attr.h"
#include "packets/s2c/0x026_item_subcontainer.h"

// Go host pure half: packetsystem.ValidateSubcontainer / ProcessSubcontainer /
// NewSubcontainerHandler (6456); plan mappacket.ClientSubcontainerTransitionFor.

namespace
{

const auto setStatusOfStorageItemAtSlot = [](CCharEntity* PChar, const uint8 slot, ItemLockFlg status) -> void
{
    if (PChar == nullptr || slot == 0)
    {
        return;
    }

    auto* PItem = PChar->getStorage(LOC_STORAGE)->GetItem(slot);
    if (PItem == nullptr)
    {
        return;
    }

    PChar->pushPacket<GP_SERV_COMMAND_ITEM_LIST>(PItem, status);
};

const auto validContainers = [](const CCharEntity* PChar) -> std::set<CONTAINER_ID>
{
    std::set allowedContainers = {
        LOC_MOGSAFE
    };

    // Bitflag indicating if Mog 2F is unlocked
    if (PChar->profile.mhflag & 0x20)
    {
        allowedContainers.insert(LOC_MOGSAFE2);
    }

    return allowedContainers;
};

} // namespace

auto GP_CLI_COMMAND_SUBCONTAINER::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .blockedBy({ BlockedState::InEvent })
        .mustEqual(PChar->m_moghouseID, PChar->id, "Character not in their mog house")
        .oneOf<GP_CLI_COMMAND_SUBCONTAINER_KIND>(this->Kind)
        .oneOf<GP_CLI_COMMAND_SUBCONTAINER_CONTAINERINDEX>(this->ContainerIndex)
        .oneOf("Category1", static_cast<CONTAINER_ID>(this->Category1), validContainers(PChar));
}

void GP_CLI_COMMAND_SUBCONTAINER::process(MapSession* PSession, CCharEntity* PChar) const
{
    auto* PItem = PChar->getStorage(this->Category1)->GetItem(this->ItemIndex1);
    if (PItem == nullptr)
    {
        ShowWarning("GP_CLI_COMMAND_SUBCONTAINER: Unable to load mannequin from slot %u in location %u by %s", this->ItemIndex1, this->Category1, PChar->getName());
        return;
    }

    auto* PFurnishing = static_cast<CItemFurnishing*>(PItem);
    auto& mannequin   = PFurnishing->exdata<Exdata::Mannequin>();

    const auto transition = subcontainerhelpers::BuildTransition(
        static_cast<GP_CLI_COMMAND_SUBCONTAINER_KIND>(this->Kind),
        static_cast<CONTAINER_ID>(this->Category2),
        this->ContainerIndex,
        { mannequin.EquipMain, mannequin.EquipSub, mannequin.EquipRanged, mannequin.EquipHead, mannequin.EquipBody, mannequin.EquipHands, mannequin.EquipLegs, mannequin.EquipFeet },
        this->ItemIndex2);
    if (!transition.accepted)
    {
        ShowWarning("GP_CLI_COMMAND_SUBCONTAINER: Invalid item location passed to Mannequin Equip packet %u by %s", this->Category2, PChar->getName());
        return;
    }

    for (const auto& update : transition.lockUpdates)
    {
        setStatusOfStorageItemAtSlot(PChar, update.slot, update.lock == subcontainerhelpers::ItemLock::Normal ? ItemLockFlg::Normal : ItemLockFlg::Mannequin);
    }
    mannequin.EquipMain   = transition.equipment[0];
    mannequin.EquipSub    = transition.equipment[1];
    mannequin.EquipRanged = transition.equipment[2];
    mannequin.EquipHead   = transition.equipment[3];
    mannequin.EquipBody   = transition.equipment[4];
    mannequin.EquipHands  = transition.equipment[5];
    mannequin.EquipLegs   = transition.equipment[6];
    mannequin.EquipFeet   = transition.equipment[7];

    const auto rset = db::preparedStmt("UPDATE char_inventory "
                                       "SET "
                                       "extra = ? "
                                       "WHERE location = ? AND slot = ? AND charid = ?",
                                       PItem->m_extra,
                                       this->Category1,
                                       this->ItemIndex1,
                                       PChar->id);
    if (rset)
    {
        PChar->pushPacket<GP_SERV_COMMAND_ITEM_ATTR>(PFurnishing, static_cast<CONTAINER_ID>(this->Category1), this->ItemIndex1);
        PChar->pushPacket<GP_SERV_COMMAND_ITEM_SUBCONTAINER>(PChar, static_cast<CONTAINER_ID>(this->Category1), this->ItemIndex1, mannequin);
        PChar->pushPacket<GP_SERV_COMMAND_ITEM_SAME>(PChar);
    }
    else
    {
        ShowError("GP_CLI_COMMAND_SUBCONTAINER: Problem writing Mannequin to database!");
    }
}
