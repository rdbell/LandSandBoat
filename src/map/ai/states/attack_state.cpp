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

#include "attack_state.h"

#include "action/action.h"
#include "attack_state_capacity.h"
#include "entities/battle_entity.h"

#include "ai/ai_container.h"
#include "packets/s2c/0x028_battle2.h"
#include "packets/s2c/0x058_assist.h"
#include "utils/battleutils.h"

CAttackState::CAttackState(CBattleEntity* PEntity, uint16 targid)
: CState(PEntity, targid)
, m_PEntity(PEntity)
{
    PEntity->SetBattleTargetID(targid);
    PEntity->SetBattleStartTime(timer::now());
    CAttackState::UpdateTarget();

    if (attackstatehelpers::ShouldThrowInitNoTarget(GetTarget() != nullptr, m_errorMsg != nullptr))
    {
        PEntity->SetBattleTargetID(0);
        if (this->HasErrorMsg())
        {
            throw CStateInitException(m_errorMsg->copy());
        }
        else
        {
            throw CStateInitException(std::make_unique<CBasicPacket>());
        }
    }

    if (attackstatehelpers::ShouldClearPathOnEnter(PEntity->PAI->PathFind != nullptr))
    {
        PEntity->PAI->PathFind->Clear();
    }
}

bool CAttackState::Update(timer::time_point tick)
{
    using namespace attackstatehelpers;

    auto* PTarget = static_cast<CBattleEntity*>(GetTarget());
    if (ShouldExitNoTarget(PTarget != nullptr, PTarget && PTarget->isDead()))
    {
        return true;
    }
    if (AttackReady())
    {
        if (CanAttack(PTarget))
        {
            // CanAttack may have set target id to 0 (disengage from out of range)
            if (ShouldDisengageBattleTargetZero(m_PEntity->GetBattleTargetID() == 0))
            {
                return true;
            }
            action_t action{};
            if (m_PEntity->OnAttack(*this, action))
            {
                // TODO: what about AoE auto attacks?
                battleutils::handleKillshotEnmity(m_PEntity, PTarget);

                // CMobEntity::OnAttack(...) can generate it's own action with a mobmod, and that leaves this action.actionType = 0, which is never valid. Skip sending the packet.
                if (ShouldSendAttackPacket(static_cast<uint8>(action.actiontype)))
                {
                    m_PEntity->loc.zone->PushPacket(m_PEntity, CHAR_INRANGE_SELF, std::make_unique<GP_SERV_COMMAND_BATTLE2>(action));
                }
            }
        }
        else if (m_PEntity->OnAttackError(*this))
        {
            m_PEntity->HandleErrorMessage(m_errorMsg);
        }
        if (ShouldExitAfterReadyPath(m_PEntity->GetBattleTargetID() == 0))
        {
            return true;
        }
    }
    else
    {
        // Pure gate: ShouldSubtractAttackElapsed(!AttackReady()) — always true here.
        m_attackTime -= (m_PEntity->PAI->getTick() - m_PEntity->PAI->getPrevTick());
    }
    return false;
}

void CAttackState::Cleanup(timer::time_point tick)
{
    if (attackstatehelpers::ShouldCleanupDisengage(m_PEntity->isDead()))
    {
        m_PEntity->OnDisengage(*this);
    }
}

void CAttackState::ResetAttackTimer()
{
    m_attackTime = std::chrono::milliseconds(m_PEntity->GetWeaponDelay(false));
}

void CAttackState::UpdateTarget(CBaseEntity* target)
{
    if (target != nullptr)
    {
        CAttackState::UpdateTarget(target->targid);
    }
}

void CAttackState::UpdateTarget(uint16 targid)
{
    using namespace attackstatehelpers;

    m_errorMsg.reset();
    auto           newTargid{ m_PEntity->GetBattleTargetID() };
    CBattleEntity* PNewTarget{ nullptr };
    if (ShouldResolveBattleTarget(newTargid))
    {
        PNewTarget = m_PEntity->IsValidTarget(newTargid, TARGET_ENEMY, m_errorMsg);
        if (!PNewTarget)
        {
            newTargid          = 0;
            CCharEntity* PChar = dynamic_cast<CCharEntity*>(m_PEntity);
            if (ShouldTryAutoTarget(PChar != nullptr, PChar && PChar->hasAutoTargetEnabled()))
            {
                for (auto&& PPotentialTarget : PChar->SpawnMOBList)
                {
                    if (AutoTargetCandidate(
                            PPotentialTarget.second->animation,
                            facing(PChar->loc.p, PPotentialTarget.second->loc.p, AutoTargetFacingCone),
                            distance(PChar->loc.p, PPotentialTarget.second->loc.p)))
                    {
                        std::unique_ptr<CBasicPacket> errMsg;
                        if (PChar->IsValidTarget(PPotentialTarget.second->targid, TARGET_ENEMY, errMsg))
                        {
                            newTargid = PPotentialTarget.second->targid;
                            PChar->pushPacket<GP_SERV_COMMAND_ASSIST>(PChar, static_cast<CBattleEntity*>(PPotentialTarget.second));
                            break;
                        }
                    }
                }
            }
            m_PEntity->PAI->ChangeTarget(newTargid);
        }
    }
    if (ShouldChangeTarget(targid, newTargid))
    {
        if (ShouldNotifyChangeTarget(targid, newTargid))
        {
            m_PEntity->OnChangeTarget(PNewTarget);
            SetTarget(newTargid);
            if (ShouldClearErrorOnMissingNewTarget(PNewTarget != nullptr))
            {
                m_errorMsg.reset();
                return;
            }
        }
    }
    CState::UpdateTarget(m_PEntity->GetBattleTargetID());
}

bool CAttackState::CanAttack(CBattleEntity* PTarget)
{
    auto ret = m_PEntity->CanAttack(PTarget, m_errorMsg);

    if (attackstatehelpers::ShouldAddWeaponDelayAfterCanAttack(ret, m_errorMsg != nullptr))
    {
        m_attackTime += std::chrono::milliseconds(m_PEntity->GetWeaponDelay(false));
    }
    return ret;
}

bool CAttackState::AttackReady()
{
    return m_attackTime < 0ms && m_PEntity->isAlive();
}
