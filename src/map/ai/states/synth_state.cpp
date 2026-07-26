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

#include "synth_state.h"

#include "entities/battle_entity.h"

#include "ai/ai_container.h"
#include "ai/states/synth_update.h"
#include "utils/synthutils.h"

CSynthState::CSynthState(CCharEntity* PChar, SKILLTYPE skill)
: CState(PChar, PChar->targid)
, m_PEntity(PChar)
{
    const auto speedMod = synthupdate::speedModID(static_cast<uint8>(skill));
    if (speedMod != Mod::NONE)
    {
        m_synthFinishTime -= std::chrono::milliseconds(PChar->getMod(speedMod));
    }
}

bool CSynthState::Update(timer::time_point tick)
{
    const bool isDead = m_PEntity->isDead();
    switch (synthupdate::updateAction(isDead, !isDead && SynthReady()))
    {
        case synthupdate::UpdateAction::CriticalFail:
            synthutils::doSynthCriticalFail(m_PEntity);
            return true;

        case synthupdate::UpdateAction::Done:
            synthutils::sendSynthDone(m_PEntity);
            return true;

        case synthupdate::UpdateAction::Countdown:
            m_synthFinishTime -= (m_PEntity->PAI->getTick() - m_PEntity->PAI->getPrevTick());
            return false;
    }

    return false; // unreachable; keeps compilers without exhaustive-switch analysis happy
}

void CSynthState::Cleanup(timer::time_point tick)
{
    std::ignore = tick;
}

void CSynthState::UpdateTarget(CBaseEntity* target)
{
    std::ignore = target;
}

// stub
void CSynthState::UpdateTarget(uint16 targid)
{
    std::ignore = targid;
}

bool CSynthState::SynthReady()
{
    // Dual-wire: synthupdate::isReady (slice 6315)
    const bool remainingNegative = m_synthFinishTime < 0ms;
    const bool isAlive           = m_PEntity->isAlive();
    return synthupdate::isReady(remainingNegative, isAlive);
}
