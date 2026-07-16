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

#include "0x020_item_attr.h"
#include "item_attr_runtime.h"

#include "common/vana_time.h"
#include "entities/char_entity.h"
#include "enums/item_lockflg.h"
#include "items/item_linkshell.h"
#include "utils/itemutils.h"

GP_SERV_COMMAND_ITEM_ATTR::GP_SERV_COMMAND_ITEM_ATTR(CItem* PItem, const CONTAINER_ID locationId, const uint8_t slotId, CItem* staleItem)
{
    auto& packet = this->data();
    auto facts       = itemattrhelpers::Facts{};

    if (PItem)
    {
        facts.hasItem = true;
        facts.quantity = PItem->getQuantity();
        facts.price = PItem->getCharPrice();
        facts.itemID = PItem->getID();
        facts.extra = std::to_array(PItem->m_extra);
        facts.charged = PItem->isSubType(ITEM_CHARGED);
        facts.unlockableWeapon = PItem->isType(ITEM_WEAPON) && static_cast<CItemWeapon*>(PItem)->isUnlockable();
        facts.locked = PItem->isSubType(ITEM_LOCKED);
        facts.linkshell = PItem->isType(ITEM_LINKSHELL);

        if (facts.charged)
        {
            auto* usable = static_cast<CItemUsable*>(PItem);
            facts.currentCharges = usable->getCurrentCharges();
            facts.maxCharges = usable->getMaxCharges();
            if (facts.currentCharges > 0)
            {
                facts.reuseReady = usable->getReuseTime() == 0s;
                if (!facts.reuseReady)
                {
                    facts.nextUseTimestamp = earth_time::vanadiel_timestamp(timer::to_utc(usable->getNextUseTime()));
                    facts.delayTimestamp = static_cast<uint32_t>(timer::count_seconds(usable->getUseDelay()) + earth_time::vanadiel_timestamp());
                }
            }
        }

        if (facts.linkshell)
        {
            facts.linkshellType = static_cast<CItemLinkshell*>(PItem)->GetLSType();
        }
    }
    else if (staleItem)
    {
        facts.hasStaleItem = true;
        facts.staleExtra = std::to_array(staleItem->m_extra);
        facts.leakStaleExtra = settings::get<bool>("map.LEAK_EXT_DATA_ON_ITEM_MOVE");
    }

    packet = itemattrhelpers::PlanFor(locationId, slotId, facts);
}

GP_SERV_COMMAND_ITEM_ATTR::GP_SERV_COMMAND_ITEM_ATTR(CItem* PItem, const ItemLocation& loc, CItem* staleItem)
: GP_SERV_COMMAND_ITEM_ATTR(PItem, loc.Container, loc.Slot, staleItem)
{
}
