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

#include "0x105_bazaar_list.h"

#include "common/utils.h"
#include "common/vana_time.h"
#include "items/item_usable.h"
#include "utils/itemutils.h"

GP_SERV_COMMAND_BAZAAR_LIST::GP_SERV_COMMAND_BAZAAR_LIST(CItem* PItem, const uint8 slotId, const uint16 tax)
{
    auto& packet = this->data();

    bazaarlisthelpers::ItemFacts facts{};

    if (PItem != nullptr)
    {
        facts.present  = true;
        facts.price    = PItem->getCharPrice();
        facts.quantity = PItem->getQuantity();
        facts.itemId   = PItem->getID();

        if (PItem->isSubType(ITEM_CHARGED) && PItem->isType(ITEM_USABLE))
        {
            const timer::time_point currentTime = timer::now();
            const timer::time_point nextUseTime = static_cast<CItemUsable*>(PItem)->getNextUseTime();

            facts.chargedUsable    = true;
            facts.charges          = static_cast<CItemUsable*>(PItem)->getCurrentCharges();
            facts.nextUseFuture    = nextUseTime > currentTime;
            facts.nextUseTimestamp = earth_time::vanadiel_timestamp(timer::to_utc(nextUseTime));
            facts.delayTimestamp   = static_cast<uint32>(timer::count_seconds(static_cast<CItemUsable*>(PItem)->getUseDelay()) + earth_time::vanadiel_timestamp());
        }
        else
        {
            std::memcpy(facts.extra.data(), PItem->m_extra, std::min<size_t>(CItem::extra_size, facts.extra.size()));
        }
    }
    const auto plan  = bazaarlisthelpers::PlanFor(slotId, tax, facts);
    packet.Price     = plan.price;
    packet.ItemNum   = plan.quantity;
    packet.TaxRate   = plan.tax;
    packet.ItemNo    = plan.itemId;
    packet.ItemIndex = plan.slot;
    std::memcpy(packet.Attr, plan.attr.data(), sizeof(packet.Attr));
}
