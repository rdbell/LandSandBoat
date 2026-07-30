/*
===========================================================================

  Copyright (c) 2010-2015 Darkstar Dev Teams

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

#include "entities/battle_entity.h"

#include "entities/char_entity.h"
#include "items/item_weapon.h"
#include "latent_action_plan.h"
#include "latent_effect.h"
#include "packets/s2c/0x0ac_command_data.h"
#include "status_effect_container.h"
#include "utils/charutils.h"

CLatentEffect::CLatentEffect(CBattleEntity* owner, xi::Latent conditionsId, uint16 conditionsValue, uint8 slot, Mod modValue, int16 modPower)
: m_POwner(owner)
, m_ConditionsID(conditionsId)
, m_ConditionsValue(conditionsValue)
, m_SlotID(slot)
, m_ModValue(modValue)
, m_ModPower(modPower)
{
}

CLatentEffect::~CLatentEffect()
{
    if (m_Activated)
    {
        Deactivate();
    }
}

auto CLatentEffect::GetConditionsID() const -> xi::Latent
{
    return m_ConditionsID;
}

uint16 CLatentEffect::GetConditionsValue() const
{
    return m_ConditionsValue;
}

uint8 CLatentEffect::GetSlot() const
{
    return m_SlotID;
}

Mod CLatentEffect::GetModValue() const
{
    return m_ModValue;
}

int16 CLatentEffect::GetModPower() const
{
    return m_ModPower;
}

bool CLatentEffect::IsActivated() const
{
    return m_Activated;
}

CBattleEntity* CLatentEffect::GetOwner() const
{
    return m_POwner;
}

void CLatentEffect::SetConditionsId(xi::Latent id)
{
    m_ConditionsID = id;
}

void CLatentEffect::SetConditionsValue(uint16 value)
{
    m_ConditionsValue = value;
}

void CLatentEffect::SetSlot(uint8 slot)
{
    m_SlotID = slot;
}

void CLatentEffect::SetModValue(Mod value)
{
    m_ModValue = value;
}

void CLatentEffect::SetModPower(int16 power)
{
    m_ModPower = power;
}

bool CLatentEffect::ModOnItemOnly(Mod modID)
{
    if (modID == Mod::DMG_RATING ||
        modID == Mod::ITEM_ADDEFFECT_TYPE ||
        modID == Mod::ITEM_SUBEFFECT ||
        modID == Mod::ITEM_ADDEFFECT_DMG ||
        modID == Mod::ITEM_ADDEFFECT_CHANCE ||
        modID == Mod::ITEM_ADDEFFECT_ELEMENT ||
        modID == Mod::ITEM_ADDEFFECT_STATUS ||
        modID == Mod::ITEM_ADDEFFECT_POWER ||
        modID == Mod::ITEM_ADDEFFECT_DURATION ||
        modID == Mod::ADDS_WEAPONSKILL ||
        modID == Mod::MOVE_SPEED_GEAR_BONUS ||
        modID == Mod::CRITHITRATE_ONLY_WEP)
    {
        return true;
    }
    return false;
}

bool CLatentEffect::Activate()
{
    const auto itemOnly = ModOnItemOnly(GetModValue());
    auto*      PChar    = static_cast<CCharEntity*>(nullptr);
    auto*      item     = static_cast<CItemEquipment*>(nullptr);

    if (!IsActivated() && itemOnly)
    {
        PChar = static_cast<CCharEntity*>(m_POwner);
        item  = PChar->getEquip((SLOTTYPE)GetSlot());
    }

    const auto plan = latenthelpers::PlanLatentActivation(IsActivated(), itemOnly, item != nullptr);
    return latenthelpers::ApplyLatentActivationPlan(
        plan,
        [this, PChar, item](const latenthelpers::LatentActivationAction action)
        {
            switch (action)
            {
                case latenthelpers::LatentActivationAction::AddItemModifier:
                    item->addModifier(GetModValue(), GetModPower());
                    break;
                case latenthelpers::LatentActivationAction::RebuildWeaponSkills:
                    charutils::BuildingCharWeaponSkills(PChar);
                    break;
                case latenthelpers::LatentActivationAction::PushCommandData:
                    PChar->pushPacket<GP_SERV_COMMAND_COMMAND_DATA>(PChar);
                    break;
                case latenthelpers::LatentActivationAction::RememberItem:
                    m_PItem = item;
                    break;
                case latenthelpers::LatentActivationAction::AddOwnerModifier:
                    m_POwner->addModifier(m_ModValue, m_ModPower);
                    break;
                case latenthelpers::LatentActivationAction::MarkActivated:
                    m_Activated = true;
                    break;
            }
        });
}

bool CLatentEffect::Deactivate()
{
    const auto plan = latenthelpers::PlanLatentDeactivation(IsActivated(), ModOnItemOnly(GetModValue()), m_PItem != nullptr);
    return latenthelpers::ApplyLatentDeactivationPlan(
        plan,
        [this](const latenthelpers::LatentDeactivationAction action)
        {
            switch (action)
            {
                case latenthelpers::LatentDeactivationAction::RemoveItemModifier:
                    m_PItem->delModifier(GetModValue(), GetModPower());
                    break;
                case latenthelpers::LatentDeactivationAction::RebuildWeaponSkills:
                    charutils::BuildingCharWeaponSkills(static_cast<CCharEntity*>(m_POwner));
                    break;
                case latenthelpers::LatentDeactivationAction::PushCommandData:
                {
                    auto* PChar = static_cast<CCharEntity*>(m_POwner);
                    PChar->pushPacket<GP_SERV_COMMAND_COMMAND_DATA>(PChar);
                    break;
                }
                case latenthelpers::LatentDeactivationAction::RemoveOwnerModifier:
                    m_POwner->delModifier(m_ModValue, m_ModPower);
                    break;
                case latenthelpers::LatentDeactivationAction::MarkDeactivated:
                    m_Activated = false;
                    break;
            }
        });
}
