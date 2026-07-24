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

#include "inactive_state.h"
#include "inactive_duration_exit.h"
#include "inactive_interrupt.h"
#include "inactive_zero_duration_exit.h"
#include "ai/ai_container.h"
#include "entities/battle_entity.h"
#include "status_effect_container.h"

CInactiveState::CInactiveState(CBaseEntity* PEntity, timer::duration _duration, bool canChangeState, bool untargetable)
: CState(PEntity, 0)
, m_duration(_duration)
, m_canChangeState(canChangeState)
, m_untargetable(untargetable)
{
    if (inactiveinterrupt::shouldInterrupt(canChangeState))
    {
        PEntity->PAI->InterruptStates();
    }
}

bool CInactiveState::Update(timer::time_point tick)
{
    auto* PBattleEntity{ dynamic_cast<CBattleEntity*>(m_PEntity) };
    if (PBattleEntity && m_duration == 0ms)
    {
        if (inactivezerodurationexit::shouldExit(
                [&]()
                {
                    return PBattleEntity->isDead();
                },
                [&]()
                {
                    return PBattleEntity->StatusEffectContainer->HasPreventActionEffect();
                },
                [&]()
                {
                    return PBattleEntity->StatusEffectContainer->HasStatusEffect({ xi::StatusEffect::CharmI, xi::StatusEffect::CharmIi });
                },
                [&]()
                {
                    return PBattleEntity->StatusEffectContainer->HasPreventActionEffect(true);
                }))
        {
            return true;
        }
    }

    // Positive-duration tail (pure gate: inactivedurationexit); host precomputes
    // duration > 0ms and strict tick > entry + duration (slice 6301 dual-wire).
    return inactivedurationexit::shouldExit(m_duration > 0ms, tick > GetEntryTime() + m_duration);
}

void CInactiveState::Cleanup(timer::time_point tick)
{
}
