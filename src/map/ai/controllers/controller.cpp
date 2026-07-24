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

#include "controller.h"
#include "controller_action_dispatch_capacity.h"

#include "ai/ai_container.h"
#include "entities/battle_entity.h"

CController::CController(CBattleEntity* _POwner)
: m_Tick(timer::now())
, POwner(_POwner)
{
}

void CController::Despawn()
{
    // Dual-wire: controlleractiondispatch::Dispatch owner gate (slice 6363).
    // Go host pure half: controller.Despawn runs Internal_Despawn when hasOwner.
    (void)controlleractiondispatch::Dispatch(POwner != nullptr, [&]() {
        POwner->PAI->Internal_Despawn();
        return true;
    });
}

void CController::Reset()
{
}

bool CController::Cast(uint16 targid, SpellID spellid)
{
    return controlleractiondispatch::Dispatch(
        POwner != nullptr, [&]() { return POwner->PAI->Internal_Cast(targid, spellid); });
}

bool CController::Engage(uint16 targid)
{
    return controlleractiondispatch::Dispatch(
        POwner != nullptr, [&]() { return POwner->PAI->Internal_Engage(targid); });
}

bool CController::ChangeTarget(uint16 targid)
{
    return controlleractiondispatch::Dispatch(
        POwner != nullptr, [&]() { return POwner->PAI->Internal_ChangeTarget(targid); });
}

bool CController::Disengage()
{
    return controlleractiondispatch::Dispatch(
        POwner != nullptr, [&]() { return POwner->PAI->Internal_Disengage(); });
}

bool CController::WeaponSkill(uint16 targid, uint16 wsid)
{
    return controlleractiondispatch::Dispatch(
        POwner != nullptr, [&]() { return POwner->PAI->Internal_WeaponSkill(targid, wsid); });
}

bool CController::RangedAttack(uint16 targid)
{
    return controlleractiondispatch::Dispatch(
        POwner != nullptr, [&]() { return POwner->PAI->Internal_RangedAttack(targid); });
}

bool CController::IsAutoAttackEnabled() const
{
    return m_AutoAttackEnabled;
}

void CController::SetAutoAttackEnabled(bool enabled)
{
    m_AutoAttackEnabled = enabled;
}

bool CController::IsRangedAttackEnabled() const
{
    return m_RangedAttackEnabled;
}

void CController::SetRangedAttackEnabled(bool enabled)
{
    m_RangedAttackEnabled = enabled;
}

bool CController::IsWeaponSkillEnabled() const
{
    return m_WeaponSkillEnabled;
}

void CController::SetWeaponSkillEnabled(bool enabled)
{
    m_WeaponSkillEnabled = enabled;
}

bool CController::IsMagicCastingEnabled() const
{
    return m_MagicCastingEnabled;
}

void CController::SetMagicCastingEnabled(bool enabled)
{
    m_MagicCastingEnabled = enabled;
}
