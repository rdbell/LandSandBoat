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

#include "0x028_item_dump.h"

#include "entities/char_entity.h"
#include "enums/msg_std.h"
#include "items.h"
#include "items/item_linkshell.h"
#include "linkshell.h"
#include "utils/charutils.h"

namespace
{

// Retail honors _every_ container but Recycle Bin, even if you do not presently have access.
const std::set validContainers = {
    LOC_INVENTORY,
    LOC_MOGSAFE,
    LOC_MOGSAFE2,
    LOC_STORAGE,
    LOC_TEMPITEMS,
    LOC_MOGLOCKER,
    LOC_MOGSATCHEL,
    LOC_MOGSACK,
    LOC_MOGCASE,
    LOC_WARDROBE,
    LOC_WARDROBE2,
    LOC_WARDROBE3,
    LOC_WARDROBE4,
    LOC_WARDROBE5,
    LOC_WARDROBE6,
    LOC_WARDROBE7,
    LOC_WARDROBE8,
};

} // namespace

auto itemdump::PlanFor(const uint8_t category, const uint8_t index, const uint32_t quantity, const bool itemPresent, const bool locked, const uint32_t available, const bool storedSlip, const bool mainLinkshell, const bool recycleEnabled, const bool noRecycle) -> Plan
{
    if (category == LOC_INVENTORY && index == 0)
        return { .action = Action::Message };
    if (!itemPresent || locked || available < quantity)
        return {};
    if (storedSlip)
        return { .action = Action::Message };
    return { .action = (!recycleEnabled || category != LOC_INVENTORY || noRecycle) ? Action::Drop : Action::Recycle, .breakLinkshell = mainLinkshell };
}

// Go host pure half: packetsystem.ValidateItemDump / ProcessItemDump /
// NewItemDumpHandler (6446); pure plan itemdump.PlanFor.
auto GP_CLI_COMMAND_ITEM_DUMP::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .blockedBy({ BlockedState::InEvent })
        .oneOf("Category", static_cast<CONTAINER_ID>(this->Category), validContainers)
        .range("ItemNum", this->ItemNum, 0, 99); // Retail honors 0 quantity.
}

void GP_CLI_COMMAND_ITEM_DUMP::process(MapSession* PSession, CCharEntity* PChar) const
{
    CItem* PItem          = nullptr;
    bool   locked         = false;
    uint32 available      = 0;
    bool   storedSlip     = false;
    bool   mainLinkshell  = false;
    bool   recycleEnabled = false;
    bool   noRecycle      = false;

    // The gil slot returns before storage lookup in the original handler.
    const bool gil = this->Category == LOC_INVENTORY && this->ItemIndex == 0;
    if (!gil)
    {
        PItem = PChar->getStorage(this->Category)->GetItem(this->ItemIndex);
        locked = PItem && PItem->isSubType(ITEM_LOCKED);
        if (PItem && !locked)
        {
            available = PItem->getQuantity() - PItem->getReserve();
            if (available >= this->ItemNum && PItem->isStorageSlip())
            {
                // Retail accurate: Slips with stored items cannot be thrown away.
                int slipData = 0;
                for (int i = 0; i < CItem::extra_size; i++)
                {
                    slipData += PItem->m_extra[i];
                }

                storedSlip = slipData != 0;
            }

            if (available >= this->ItemNum && !storedSlip)
            {
                auto* itemLinkshell = dynamic_cast<CItemLinkshell*>(PItem);
                mainLinkshell       = itemLinkshell && itemLinkshell->GetLSType() == LSTYPE_LINKSHELL;
                recycleEnabled      = settings::get<bool>("map.ENABLE_ITEM_RECYCLE_BIN");
                noRecycle           = PItem->hasFlag(ItemFlag::NoRecycle);
            }
        }
    }

    const auto plan = itemdump::PlanFor(this->Category, this->ItemIndex, this->ItemNum, PItem != nullptr, locked, available, storedSlip, mainLinkshell, recycleEnabled, noRecycle);

    if (plan.action == itemdump::Action::Message)
    {
        if (gil)
        {
            PChar->pushPacket<GP_SERV_COMMAND_MESSAGE>(ITEMID::GIL, MsgStd::UnableToThrowAway);
        }
        else
        {
            PChar->pushPacket<GP_SERV_COMMAND_MESSAGE>(PItem->getID(), MsgStd::UnableToThrowAway);
        }
        return;
    }

    if (plan.action == itemdump::Action::Reject)
    {
        if (!PItem || locked)
        {
            ShowWarning("GP_CLI_COMMAND_ITEM_DUMP: Attempt of removal of invalid item from slot %u", this->ItemIndex);
        }
        else
        {
            ShowWarning("GP_CLI_COMMAND_ITEM_DUMP: Trying to drop too much quantity from location %u slot %u", this->Category, this->ItemIndex);
        }
        return;
    }

    auto* itemLinkshell = dynamic_cast<CItemLinkshell*>(PItem);

    // Break linkshell if the main shell was disposed of.
    if (plan.breakLinkshell)
    {
        const uint32 lsid       = itemLinkshell->GetLSID();
        CLinkshell*  PLinkshell = linkshell::GetLinkshell(lsid);
        if (!PLinkshell)
            PLinkshell = linkshell::LoadLinkshell(lsid);
        PLinkshell->BreakLinkshell();
        linkshell::UnloadLinkshell(lsid);
    }

    // Retail accurate: Any item dropped from a container other than inventory skips the recycle bin.
    // Items with the NoRecycle flag bypass the recycle bin entirely (e.g. linkshells).
    if (plan.action == itemdump::Action::Drop)
    {
        charutils::DropItem(PChar, this->Category, this->ItemIndex, this->ItemNum, PItem->getID());
        return;
    }

    // Otherwise, to the recycle bin!
    // Note: AddItemToRecycleBin moves the whole item without using ItemNum which is not retail accurate.
    charutils::AddItemToRecycleBin(PChar, this->Category, this->ItemIndex, this->ItemNum);
}
