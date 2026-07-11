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

#include "ability_state.h"

#include "ability.h"
#include "ability_state_capacity.h"
#include "action/action.h"
#include "action/interrupts.h"
#include "ai/ai_container.h"
#include "common/utils.h"
#include "enmity_container.h"
#include "entities/char_entity.h"
#include "entities/mob_entity.h"
#include "entities/pet_entity.h"
#include "packets/s2c/0x028_battle2.h"
#include "packets/s2c/0x029_battle_message.h"
#include "petskill.h"
#include "recast_container.h"
#include "status_effect_container.h"
#include "utils/battleutils.h"
#include "utils/charutils.h"
#include "utils/zoneutils.h"

namespace
{

// Handle Blood Pacts and Ready distance checks separately.
// They come in as the final ability to be used through the packets but must pass the intermediary ability distance before triggering
// Examples:
// Predator Claws in packet -> PC must pass Blood Pact: Rage (20y) distance check
// Lamb Chop in packet -> PC must pass Ready (4y) distance check
auto PetSkillDistanceCheck(CCharEntity* PChar, CBaseEntity* PTarget, const CAbility* PAbility) -> bool
{
    auto*            PPet      = dynamic_cast<CPetEntity*>(PChar->PPet);
    const CPetSkill* PPetSkill = battleutils::GetPetSkill(PAbility->getID());

    if (!PPet || !PPetSkill)
    {
        return false;
    }

    const bool isBloodPact = PPetSkill->isBloodPactRage() || PPetSkill->isBloodPactWard();
    const bool isJugReady  = PPetSkill->getMobSkillID() > 0;

    float petToEnemyDist = 0.f;
    float enemyHitbox    = 0.f;
    bool  hasPetTarget   = false;
    if (isJugReady && (PPetSkill->getValidTargets() & TARGET_ENEMY))
    {
        if (auto* PPetTarget = PPet->GetBattleTarget(); PPetTarget)
        {
            hasPetTarget   = true;
            petToEnemyDist = distance(PPet->loc.p, PPetTarget->loc.p);
            enemyHitbox    = PPetTarget->modelHitboxSize;
        }
    }

    const bool ok = abilitystatehelpers::EvaluatePetSkillDistance(
        true,
        isBloodPact,
        PChar == PTarget,
        distance(PChar->loc.p, PTarget->loc.p),
        PChar->modelHitboxSize,
        PTarget->modelHitboxSize,
        distance(PPet->loc.p, PTarget->loc.p),
        PPetSkill->getDistance(),
        PPet->modelHitboxSize,
        isJugReady,
        distance(PChar->loc.p, PPet->loc.p),
        (PPetSkill->getValidTargets() & TARGET_ENEMY) != 0,
        hasPetTarget,
        petToEnemyDist,
        enemyHitbox);

    if (!ok)
    {
        // Message selection mirrors original branches for out-of-range cases.
        if (isBloodPact)
        {
            if (abilitystatehelpers::BloodPactPCOutOfRange(
                    PChar == PTarget, distance(PChar->loc.p, PTarget->loc.p),
                    PChar->modelHitboxSize, PTarget->modelHitboxSize))
            {
                // Original: silent fail (no packet) for PC range
                return false;
            }
            PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PTarget, 0, 0, MsgBasic::TargetOutOfRange);
            return false;
        }
        if (isJugReady)
        {
            if (abilitystatehelpers::ReadyPCToPetOutOfRange(
                    distance(PChar->loc.p, PPet->loc.p), PChar->modelHitboxSize, PPet->modelHitboxSize))
            {
                PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PChar, 0, 0, MsgBasic::TargetOutOfRange);
                return false;
            }
            if (auto* PPetTarget = PPet->GetBattleTarget(); PPetTarget)
            {
                PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PPetTarget, 0, 0, MsgBasic::TargetOutOfRange);
            }
            return false;
        }
        return false;
    }

    return true;
}

} // namespace

CAbilityState::CAbilityState(CBattleEntity* PEntity, uint16 targid, uint16 abilityid)
: CState(PEntity, targid)
, m_PEntity(PEntity)
{
    CAbility* PAbility = ability::GetAbility(abilityid);

    if (!PAbility)
    {
        throw CStateInitException(std::make_unique<GP_SERV_COMMAND_BATTLE_MESSAGE>(m_PEntity, m_PEntity, 0, 0, MsgBasic::UnableToUseJobAbility));
    }
    auto* PTarget = m_PEntity->IsValidTarget(m_targid, PAbility->getValidTarget(), m_errorMsg);

    if (!PTarget || this->HasErrorMsg())
    {
        if (this->HasErrorMsg())
        {
            throw CStateInitException(m_errorMsg->copy());
        }
        else
        {
            throw CStateInitException(std::make_unique<CBasicPacket>());
        }
    }
    SetTarget(PTarget->targid);
    m_PAbility = std::make_unique<CAbility>(*PAbility);
    m_castTime = PAbility->getCastTime();

    if (m_castTime > 0s && CanUseAbility())
    {
        action_t action{
            .actorId    = PEntity->id,
            .actiontype = ActionCategory::AbilityStart,
            .targets    = {
                {
                    .actorId = PTarget->id,
                    .results = {
                        {
                            .animation = ActionAnimation::SkillStart,
                            .param     = PAbility->getID(),
                            .messageID = MsgBasic::ReadiesSkill,
                        },
                    },
                },
            }
        };

        PEntity->loc.zone->PushPacket(PEntity, CHAR_INRANGE_SELF, std::make_unique<GP_SERV_COMMAND_BATTLE2>(action));
        m_PEntity->PAI->EventHandler.triggerListener("ABILITY_START", m_PEntity, PAbility);

        // face toward target
        battleutils::turnTowardsTarget(m_PEntity, PTarget);
    }
    else
    {
        m_PEntity->PAI->EventHandler.triggerListener("ABILITY_START", m_PEntity, PAbility);
    }
}

CAbility* CAbilityState::GetAbility()
{
    return m_PAbility.get();
}

void CAbilityState::ApplyEnmity()
{
    auto* PTarget = GetTarget();
    if (!PTarget)
    {
        return;
    }

    if (abilitystatehelpers::ShouldUpdateHostileEnmity(
            (m_PAbility->getValidTarget() & TARGET_ENEMY) != 0,
            PTarget->allegiance != m_PEntity->allegiance,
            PTarget->objtype == TYPE_MOB,
            m_PAbility->getCE(),
            m_PAbility->getVE()))
    {
        CMobEntity* mob = (CMobEntity*)PTarget;
        mob->PEnmityContainer->UpdateEnmity(
            m_PEntity, m_PAbility->getCE(), m_PAbility->getVE(), false,
            abilitystatehelpers::IsCharmAbility(m_PAbility->getID()));
        battleutils::ClaimMob(mob, m_PEntity);
    }
    else if (abilitystatehelpers::ShouldGenerateAllyEnmity(PTarget->allegiance == m_PEntity->allegiance))
    {
        battleutils::GenerateInRangeEnmity(m_PEntity, m_PAbility->getCE(), m_PAbility->getVE());
    }
}

bool CAbilityState::CanChangeState()
{
    return IsCompleted();
}

bool CAbilityState::Update(timer::time_point tick)
{
    // Rotate towards target during ability
    if (m_castTime > 0s && tick < GetEntryTime() + m_castTime)
    {
        CBaseEntity* PTarget = GetTarget();
        if (PTarget)
        {
            battleutils::turnTowardsTarget(m_PEntity, PTarget);
        }
    }

    if (!IsCompleted() && tick > GetEntryTime() + m_castTime)
    {
        if (CanUseAbility())
        {
            action_t action{};
            m_PEntity->OnAbility(*this, action);
            m_PEntity->PAI->EventHandler.triggerListener("ABILITY_USE", m_PEntity, GetTarget(), m_PAbility.get(), &action);
            // Only send packet if action was populated (e.g. interrupts return early)
            if (!action.targets.empty())
            {
                m_PEntity->loc.zone->PushPacket(m_PEntity, CHAR_INRANGE_SELF, std::make_unique<GP_SERV_COMMAND_BATTLE2>(action));
            }
            for (auto& actionTarget : action.targets)
            {
                auto* PActionTarget = dynamic_cast<CBattleEntity*>(zoneutils::GetEntity(actionTarget.actorId));
                if (PActionTarget)
                {
                    PActionTarget->PAI->EventHandler.triggerListener("ABILITY_TAKE", m_PEntity, PActionTarget, m_PAbility.get(), &action);
                }
            }
        }

        Complete();
    }

    if (IsCompleted() && tick > GetEntryTime() + m_castTime + m_PAbility->getAnimationTime())
    {
        if (m_PEntity->objtype == TYPE_PC)
        {
            CCharEntity* PChar = static_cast<CCharEntity*>(m_PEntity);
            PChar->m_charHistory.abilitiesUsed++;
        }
        m_PEntity->PAI->EventHandler.triggerListener("ABILITY_STATE_EXIT", m_PEntity, m_PAbility.get());
        return true;
    }

    return false;
}

bool CAbilityState::CanUseAbility()
{
    CAbility*    PAbility = GetAbility();
    CBaseEntity* PTarget  = GetTarget();

    std::unique_ptr<CBasicPacket> errMsg;

    if (m_PEntity->objtype == TYPE_PC)
    {
        auto* PChar = static_cast<CCharEntity*>(m_PEntity);
        if (abilitystatehelpers::HasAbilityRecast(
                PChar->PRecastContainer->HasRecast(RECAST_ABILITY, PAbility->getRecastId(), PAbility->getRecastTime())))
        {
            PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PChar, 0, 0, MsgBasic::WaitLonger);
            return false;
        }

        const bool   hasAmnesia    = PChar->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Amnesia);
        const bool   hasImpairment = PChar->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Impairment);
        const uint16 impairPower   = hasImpairment ? PChar->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Impairment)->GetPower() : 0;
        const bool   hasAbility    = charutils::hasAbility(PChar, PAbility->getID());
        const bool   hasPetAbility = PAbility->isPetAbility() && PAbility->getID() >= ABILITY_HEALING_RUBY
                                         ? charutils::hasPetAbility(PChar, PAbility->getID() - ABILITY_HEALING_RUBY)
                                         : false;

        if (abilitystatehelpers::StatusBlocksAbility(hasAmnesia, hasImpairment, impairPower) ||
            abilitystatehelpers::PCLacksAbilityAccess(
                PAbility->isPetAbility(), PAbility->getID(), hasAbility, hasPetAbility))
        {
            PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PChar, 0, 0, MsgBasic::UnableToUseJobAbility2);
            return false;
        }

        if (PTarget && PChar->IsValidTarget(PTarget->targid, PAbility->getValidTarget(), errMsg))
        {
            // TODO: Rework the way abilities and pet abilities are laid out so it can all go through the same block and have the pet special checks done in lua
            const CPetSkill* PPetSkill = PAbility->isPetAbility() ? battleutils::GetPetSkill(PAbility->getID()) : nullptr;
            if (PPetSkill && abilitystatehelpers::NeedsPetSkillDistanceCheck(
                                 PAbility->isPetAbility(),
                                 true,
                                 PPetSkill->isBloodPactRage(),
                                 PPetSkill->isBloodPactWard(),
                                 PPetSkill->getMobSkillID() > 0,
                                 PAbility->getID()))
            {
                if (!PetSkillDistanceCheck(PChar, PTarget, PAbility))
                {
                    return false;
                }
            }
            else if (PChar != PTarget &&
                     abilitystatehelpers::OutOfAbilityRange(
                         distance(PChar->loc.p, PTarget->loc.p),
                         PAbility->getRange(),
                         PChar->modelHitboxSize,
                         PTarget->modelHitboxSize))
            {
                PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PTarget, 0, 0, MsgBasic::TooFarAway);
                return false;
            }

            if (abilitystatehelpers::ShouldCheckPlayerAbilityLOS(m_PEntity->loc.zone->CanUseMisc(MISC_LOS_PLAYER_BLOCK)) &&
                !m_PEntity->CanSeeTarget(PTarget))
            {
                PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PTarget, 0, 0, MsgBasic::UnableToSeeTarget);
                return false;
            }

            CBaseEntity* PMsgTarget = PChar;
            int32        errNo      = luautils::OnAbilityCheck(PChar, PTarget, PAbility, &PMsgTarget);
            if (errNo != 0)
            {
                PChar->pushPacket<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PMsgTarget, PAbility->getID(), PAbility->getID(), static_cast<MsgBasic>(errNo));
                return false;
            }
            return true;
        }
        return false;
    }

    // Non-PC path
    const bool hasAmnesia    = m_PEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Amnesia);
    const bool hasImpairment = m_PEntity->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::Impairment);
    const uint16 impairPower = hasImpairment ? m_PEntity->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::Impairment)->GetPower() : 0;
    const bool targetValid   = PTarget && m_PEntity->IsValidTarget(PTarget->targid, PAbility->getValidTarget(), errMsg);
    const float dist         = (PTarget) ? distance(m_PEntity->loc.p, PTarget->loc.p) : 0.f;
    const float tHitbox      = (PTarget) ? PTarget->modelHitboxSize : 0.f;

    if (abilitystatehelpers::NonPCCancelAbility(
            PTarget != nullptr,
            hasAmnesia,
            hasImpairment,
            impairPower,
            targetValid,
            PTarget && m_PEntity == PTarget,
            dist,
            PAbility->getRange(),
            m_PEntity->modelHitboxSize,
            tHitbox))
    {
        return false;
    }

    // TODO: should luautils::OnAbilityCheck go here too?
    return true;
}
