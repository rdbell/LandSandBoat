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

#include "0x0fd_myroom_plant_check.h"

#include "entities/char_entity.h"
#include "items/item_flowerpot.h"
#include "packets/s2c/0x029_battle_message.h"
#include "packets/s2c/0x0fa_myroom_operation.h"
#include "utils/charutils.h"

namespace
{

const std::set<uint8_t> validPlantCategories = { LOC_MOGSAFE, LOC_MOGSAFE2 };

}

auto myroomplantcheck::PlanFor(const Facts& facts) -> Plan
{
    if (!facts.isFlowerpot)
    {
        return {};
    }

    auto plan = Plan{
        .sendMyRoomOperation = true,
    };

    if (!facts.isPlanted)
    {
        return plan;
    }

    plan.outputs.push_back({ Message::SeedSown, facts.seedItemID });

    if (facts.isTree && facts.stage > FLOWERPOT_STAGE_FIRST_SPROUTS_CRYSTAL)
    {
        plan.outputs.push_back({ facts.extraCrystalItemID != 0 ? Message::CrystalUsed : Message::CrystalNone, facts.extraCrystalItemID });
    }

    if (facts.stage > FLOWERPOT_STAGE_SECOND_SPROUTS_CRYSTAL)
    {
        plan.outputs.push_back({ facts.commonCrystalItemID != 0 ? Message::CrystalUsed : Message::CrystalNone, facts.commonCrystalItemID });
    }

    plan.markExamined = !facts.wasExamined;
    return plan;
}

// Go host pure half: packetsystem.ValidateMyRoomPlantCheck / ProcessMyRoomPlantCheck / NewMyRoomPlantCheckHandler (6535); plan mappacket.ClientMyRoomPlantCheckRuntimePlanFor.
auto GP_CLI_COMMAND_MYROOM_PLANT_CHECK::validate(MapSession* PSession, const CCharEntity* PChar) const -> PacketValidationResult
{
    return PacketValidator(PChar)
        .blockedBy({ BlockedState::InEvent })
        .mustNotEqual(this->MyroomPlantItemNo, 0, "MyroomPlantItemNo must not be 0")
        .oneOf("MyroomPlantCategory", this->MyroomPlantCategory, validPlantCategories);
}

void GP_CLI_COMMAND_MYROOM_PLANT_CHECK::process(MapSession* PSession, CCharEntity* PChar) const
{
    CItemContainer* PItemContainer = PChar->getStorage(this->MyroomPlantCategory);
    auto*           PItem          = PItemContainer->GetItem(this->MyroomPlantItemIndex);
    auto*           PPotItem       = dynamic_cast<CItemFlowerpot*>(PItem);

    if (PPotItem == nullptr)
    {
        if (PItem)
        {
            ShowWarning(fmt::format("GP_CLI_COMMAND_MYROOM_PLANT_CHECK::process: {} has tried to use invalid gardening pot {} ({})",
                                    PChar->getName(),
                                    PItem->getID(),
                                    PItem->getName()));
            return;
        }
        else
        {
            ShowWarning(fmt::format("GP_CLI_COMMAND_MYROOM_PLANT_CHECK::process: {} has tried to use invalid gardening pot item (MyroomPlantCategory = {}, MyroomPlantItemIndex = {})",
                                    PChar->getName(),
                                    this->MyroomPlantCategory,
                                    this->MyroomPlantItemIndex));
        }
        return;
    }

    const auto crystalItemID = [](const FLOWERPOT_ELEMENT_TYPE element) -> uint16
    {
        return element == FLOWERPOT_ELEMENT_NONE ? 0 : static_cast<uint16>(CItemFlowerpot::getItemFromElement(element));
    };

    const auto plan = myroomplantcheck::PlanFor({
        .isFlowerpot          = true,
        .isPlanted            = PPotItem->isPlanted(),
        .isTree               = PPotItem->isTree(),
        .wasExamined          = PPotItem->wasExamined(),
        .stage                = static_cast<uint8_t>(PPotItem->getStage()),
        .seedItemID           = CItemFlowerpot::getSeedID(PPotItem->getPlant()),
        .extraCrystalItemID   = crystalItemID(PPotItem->getExtraCrystalFeed()),
        .commonCrystalItemID  = crystalItemID(PPotItem->getCommonCrystalFeed()),
    });

    for (const auto& output : plan.outputs)
    {
        switch (output.message)
        {
            case myroomplantcheck::Message::SeedSown:
                PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PChar, output.itemID, 0, MsgBasic::GardeningSeedSown);
                break;
            case myroomplantcheck::Message::CrystalUsed:
                PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PChar, output.itemID, 0, MsgBasic::GardeningCrystalUsed);
                break;
            case myroomplantcheck::Message::CrystalNone:
                PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PChar, 0, 0, MsgBasic::GardeningCrystalNone);
                break;
        }
    }

    if (plan.markExamined)
    {
        PPotItem->markExamined();

        db::preparedStmt("UPDATE char_inventory SET extra = ? WHERE charid = ? AND location = ? AND slot = ? LIMIT 1",
                         PPotItem->m_extra,
                         PChar->id,
                         PPotItem->getLocationID(),
                         PPotItem->getSlotID());
    }

    if (plan.sendMyRoomOperation)
    {
        PChar->pushPacket<GP_SERV_COMMAND_MYROOM_OPERATION>(PPotItem, static_cast<CONTAINER_ID>(this->MyroomPlantCategory), this->MyroomPlantItemIndex);
    }
}
