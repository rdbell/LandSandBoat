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

#include "0x053_lockstyle.h"

#include "enums/msg_std.h"
#include "items/item_equipment.h"
#include "lockstyle_set_conflict_capacity.h"
#include "lockstyle_set_item_capacity.h"
#include "lockstyle_set_style_update_capacity.h"
#include "packets/char_sync.h"
#include "packets/s2c/0x009_message.h"
#include "packets/s2c/0x051_grap_list.h"
#include "utils/charutils.h"
#include "utils/itemutils.h"

// Go host pure half: packetsystem.ValidateLockStyle / ProcessLockStyle /
// NewLockStyleHandler (6465); non-Set plan mappacket.ClientLockStylePacket.RuntimePlan.

namespace
{

const auto updateClientAppearance = [](CCharEntity* PChar)
{
    PChar->pushPacket<GP_SERV_COMMAND_GRAP_LIST>(PChar);
    PChar->pushPacket<CCharSyncPacket>(PChar);
};

} // namespace

auto GP_CLI_COMMAND_LOCKSTYLE::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .oneOf<GP_CLI_COMMAND_LOCKSTYLE_MODE>(this->Mode)
        .range("Count", this->Count, 0, 16);
}

void GP_CLI_COMMAND_LOCKSTYLE::process(MapSession* PSession, CCharEntity* PChar) const
{
    bool hasH2HInMainSlot = false;

    switch (static_cast<GP_CLI_COMMAND_LOCKSTYLE_MODE>(this->Mode))
    {
        case GP_CLI_COMMAND_LOCKSTYLE_MODE::Disable:
        {
            if (PChar->getStyleLocked())
            {
                charutils::SetStyleLock(PChar, false);
                PChar->RequestPersist(CHAR_PERSIST::EQUIP);
                updateClientAppearance(PChar);
            }
        }
        break;
        case GP_CLI_COMMAND_LOCKSTYLE_MODE::Continue:
        {
            PChar->setStyleLocked(true);
        }
        break;
        case GP_CLI_COMMAND_LOCKSTYLE_MODE::Query:
        {
            PChar->pushPacket<GP_SERV_COMMAND_MESSAGE>(PChar->getStyleLocked() ? MsgStd::StyleLockIsOn : MsgStd::StyleLockIsOff);
        }
        break;
        case GP_CLI_COMMAND_LOCKSTYLE_MODE::Set:
        {
            // TODO: First, move this to charutils.
            // TODO: Missing a handful of retail checks here
            charutils::SetStyleLock(PChar, true);

            // Build new lockstyle
            for (int i = 0; i < this->Count; i++)
            {
                const auto& item   = this->Items[i];
                uint16_t    itemId = item.ItemNo;

                // Skip non-visible items
                if (item.EquipKind > SLOT_FEET)
                {
                    continue;
                }

                const auto* PItem = xi::items::lookup<CItemEquipment>(itemId);
                const bool  isVisibleItem = PItem != nullptr && (PItem->isType(ITEM_WEAPON) || PItem->isType(ITEM_EQUIPMENT));
                const auto* PItemWeapon   = i == SLOT_MAIN ? dynamic_cast<const CItemWeapon*>(PItem) : nullptr;
                const auto plan = lockstylesetitemhelpers::PlanFor({
                    .packetIndex   = static_cast<std::uint8_t>(i),
                    .equipKind     = item.EquipKind,
                    .itemID        = itemId,
                    .itemFound     = PItem != nullptr,
                    .isVisibleItem = isVisibleItem,
                    .fitsEquipKind = isVisibleItem && (PItem->getEquipSlotId() & (1 << item.EquipKind)) != 0,
                    .isHandToHand  = PItemWeapon != nullptr && PItemWeapon->isHandToHand(),
                });

                PChar->styleItems[plan.styleSlot] = plan.styleItemID;

                if (plan.mainHasH2H)
                {
                    hasH2HInMainSlot = true;
                }
            }

            // Check if we need to remove conflicting slots. Essentially, packet injection shenanigan detector.
            auto styleItems = std::array<std::uint16_t, lockstylesetconflicthelpers::StyleSlotCount>{};
            auto items      = std::array<lockstylesetconflicthelpers::Item, lockstylesetconflicthelpers::ScannedStyleSlots + 1>{};
            for (std::size_t i = 0; i < styleItems.size(); ++i)
            {
                styleItems[i] = PChar->styleItems[i];
                if (i < lockstylesetconflicthelpers::ScannedStyleSlots)
                {
                    const auto* PItemEquipment = xi::items::lookup<CItemEquipment>(styleItems[i]);
                    items[i]                   = {
                        .itemID      = styleItems[i],
                        .found       = PItemEquipment != nullptr,
                        .removeSlots = PItemEquipment ? PItemEquipment->getRemoveSlotId() : std::uint16_t{ 0 },
                    };
                }
            }
            if (const auto* PEmptyItem = xi::items::lookup<CItemEquipment>(0))
            {
                items.back() = {
                    .itemID      = 0,
                    .found       = true,
                    .removeSlots = PEmptyItem->getRemoveSlotId(),
                };
            }
            const auto conflictPlan = lockstylesetconflicthelpers::PlanFor(styleItems, items);
            for (std::size_t i = 0; i < conflictPlan.styleItems.size(); ++i)
            {
                PChar->styleItems[i] = conflictPlan.styleItems[i];
            }

            const auto styleUpdatePlan = lockstylesetstyleupdatehelpers::PlanFor(hasH2HInMainSlot);
            for (std::size_t i = 0; i < styleUpdatePlan.actionCount; ++i)
            {
                const auto& action = styleUpdatePlan.actions[i];
                auto* PItem         = PChar->getEquip(static_cast<SLOTTYPE>(action.slot));

                switch (action.kind)
                {
                    case lockstylesetstyleupdatehelpers::ActionKind::Weapon:
                        charutils::UpdateWeaponStyle(PChar, action.slot, PItem);
                        break;
                    case lockstylesetstyleupdatehelpers::ActionKind::Armor:
                        charutils::UpdateArmorStyle(PChar, action.slot);
                        break;
                }
            }

            charutils::UpdateRemovedSlotsLookForLockStyle(PChar);
            PChar->RequestPersist(CHAR_PERSIST::EQUIP);
            updateClientAppearance(PChar);
        }
        break;
        case GP_CLI_COMMAND_LOCKSTYLE_MODE::Enable:
        {
            charutils::SetStyleLock(PChar, true);
            charutils::UpdateRemovedSlotsLookForLockStyle(PChar);
            PChar->RequestPersist(CHAR_PERSIST::EQUIP);
            updateClientAppearance(PChar);
        }
        break;
    }
}
