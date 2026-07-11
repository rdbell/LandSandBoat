/*
===========================================================================

  Copyright (c) 2022 LandSandBoat Dev Team

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

#include "petskill_state.h"
#include "action/action.h"
#include "action/interrupts.h"
#include "ai/ai_container.h"
#include "enmity_container.h"
#include "entities/pet_entity.h"
#include "packets/s2c/0x028_battle2.h"
#include "petskill.h"
#include "petskill_state_capacity.h"
#include "status_effect_container.h"
#include "utils/battleutils.h"
#include "utils/petutils.h"

CPetSkillState::CPetSkillState(CPetEntity* PEntity, uint16 targid, uint16 wsid)
: CState(PEntity, targid)
, m_PEntity(PEntity)
, m_spentTP(0)
{
    auto* skill = battleutils::GetPetSkill(wsid);
    if (!skill)
    {
        throw CStateInitException(nullptr);
    }

    if (petskillstatehelpers::ShouldRejectAmnesiaOrImpairment(
            m_PEntity->StatusEffectContainer->HasStatusEffect({ xi::StatusEffect::Amnesia, xi::StatusEffect::Impairment })))
    {
        throw CStateInitException(nullptr);
    }

    auto* PTarget = m_PEntity->IsValidTarget(m_targid, skill->getValidTargets(), m_errorMsg);

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

    m_PSkill = std::make_unique<CPetSkill>(*skill);

    m_castTime = m_PSkill->getActivationTime();

    if (petskillstatehelpers::ShouldSendSkillStartPacket(m_castTime > 0s))
    {
        const auto startParam = petskillstatehelpers::SkillStartParam(m_PSkill->getMobSkillID(), m_PSkill->getID());
        const auto startMsg   = petskillstatehelpers::SkillStartUsesWeaponskillMessage(m_PSkill->getMobSkillID())
                                    ? MsgBasic::ReadiesWeaponskill
                                    : MsgBasic::ReadiesSkill;

        action_t action{
            .actorId    = m_PEntity->id,
            .actiontype = ActionCategory::SkillStart,
            .actionid   = static_cast<uint32_t>(FourCC::SkillUse),
            .targets    = {
                {
                    .actorId = PTarget->id,
                    .results = {
                        {
                            .param     = startParam,
                            .messageID = startMsg,
                        },
                    },
                },
            },
        };

        m_PEntity->loc.zone->PushPacket(m_PEntity, CHAR_INRANGE, std::make_unique<GP_SERV_COMMAND_BATTLE2>(action));

        // Wyverns immediately emit a skill interrupt packet.
        // This looks like a hack but is retail accurate.
        if (petskillstatehelpers::ShouldEmitWyvernSkillReady(
                static_cast<uint16>(PEntity->petID()), PEntity->getMod(Mod::WYVERN_SHOW_READYING)))
        {
            ActionInterrupts::WyvernSkillReady(PEntity);
        }
    }
    m_PEntity->PAI->EventHandler.triggerListener("WEAPONSKILL_STATE_ENTER", m_PEntity, m_PSkill->getID());
    SpendCost();
}

CPetSkill* CPetSkillState::GetPetSkill()
{
    return m_PSkill.get();
}

void CPetSkillState::SpendCost()
{
    using namespace petskillstatehelpers;

    const auto [spent, remaining] = EvaluatePetSkillSpendCost(
        m_PSkill->isTpFreeSkill(), static_cast<int16>(m_PEntity->health.tp));
    m_spentTP = spent;
    if (ShouldSpendPetSkillTP(m_PSkill->isTpFreeSkill()))
    {
        m_PEntity->health.tp = remaining;
    }
}

bool CPetSkillState::Update(timer::time_point tick)
{
    using namespace petskillstatehelpers;

    // Reset the state for the current skill attempt
    m_skillSuccess = false;

    if (m_PEntity && m_PEntity->isAlive() &&
        ShouldFinishPetSkill(tick > GetEntryTime() + m_castTime, IsCompleted()))
    {
        action_t action{};
        m_PEntity->OnPetSkillFinished(*this, action);
        // Only send packet if action was populated (e.g. interrupts return early)
        if (SkillSuccessFromAction(action.targets.empty()))
        {
            m_skillSuccess = true;
            m_PEntity->loc.zone->PushPacket(m_PEntity, CHAR_INRANGE_SELF, std::make_unique<GP_SERV_COMMAND_BATTLE2>(action));
        }
        m_finishTime = tick + m_PSkill->getAnimationTime();
        Complete();
    }

    if (!m_PEntity)
    {
        ShowError("CPetSkillState: m_Pentity is nullptr");
        return false;
    }

    if (ShouldExitPetSkill(IsCompleted(), tick > m_finishTime))
    {
        auto* PTarget = GetTarget();
        if (ShouldUpdateExitEnmity(
                m_skillSuccess,
                PTarget != nullptr,
                PTarget && PTarget->objtype == TYPE_MOB,
                PTarget == m_PEntity,
                PTarget && m_PEntity->allegiance != PTarget->allegiance))
        {
            // This generates enmity for the master when using a pet skill, excluding Automatons.
            // All player pets will generate base enmity for the master, which is retail accurate.
            const bool withMaster = EnmityWithMaster(m_PEntity->objtype == TYPE_PET);
            static_cast<CMobEntity*>(PTarget)->PEnmityContainer->UpdateEnmity(m_PEntity, 0, 0, withMaster);
        }

        CCharEntity* PSummoner = dynamic_cast<CCharEntity*>(m_PEntity->PMaster);
        const bool   hasFavor  = PSummoner && PSummoner->StatusEffectContainer->HasStatusEffect(xi::StatusEffect::AvatarsFavor);

        if (ShouldApplyAvatarsFavor(
                m_PEntity->objtype == TYPE_PET,
                m_PEntity->PMaster && m_PEntity->PMaster->objtype == TYPE_PC,
                m_PSkill->isBloodPactRage(),
                m_PSkill->isBloodPactWard(),
                hasFavor))
        {
            auto power = PSummoner->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::AvatarsFavor)->GetPower();
            // Retail: Power is gained for BP use
            const auto levelGained = AvatarsFavorLevelGained(m_PSkill->isBloodPactRage());
            power                  = ApplyAvatarsFavorPower(static_cast<int16>(power), levelGained);
            PSummoner->StatusEffectContainer->GetStatusEffect(xi::StatusEffect::AvatarsFavor)->SetPower(power);
        }

        if (ShouldConsiderAvatarReengage(
                m_PEntity->objtype == TYPE_PET,
                m_PEntity->PMaster && m_PEntity->PMaster->objtype == TYPE_PC,
                m_PSkill->isBloodPactRage(),
                m_PSkill->isBloodPactWard()))
        {
            auto* PBattleTarget = dynamic_cast<CBattleEntity*>(PTarget);
            if (ShouldReengageAfterBloodPact(
                    PTarget != nullptr,
                    static_cast<uint8>(m_PEntity->getPetType()),
                    static_cast<uint16>(m_PEntity->petID()),
                    PBattleTarget && PBattleTarget->isAlive(),
                    PBattleTarget && PBattleTarget->objtype == TYPE_MOB,
                    PBattleTarget && PBattleTarget->allegiance != m_PEntity->allegiance))
            {
                // Re-engage the target after blood pact
                m_PEntity->PAI->Engage(PTarget->targid);
            }
        }
        return true;
    }
    return false;
}

void CPetSkillState::Cleanup(timer::time_point tick)
{
    if (!m_PEntity)
    {
        return;
    }

    // Interrupted.
    if (petskillstatehelpers::ShouldInterruptOnCleanup(IsCompleted()))
    {
        ActionInterrupts::AbilityInterrupt(m_PEntity);
    }

    // Not interrupted.
    else
    {
        if (petskillstatehelpers::ShouldApplyFinalAnimationSub(
                m_PEntity->isAlive(), m_PSkill->getFinalAnimationSub().has_value()))
        {
            m_PEntity->animationsub = m_PSkill->getFinalAnimationSub().value();
            m_PEntity->updatemask |= UPDATE_COMBAT;
        }

        // luautils::OnMobSkillFinalize(m_PEntity, m_PSkill.get());
    }

    // Call listener. Feed skill result.
    if (m_PEntity->isAlive())
    {
        m_PEntity->PAI->EventHandler.triggerListener("WEAPONSKILL_STATE_EXIT", m_PEntity, m_PSkill->getID(), IsCompleted());
    }
}
