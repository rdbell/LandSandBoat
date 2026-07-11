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

#include "range_state.h"

#include "action/action.h"
#include "action/interrupts.h"
#include "ai/ai_container.h"
#include "range_state_capacity.h"
#include "entities/char_entity.h"
#include "entities/trust_entity.h"
#include "enums/action/category.h"
#include "items/item_weapon.h"
#include "packets/s2c/0x028_battle2.h"
#include "packets/s2c/0x029_battle_message.h"
#include "status_effect_container.h"
#include "utils/battleutils.h"
#include "utils/charutils.h"

CRangeState::CRangeState(CBattleEntity* PEntity, uint16 targid)
: CState(PEntity, targid)
, m_PEntity(PEntity)
{
    auto* PTarget = m_PEntity->IsValidTarget(m_targid, TARGET_ENEMY, m_errorMsg);

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

    if (!CanUseRangedAttack(PTarget, false))
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

    if (distance(m_PEntity->loc.p, PTarget->loc.p) > m_PEntity->GetRangedAttackRange())
    {
        m_errorMsg = std::make_unique<GP_SERV_COMMAND_BATTLE_MESSAGE>(m_PEntity, PTarget, 0, 0, MsgBasic::TooFarAway);
        throw CStateInitException(m_errorMsg->copy());
    }

    // https://www.bg-wiki.com/ffxi/Delay#Ranged_Delay
    // GetRangedDelayReduction is 2 of the 3 steps of `Ranged Weapon Delay x (1 - Snapshot) x (1 - Velocity Shot) x (1 - Rapid Shot)`
    // If Rapid Shot fires it will do the third multiplicative step
    auto delay = m_PEntity->GetRangedWeaponDelay(false);
    delay      = battleutils::GetRangedDelayReduction(m_PEntity, delay);

    // Rapid Shot (pure policy in rangestatehelpers)
    {
        CItemWeapon* weapon     = dynamic_cast<CItemWeapon*>(m_PEntity->m_Weapons[SLOT_RANGED]);
        const bool   isThrowing = weapon && weapon->isThrowing();
        if (rangestatehelpers::ShouldTryRapidShot(
                m_PEntity->objtype == TYPE_PC || m_PEntity->objtype == TYPE_TRUST, isThrowing))
        {
            auto chance{ m_PEntity->getMod(Mod::RAPID_SHOT) };

            if (auto* PChar = dynamic_cast<CCharEntity*>(m_PEntity))
            {
                chance += PChar->PMeritPoints->GetMeritValue(MERIT_RAPID_SHOT_RATE, PChar);
            }

            if (rangestatehelpers::RapidShotProcs(chance, xirand::GetRandomNumber(100)))
            {
                // reduce delay by 2-50% (draw is [2, 50) → 2..49%)
                // https://www.bg-wiki.com/ffxi/Rapid_Shot
                // https://www.ffxiah.com/forum/topic/49806/ranger-firing-range-testing/4/#3233650
                delay       = rangestatehelpers::ApplyRapidShotDelayReduction(delay, xirand::GetRandomNumber<uint16>(2, 50));
                m_rapidShot = true;
            }
        }
    }

    if (rangestatehelpers::ShouldApplyMobReturnWeaponDelay(m_PEntity->objtype == TYPE_MOB))
    {
        // Mobs have different delay returns for pulling out their weapon
        m_returnWeaponDelay = std::chrono::milliseconds(rangestatehelpers::MobReturnWeaponDelayMs);

        if (rangestatehelpers::ShouldApplyMobMeleeFreePhase(
                true, distance(m_PEntity->loc.p, PTarget->loc.p) <= m_PEntity->GetMeleeRange(PTarget)))
        {
            // Seems to have a random factor on when it can shoot next. 1 or 2 melee auto attacks
            m_freePhaseTime = std::chrono::milliseconds(
                rangestatehelpers::MobFreePhaseTimeMs(xirand::GetRandomNumber(0, 1500)));
        }
    }

    m_aimTime  = std::chrono::milliseconds(delay);
    m_startPos = m_PEntity->loc.p;

    action_t action{
        .actorId    = m_PEntity->id,
        .actiontype = ActionCategory::RangedStart,
        .actionid   = static_cast<uint32_t>(FourCC::RangedStart),
        .targets    = {
            {
                .actorId = m_PEntity->id,
                .results = {
                    {
                        // Empty result
                    },
                },
            },
        },
    };

    m_PEntity->PAI->EventHandler.triggerListener("RANGE_START", m_PEntity, &action);
    m_PEntity->loc.zone->PushPacket(m_PEntity, CHAR_INRANGE_SELF, std::make_unique<GP_SERV_COMMAND_BATTLE2>(action));
}

void CRangeState::SpendCost()
{
}

bool CRangeState::CanChangeState()
{
    return false;
}

bool CRangeState::Update(timer::time_point tick)
{
    if (m_PEntity && m_PEntity->isAlive() && (tick > GetEntryTime() + m_aimTime && !IsCompleted()))
    {
        auto* PTarget = m_PEntity->IsValidTarget(m_targid, TARGET_ENEMY, m_errorMsg);

        CanUseRangedAttack(PTarget, true);

        if (HasMoved())
        {
            m_errorMsg = std::make_unique<GP_SERV_COMMAND_BATTLE_MESSAGE>(m_PEntity, m_PEntity, 0, 0, MsgBasic::MoveAndInterrupt);
        }

        action_t action{};
        auto*    cast_errorMsg = dynamic_cast<GP_SERV_COMMAND_BATTLE_MESSAGE*>(m_errorMsg.get());
        const uint16 errMsgId  = cast_errorMsg ? static_cast<uint16>(cast_errorMsg->getMessageId()) : 0;
        if (rangestatehelpers::ShouldInterruptOnError(
                m_errorMsg != nullptr, cast_errorMsg != nullptr, errMsgId))
        {
            if (auto* PChar = dynamic_cast<CCharEntity*>(m_PEntity))
            {
                PChar->pushPacket(m_errorMsg->copy());
            }
            // reset aim time so interrupted players only have to wait the correct 2.7s until next shot
            m_aimTime = 0s;
            ActionInterrupts::RangedInterrupt(m_PEntity);
            m_PEntity->PAI->EventHandler.triggerListener("RANGE_STATE_EXIT", m_PEntity, nullptr, &action);
        }
        else
        {
            m_errorMsg.reset();

            if (!PTarget ||
                rangestatehelpers::IsOutOfRangedAttackRange(
                    distance(m_PEntity->loc.p, PTarget->loc.p), m_PEntity->GetRangedAttackRange()))
            {
                m_isOutOfRange = true;
            }

            m_PEntity->OnRangedAttack(*this, action);
            // Only send packet if action was populated (e.g. interrupts return early)
            if (!action.targets.empty())
            {
                m_PEntity->loc.zone->PushPacket(m_PEntity, CHAR_INRANGE_SELF, std::make_unique<GP_SERV_COMMAND_BATTLE2>(action));
            }
            m_PEntity->PAI->EventHandler.triggerListener("RANGE_STATE_EXIT", m_PEntity, PTarget, &action);
        }

        Complete();
    }

    if (IsCompleted() && tick > GetEntryTime() + m_aimTime + m_returnWeaponDelay)
    {
        if (auto* PChar = dynamic_cast<CCharEntity*>(m_PEntity))
        {
            PChar->m_LastRangedAttackTime = GetEntryTime() + m_aimTime + m_returnWeaponDelay;
        }
        else if (auto* PMob = dynamic_cast<CMobEntity*>(m_PEntity))
        {
            PMob->m_LastRangedAttackTime = GetEntryTime() + m_aimTime + m_freePhaseTime;
        }
        return true;
    }

    return false;
}

void CRangeState::Cleanup(timer::time_point tick)
{
}

bool CRangeState::CanUseRangedAttack(CBattleEntity* PTarget, bool isEndOfAttack)
{
    auto*      PChar            = dynamic_cast<CCharEntity*>(m_PEntity);
    const bool isPC             = PChar != nullptr;
    bool       hasRangedWeapon  = false;
    bool       hasThrowingAmmo  = false;
    bool       hasAmmoWeapon    = false;
    uint8      skillType        = 0;

    if (PChar)
    {
        CItemWeapon* PRanged = dynamic_cast<CItemWeapon*>(PChar->getEquip(SLOT_RANGED));
        CItemWeapon* PAmmo   = dynamic_cast<CItemWeapon*>(PChar->getEquip(SLOT_AMMO));

        hasRangedWeapon = PRanged && PRanged->isType(ITEM_WEAPON);
        hasThrowingAmmo = PAmmo && PAmmo->isThrowing();
        skillType       = rangestatehelpers::ResolveRangedSkillType(
            hasRangedWeapon,
            hasRangedWeapon ? PRanged->getSkillType() : 0,
            (PAmmo != nullptr) ? PAmmo->getSkillType() : 0);

        // Barrage side-effect for throwing stays host-side.
        if (skillType == SKILL_THROWING &&
            rangestatehelpers::HasInitialRangedEquip(hasRangedWeapon, hasThrowingAmmo))
        {
            PChar->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::Barrage);
        }

        CItemWeapon* PAmmoWeapon = dynamic_cast<CItemWeapon*>(PChar->getEquip(SLOT_AMMO));
        hasAmmoWeapon            = PAmmoWeapon != nullptr && PAmmoWeapon->isType(ITEM_WEAPON);
    }

    const bool isFacing = PTarget && facing(m_PEntity->loc.p, PTarget->loc.p, 64);
    const bool canSee   = PTarget && m_PEntity->CanSeeTarget(PTarget);

    bool freePhaseBusy = false;
    if (PChar)
    {
        // Same clock period as m_freePhaseTime; .count() preserves the strict < comparison.
        const auto elapsed = m_PEntity->PAI->getTick() - PChar->m_LastRangedAttackTime;
        freePhaseBusy      = rangestatehelpers::FreePhaseBusy(elapsed.count(), m_freePhaseTime.count());
    }

    const auto fail = rangestatehelpers::EvaluateCanUseRangedAttack(
        PTarget != nullptr,
        isPC,
        hasRangedWeapon,
        hasThrowingAmmo,
        skillType,
        hasAmmoWeapon,
        isFacing,
        isEndOfAttack,
        canSee,
        freePhaseBusy,
        m_PEntity->animation);

    if (fail == rangestatehelpers::RangedUseFail::None)
    {
        return true;
    }

    const auto msg = static_cast<MsgBasic>(static_cast<uint16>(fail));
    if (fail == rangestatehelpers::RangedUseFail::NoRangedWeapon && PChar)
    {
        m_errorMsg = std::make_unique<GP_SERV_COMMAND_BATTLE_MESSAGE>(PChar, PChar, 0, 0, msg);
    }
    else if (fail == rangestatehelpers::RangedUseFail::CannotAttackTarget)
    {
        m_errorMsg = std::make_unique<GP_SERV_COMMAND_BATTLE_MESSAGE>(m_PEntity, m_PEntity, 0, 0, msg);
    }
    else
    {
        m_errorMsg = std::make_unique<GP_SERV_COMMAND_BATTLE_MESSAGE>(m_PEntity, PTarget ? PTarget : m_PEntity, 0, 0, msg);
    }
    return false;
}

bool CRangeState::HasMoved()
{
    return rangestatehelpers::RangeHasMoved(
        m_PEntity->objtype == TYPE_PC,
        distance(m_startPos, m_PEntity->loc.p, true));
}
