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

#include "0x0c9_equip_inspect_equipment.h"

#include "common/vana_time.h"
#include "entities/char_entity.h"
#include "enums/packet_c2s.h"
#include "items/exdata/augment_standard.h"
#include "items/item_equipment.h"
#include "items/item_usable.h"

GP_SERV_COMMAND_EQUIP_INSPECT::EQUIPMENT::EQUIPMENT(CCharEntity* PChar, CCharEntity* PTarget)
{
    auto& packet = this->data();

    packet.UniqNo     = PTarget->id;
    packet.ActIndex   = PTarget->targid;
    packet.OptionFlag = 0x03;

    uint8 count = 0;

    for (int32 i = 0; i < 16; ++i)
    {
        if (CItem* PItem = PTarget->getEquip(static_cast<SLOTTYPE>(i)))
        {
            auto facts = equipinspecthelpers::CheckItemFacts{
                .itemNo    = PItem->getID(),
                .equipKind = static_cast<uint8>(i),
                .charged   = PItem->isSubType(ITEM_CHARGED),
                .augmented = PItem->isSubType(ITEM_AUGMENTED),
            };

            if (facts.charged)
            {
                timer::time_point currentTime = timer::now();
                timer::time_point nextUseTime = static_cast<CItemUsable*>(PItem)->getNextUseTime();

                facts.currentCharges    = static_cast<CItemUsable*>(PItem)->getCurrentCharges();
                facts.nextUseIsFuture   = nextUseTime > currentTime;
                facts.nextUseTimestamp  = earth_time::vanadiel_timestamp(timer::to_utc(nextUseTime));
                facts.useDelayTimestamp = static_cast<uint32>(timer::count_seconds(static_cast<CItemUsable*>(PItem)->getUseDelay()) + earth_time::vanadiel_timestamp());
            }

            if (facts.augmented)
            {
                for (std::size_t index = 0; index < facts.augments.size(); ++index)
                {
                    facts.augments[index] = static_cast<CItemEquipment*>(PItem)->getAugment(index);
                }
            }

            std::memcpy(facts.signature.data(), PItem->exdata<Exdata::AugmentStandard>().Signature, facts.signature.size());
            const auto plan = equipinspecthelpers::CheckItemPlanFor(facts);
            auto&      item = packet.Equip[count];
            item.ItemNo     = plan.itemNo;
            item.EquipKind  = static_cast<SAVE_EQUIP_KIND>(plan.equipKind);
            std::memcpy(item.Data, plan.data.data(), plan.data.size());

            count++;

            if (count == 8)
            {
                packet.EquipCount = count;
                this->setSize(sizeof(GP_SERV_HEADER) + 8 + sizeof(packet.Equip[0]) * count);
                PChar->pushPacket(this->copy());

                // Reset for next batch
                std::memset(packet.Equip, 0, sizeof(packet.Equip));
                count = 0;
            }
        }
    }

    packet.EquipCount = count;
    this->setSize(sizeof(GP_SERV_HEADER) + 8 + sizeof(packet.Equip[0]) * std::max<uint8>(count, 1));
}
