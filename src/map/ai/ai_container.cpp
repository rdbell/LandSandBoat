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

#include "ai/ai_container.h"

#include "ai/ai_container_capacity.h"
#include "ai/controllers/mob_controller.h"
#include "ai/controllers/pet_controller.h"
#include "ai/controllers/player_controller.h"
#include "entities/base_entity.h"
#include "entities/battle_entity.h"
#include "entities/char_entity.h"
#include "entities/mob_entity.h"
#include "packets/s2c/0x038_schedulor.h"
#include "states/ability_state.h"
#include "states/attack_state.h"
#include "states/death_state.h"
#include "states/despawn_state.h"
#include "states/inactive_state.h"
#include "states/item_state.h"
#include "states/magic_state.h"
#include "states/mobskill_state.h"
#include "states/petskill_state.h"
#include "states/range_state.h"
#include "states/synth_state.h"
#include "states/trigger_state.h"
#include "states/weaponskill_state.h"
#include "status_effect_container.h"

CAIContainer::CAIContainer(CBaseEntity* _PEntity)
: CAIContainer(_PEntity, nullptr, nullptr, nullptr)
{
}

CAIContainer::CAIContainer(CBaseEntity*                   _PEntity,
                           std::unique_ptr<CPathFind>&&   _pathfind,
                           std::unique_ptr<CController>&& _controller,
                           std::unique_ptr<CTargetFind>&& _targetfind)
: TargetFind(std::move(_targetfind))
, PathFind(std::move(_pathfind))
, Controller(std::move(_controller))
, m_Tick(timer::now())
, m_PrevTick(timer::now())
, PEntity(_PEntity)
, ActionQueue(_PEntity)
{
}

bool CAIContainer::Cast(uint16 targid, SpellID spellid)
{
    // Dual-wire: aicontainerhelpers::CanDispatch (slice 2947)
    if (aicontainerhelpers::CanDispatch(static_cast<bool>(Controller)))
    {
        return Controller->Cast(targid, spellid);
    }
    return false;
}

bool CAIContainer::Engage(uint16 targid)
{
    // Dual-wire: aicontainerhelpers::CanDispatch (slice 2947)
    if (aicontainerhelpers::CanDispatch(static_cast<bool>(Controller)))
    {
        return Controller->Engage(targid);
    }
    return false;
}

bool CAIContainer::ChangeTarget(uint16 targid)
{
    // Dual-wire: aicontainerhelpers::CanDispatch (slice 2947)
    if (aicontainerhelpers::CanDispatch(static_cast<bool>(Controller)))
    {
        return Controller->ChangeTarget(targid);
    }
    return false;
}

bool CAIContainer::Disengage()
{
    // Dual-wire: aicontainerhelpers::CanDispatch (slice 2947)
    if (aicontainerhelpers::CanDispatch(static_cast<bool>(Controller)))
    {
        return Controller->Disengage();
    }
    return false;
}

bool CAIContainer::WeaponSkill(uint16 targid, uint16 wsid)
{
    // Dual-wire: aicontainerhelpers::CanDispatch (slice 2947)
    if (aicontainerhelpers::CanDispatch(static_cast<bool>(Controller)))
    {
        return Controller->WeaponSkill(targid, wsid);
    }
    return false;
}

bool CAIContainer::MobSkill(uint16 targid, uint16 wsid, Maybe<timer::duration> castTimeOverride)
{
    auto* AIController = dynamic_cast<CMobController*>(Controller.get());
    // Dual-wire: aicontainerhelpers::CanDispatch (slice 2947; typed inject)
    if (aicontainerhelpers::CanDispatch(AIController != nullptr))
    {
        return AIController->MobSkill(targid, wsid, castTimeOverride);
    }
    return false;
}

bool CAIContainer::PetSkill(uint16 targid, uint16 wsid)
{
    auto* AIController = dynamic_cast<CPetController*>(Controller.get());
    // Dual-wire: aicontainerhelpers::CanDispatch (slice 2947; typed inject)
    if (aicontainerhelpers::CanDispatch(AIController != nullptr))
    {
        return AIController->PetSkill(targid, wsid);
    }
    return false;
}

bool CAIContainer::Ability(uint16 targid, uint16 abilityid)
{
    // Dual-wire: aicontainerhelpers::CanDispatch (slice 2947)
    if (aicontainerhelpers::CanDispatch(static_cast<bool>(Controller)))
    {
        return Controller->Ability(targid, abilityid);
    }
    return false;
}

bool CAIContainer::RangedAttack(uint16 targid)
{
    // Dual-wire: aicontainerhelpers::CanDispatch (slice 2947)
    if (aicontainerhelpers::CanDispatch(static_cast<bool>(Controller)))
    {
        return Controller->RangedAttack(targid);
    }
    return false;
}

bool CAIContainer::Trigger(CCharEntity* player)
{
    // TODO: ensure idempotency of all onTrigger lua calls (i.e. chests can only be opened once)
    bool isDoor = luautils::OnTrigger(player, PEntity) == -1;
    PEntity->PAI->EventHandler.triggerListener("ON_TRIGGER", player, PEntity);
    if (CanChangeState())
    {
        auto ret = ChangeState<CTriggerState>(PEntity, player->targid, isDoor);
        // Dual-wire: aicontainerhelpers::ShouldPausePathingOnTrigger (slice 6360).
        // Go host pure plan: aicontainer.PlanTriggerPathing.
        if (aicontainerhelpers::ShouldPausePathingOnTrigger(
                static_cast<bool>(PathFind),
                PEntity->GetLocalVar("stopPathingOnTrigger")))
        {
            PEntity->SetLocalVar("pauseNPCPathing", 1);
        }
        return ret;
    }
    return false;
}

bool CAIContainer::UseItem(uint16 targid, uint8 loc, uint8 slotid)
{
    auto* PlayerController = dynamic_cast<CPlayerController*>(PEntity->PAI->GetController());
    // Dual-wire: aicontainerhelpers::CanDispatch (slice 2947; typed inject)
    if (aicontainerhelpers::CanDispatch(PlayerController != nullptr))
    {
        return PlayerController->UseItem(targid, loc, slotid);
    }
    return false;
}

bool CAIContainer::Inactive(timer::duration _duration, bool canChangeState)
{
    // Go host pure half: aicontainer.Inactive / InactiveOn (slice 6371)
    // ForceChangeState<CInactiveState>(..., untargetable=false).
    return ForceChangeState<CInactiveState>(PEntity, _duration, canChangeState, false);
}

bool CAIContainer::Untargetable(timer::duration _duration, bool canChangeState)
{
    // Go host pure half: aicontainer.Untargetable / UntargetableOn (slice 6371)
    // ForceChangeState<CInactiveState>(..., untargetable=true).
    return ForceChangeState<CInactiveState>(PEntity, _duration, canChangeState, true);
}

bool CAIContainer::Internal_Engage(uint16 targetid)
{
    // TODO: pet engage/disengage
    auto* entity = dynamic_cast<CBattleEntity*>(PEntity);

    // Already-engaged path: retarget iff battle target differs (slice 6292 dual-wire).
    const bool hasBattleEntity = entity != nullptr;
    const bool isEngaged       = hasBattleEntity && entity->PAI->IsEngaged();
    if (aicontainerhelpers::InternalEngageIsAlreadyEngagedPath(hasBattleEntity, isEngaged))
    {
        if (aicontainerhelpers::InternalEngageShouldRetarget(entity->GetBattleTargetID(), targetid))
        {
            ChangeTarget(targetid);
            return true;
        }
        return false;
    }
    // TODO: use valid target stuff from spell
    if (entity)
    {
        // TODO: remove m_battleTarget if possible (need to check disengage)
        // Check if an entity can change to the attack state.
        // Allow entity with prevent action effect to very briefly switch to the
        // attack state to be properly engaged (slice 6291 dual-wire).
        auto* const current                    = GetCurrentState();
        const bool  hasCurrentState            = current != nullptr;
        const bool  currentIsCompleted         = hasCurrentState && current->IsCompleted();
        const bool  preventActionIgnoringCharm = entity->StatusEffectContainer->HasPreventActionEffect(true);
        if (aicontainerhelpers::InternalEngageForceAttackAllowed(
                CanChangeState(), hasCurrentState, currentIsCompleted, preventActionIgnoringCharm))
        {
            if (ForceChangeState<CAttackState>(entity, targetid))
            {
                entity->OnEngage(*static_cast<CAttackState*>(GetCurrentState()));

                // Resume being inactive if entity has a status effect preventing them from doing actions
                // Go host pure half: aicontainer.InternalEngageResumeInactive (slice 6371).
                if (aicontainerhelpers::InternalEngageShouldResumeInactive(
                        entity->StatusEffectContainer->HasPreventActionEffect(true)))
                {
                    entity->PAI->Inactive(0ms, false);
                }
            }
        }
        return true;
    }
    return false;
}

bool CAIContainer::Internal_Cast(uint16 targetid, SpellID spellid)
{
    auto* entity = dynamic_cast<CBattleEntity*>(PEntity);
    if (entity)
    {
        auto*      target          = entity->GetEntity(targetid);
        const bool hasTarget       = target != nullptr;
        const bool isUntargetable  = hasTarget && target->PAI->IsUntargetable();
        // Target untargetable gate (slice 6302 dual-wire).
        if (!aicontainerhelpers::InternalActionTargetAllowed(hasTarget, isUntargetable))
        {
            return false;
        }
        return ChangeState<CMagicState>(entity, targetid, spellid);
    }
    return false;
}

bool CAIContainer::Internal_ChangeTarget(uint16 targetid)
{
    auto* entity                   = dynamic_cast<CBattleEntity*>(PEntity);
    const bool hasBattleEntity     = entity != nullptr;
    // Outer gate + path split (slice 6294 dual-wire).
    if (aicontainerhelpers::InternalChangeTargetHasBattleEntity(hasBattleEntity))
    {
        if (aicontainerhelpers::InternalChangeTargetShouldSetBattleTarget(IsEngaged(), targetid))
        {
            entity->SetBattleTargetID(targetid);
            return true;
        }
        else
        {
            return Engage(targetid);
        }
    }
    return false;
}

bool CAIContainer::Internal_Disengage()
{
    auto* entity               = dynamic_cast<CBattleEntity*>(PEntity);
    const bool hasBattleEntity = entity != nullptr;
    // Outer battle-entity gate (slice 6296 dual-wire).
    if (aicontainerhelpers::InternalDisengageHasBattleEntity(hasBattleEntity))
    {
        entity->SetBattleTargetID(0);
        return true;
    }
    return false;
}

bool CAIContainer::Internal_WeaponSkill(uint16 targid, uint16 wsid)
{
    auto* entity = dynamic_cast<CBattleEntity*>(PEntity);
    if (entity)
    {
        auto*      target          = entity->GetEntity(targid);
        const bool hasTarget       = target != nullptr;
        const bool isUntargetable  = hasTarget && target->PAI->IsUntargetable();
        // Target untargetable gate (slice 6302 dual-wire).
        if (!aicontainerhelpers::InternalActionTargetAllowed(hasTarget, isUntargetable))
        {
            return false;
        }
        return ChangeState<CWeaponSkillState>(entity, targid, wsid);
    }
    return false;
}

bool CAIContainer::Internal_MobSkill(uint16 targid, uint16 wsid, Maybe<timer::duration> castTimeOverride)
{
    auto* entity = dynamic_cast<CBattleEntity*>(PEntity);
    if (entity)
    {
        auto*      target          = entity->GetEntity(targid);
        const bool hasTarget       = target != nullptr;
        const bool isUntargetable  = hasTarget && target->PAI->IsUntargetable();
        // Target untargetable gate (slice 6302 dual-wire).
        if (!aicontainerhelpers::InternalActionTargetAllowed(hasTarget, isUntargetable))
        {
            return false;
        }
        return ChangeState<CMobSkillState>(entity, targid, wsid, castTimeOverride);
    }
    return false;
}

bool CAIContainer::Internal_PetSkill(uint16 targid, uint16 abilityid)
{
    auto* entity = dynamic_cast<CPetEntity*>(PEntity);
    if (entity)
    {
        auto*      target          = entity->GetEntity(targid);
        const bool hasTarget       = target != nullptr;
        const bool isUntargetable  = hasTarget && target->PAI->IsUntargetable();
        // Target untargetable gate (slice 6302 dual-wire).
        if (!aicontainerhelpers::InternalActionTargetAllowed(hasTarget, isUntargetable))
        {
            return false;
        }
        return ChangeState<CPetSkillState>(entity, targid, abilityid);
    }
    return false;
}

bool CAIContainer::Internal_Ability(uint16 targetid, uint16 abilityid)
{
    auto* entity = dynamic_cast<CBattleEntity*>(PEntity);
    if (entity)
    {
        auto*      target          = entity->GetEntity(targetid);
        const bool hasTarget       = target != nullptr;
        const bool isUntargetable  = hasTarget && target->PAI->IsUntargetable();
        // Target untargetable gate (slice 6302 dual-wire).
        if (!aicontainerhelpers::InternalActionTargetAllowed(hasTarget, isUntargetable))
        {
            return false;
        }
        return ChangeState<CAbilityState>(entity, targetid, abilityid);
    }
    return false;
}

bool CAIContainer::Internal_RangedAttack(uint16 targetid)
{
    auto* entity = dynamic_cast<CBattleEntity*>(PEntity);
    if (entity)
    {
        auto*      target          = entity->GetEntity(targetid);
        const bool hasTarget       = target != nullptr;
        const bool isUntargetable  = hasTarget && target->PAI->IsUntargetable();
        // Target untargetable gate (slice 6302 dual-wire).
        if (!aicontainerhelpers::InternalActionTargetAllowed(hasTarget, isUntargetable))
        {
            return false;
        }
        return ChangeState<CRangeState>(entity, targetid);
    }
    return false;
}

bool CAIContainer::Internal_Die(timer::duration deathTime)
{
    auto* entity               = dynamic_cast<CBattleEntity*>(PEntity);
    const bool hasBattleEntity = entity != nullptr;
    // Outer battle-entity gate (slice 6298 dual-wire).
    // Go host pure half: aicontainer.InternalDie / InternalDieOn (slice 6370)
    // composes admission + ChangeState.
    if (aicontainerhelpers::InternalDieHasBattleEntity(hasBattleEntity))
    {
        return ChangeState<CDeathState>(entity, deathTime);
    }
    return false;
}

bool CAIContainer::Internal_UseItem(uint16 targetid, uint8 loc, uint8 slotid)
{
    auto*      entity        = dynamic_cast<CCharEntity*>(PEntity);
    const bool hasCharEntity = entity != nullptr;
    // Outer char-entity gate (slice 6307 dual-wire).
    // Go host pure half: aicontainer.InternalUseItem / InternalUseItemOn (slice 6370)
    // composes admission + ChangeState.
    if (aicontainerhelpers::InternalUseItemHasCharEntity(hasCharEntity))
    {
        return ChangeState<CItemState>(entity, targetid, loc, slotid);
    }
    return false;
}

CState* CAIContainer::GetCurrentState()
{
    return m_currentState.get();
}

void CAIContainer::enterState(std::unique_ptr<CState> next)
{
    // Dual-wire: aicontainerhelpers::ShouldSuspendCurrentOnEnter (slice 6327).
    // Go host pure half: aicontainer.EnterState drives Stack.PushCurrent (slice 6366).
    // Suspend the state we're leaving beneath the new one, which becomes current.
    if (aicontainerhelpers::ShouldSuspendCurrentOnEnter(static_cast<bool>(m_currentState)))
    {
        m_stateStack.push(std::move(m_currentState));
    }
    m_currentState = std::move(next);
}

void CAIContainer::resumeNextState()
{
    // Dual-wire: aicontainerhelpers::ShouldResumeStackedState (slice 6327).
    // Go host pure half: aicontainer.ResumeNextState drives Stack (slice 6366).
    // The current state is finished; resume the one suspended beneath it, or go idle.
    if (!aicontainerhelpers::ShouldResumeStackedState(m_stateStack.empty()))
    {
        m_currentState.reset();
    }
    else
    {
        m_currentState = std::move(m_stateStack.top());
        m_stateStack.pop();
    }
}

bool CAIContainer::CanChangeState()
{
    // Dual-wire: aicontainerhelpers::CanChangeState (slice 2952)
    // CState::CanChangeState is non-const; keep a mutable pointer for the inject.
    auto* const current                = GetCurrentState();
    const bool  hasCurrentState        = current != nullptr;
    const bool  currentCanChange       = hasCurrentState && current->CanChangeState();
    return aicontainerhelpers::CanChangeState(hasCurrentState, currentCanChange);
}

bool CAIContainer::CanFollowPath()
{
    // Dual-wire: aicontainerhelpers::CanFollowPath (slice 6306)
    // CState::CanChangeState is non-const; keep a mutable pointer for the inject.
    auto* const current          = GetCurrentState();
    const bool  hasPathFind      = static_cast<bool>(PathFind);
    const bool  hasCurrentState  = current != nullptr;
    const bool  currentCanChange = hasCurrentState && current->CanChangeState();
    return aicontainerhelpers::CanFollowPath(hasPathFind, hasCurrentState, currentCanChange);
}

void CAIContainer::SetController(std::unique_ptr<CController> controller)
{
    Controller = std::move(controller);
}

CController* CAIContainer::GetController()
{
    return Controller.get();
}

void CAIContainer::Reset()
{
    // Dual-wire: ShouldClearPathOnReset / ShouldResetControllerOnReset (slice 6360).
    // Go host half: aicontainer.Reset drives pathfind.Path.Clear.
    if (aicontainerhelpers::ShouldClearPathOnReset(static_cast<bool>(PathFind)))
    {
        PathFind->Clear();
    }

    if (aicontainerhelpers::ShouldResetControllerOnReset(static_cast<bool>(Controller)))
    {
        Controller->Reset();
    }

    m_currentState.reset();
    while (!m_stateStack.empty())
    {
        m_stateStack.pop();
    }
}

auto CAIContainer::Tick(timer::time_point tick) -> Task<void>
{
    TracyZoneScoped;

    m_PrevTick = m_Tick;
    m_Tick     = tick;

    // TODO: timestamp in the event?
    EventHandler.triggerListener("TICK", PEntity);

    co_await PEntity->Tick(tick);

    // Go host pure half: aicontainer.Tick (slice 6369) orchestrates the phases
    // below in this order: ActionQueue → Pathing → Controller → StateDrain → Park.

    // TODO: check this in the controller instead maybe? (might not want to check every tick)
    // Dual-wire: aicontainerhelpers::ShouldCheckActionQueue (slice 6362).
    // Go host pure half: aicontainer.TickActionQueue drives actionqueue.Check.
    // ActionQueue is always present as a member (hasQueue=true).
    if (aicontainerhelpers::ShouldCheckActionQueue(true))
    {
        ActionQueue.checkAction(tick);
    }

    // check pathfinding only if there is no controller to do it
    // Dual-wire: aicontainerhelpers::ShouldTickFollowPath / ShouldNotifyPathPoint (slice 6359).
    // Go host pure half: aicontainer.TickPathing drives pathfind.FollowPath.
    bool isPathingPaused = PEntity->GetLocalVar("pauseNPCPathing") != 0;
    if (aicontainerhelpers::ShouldTickFollowPath(
            static_cast<bool>(Controller),
            CanFollowPath(),
            isPathingPaused))
    {
        PathFind->FollowPath(tick);
        if (aicontainerhelpers::ShouldNotifyPathPoint(PathFind->OnPoint()))
        {
            EventHandler.triggerListener("PATH", PEntity);
            luautils::OnPath(PEntity);
        }
    }

    // Dual-wire: aicontainerhelpers::ShouldTickController (slice 6361).
    // Go host pure half: aicontainer.TickController runs controllerTick inject.
    if (aicontainerhelpers::ShouldTickController(
            static_cast<bool>(Controller),
            Controller && Controller->canUpdate))
    {
        co_await Controller->Tick(tick);
    }

    //
    // The current state is held in m_currentState (not on the stack) while it runs, so a
    // re-entrant change can't free the object we're executing in. Entering a state only
    // suspends the current one beneath it, never frees it.
    //

    // The guard is a backstop against
    // a state that completes and re-enters itself every iteration (the stack is capped at
    // 10, so a healthy tick drains well within this bound).
    int guard = 0;

    while (m_currentState)
    {
        // Dual-wire: aicontainerhelpers::TickStateLoopContinue (slice 6313).
        // Go host pure half: aicontainer.TickStateDrain (slice 6367).
        // Pass post-increment guard; continues while guard <= 32.
        if (!aicontainerhelpers::TickStateLoopContinue(++guard))
        {
            ShowWarning("AI state loop exceeded its iteration bound; breaking to avoid a hang.");
            break;
        }

        CState* running = m_currentState.get();

        if (running->DoUpdate(tick))
        {
            // A state can enter a successor during its own update (e.g. petskill
            // re-engages), which becomes current. Only retire the state we actually ran.
            if (running == m_currentState.get())
            {
                running->Cleanup(tick);
                resumeNextState();
            }
        }
        else // Not finished: leave it current and stop.
        {
            break;
        }
    }

    // Magic and mobskill states decide their own interrupt at their finish (mid-action
    // prevent-action effects don't cancel them on retail), so we never force them inactive
    // from here. Once such a state ends, this poll parks the entity inactive.
    // Dual-wire: aicontainerhelpers::TickPreventActionParkAllowed (slice 6314).
    // Go host pure half: aicontainer.TickPreventActionPark (slice 6365);
    // composition with Inactive ForceChange: TickPreventActionParkInactive (6371).
    {
        auto*      battle                  = dynamic_cast<CBattleEntity*>(PEntity);
        const bool hasBattleEntity         = battle != nullptr;
        const bool isAlive                 = hasBattleEntity && battle->isAlive();
        const bool isInactiveState         = IsCurrentState<CInactiveState>();
        const bool isMagicState            = IsCurrentState<CMagicState>();
        const bool isMobSkillState         = IsCurrentState<CMobSkillState>();
        const bool hasPreventActionEffect  = hasBattleEntity && battle->StatusEffectContainer->HasPreventActionEffect();
        if (aicontainerhelpers::TickPreventActionParkAllowed(
                hasBattleEntity, isAlive, isInactiveState, isMagicState, isMobSkillState, hasPreventActionEffect))
        {
            Inactive(0ms, false);
        }
    }

    PEntity->PostTick();

    co_return;
}

bool CAIContainer::IsStateStackEmpty()
{
    // Dual-wire: aicontainerhelpers::IsStateStackEmpty (slice 6311)
    const bool hasCurrentState = m_currentState != nullptr;
    return aicontainerhelpers::IsStateStackEmpty(hasCurrentState);
}

void CAIContainer::ClearStateStack()
{
    // Dual-wire: aicontainerhelpers::ShouldClearCurrentState (slice 6322).
    while (aicontainerhelpers::ShouldClearCurrentState(m_currentState != nullptr))
    {
        m_currentState->Cleanup(timer::now());
        resumeNextState();
    }
}

void CAIContainer::InterruptStates()
{
    // Dual-wire: aicontainerhelpers::ShouldInterruptCurrent (slice 6321).
    // CState::CanInterrupt is non-const; keep a mutable pointer for the inject.
    while (true)
    {
        auto* const current              = m_currentState.get();
        const bool  hasCurrentState      = current != nullptr;
        const bool  currentCanInterrupt  = hasCurrentState && current->CanInterrupt();
        if (!aicontainerhelpers::ShouldInterruptCurrent(hasCurrentState, currentCanInterrupt))
        {
            break;
        }
        m_currentState->Cleanup(timer::now());
        resumeNextState();
    }
}

bool CAIContainer::IsSpawned()
{
    // Dual-wire: aicontainerhelpers::IsSpawnedStatus (slice 6309)
    const bool isDisappear = PEntity->status == STATUS_TYPE::DISAPPEAR;
    return aicontainerhelpers::IsSpawnedStatus(isDisappear);
}

bool CAIContainer::IsRoaming()
{
    // Dual-wire: aicontainerhelpers::IsRoamingAnimation (slice 6308)
    const bool animationIsNone = PEntity->animation == ANIMATION_NONE;
    return aicontainerhelpers::IsRoamingAnimation(animationIsNone);
}

bool CAIContainer::IsEngaged()
{
    // Dual-wire: aicontainerhelpers::IsEngagedAnimation (slice 6308)
    const bool animationIsAttack = PEntity->animation == ANIMATION_ATTACK;
    return aicontainerhelpers::IsEngagedAnimation(animationIsAttack);
}

bool CAIContainer::IsUntargetable()
{
    // Dual-wire: aicontainerhelpers::IsUntargetable (slice 6310)
    const bool isInactiveState       = IsCurrentState<CInactiveState>();
    const bool inactiveUntargetable  = isInactiveState && static_cast<CInactiveState*>(GetCurrentState())->GetUntargetable();
    const bool entityUntargetable    = PEntity->GetUntargetable();
    return aicontainerhelpers::IsUntargetable(isInactiveState, inactiveUntargetable, entityUntargetable);
}

timer::time_point CAIContainer::getTick()
{
    return m_Tick;
}

timer::time_point CAIContainer::getPrevTick()
{
    return m_PrevTick;
}

void CAIContainer::Despawn()
{
    const bool hasController = static_cast<bool>(Controller);
    // Controller-vs-Internal_Despawn branch (slice 6305 dual-wire).
    if (aicontainerhelpers::DespawnShouldDispatchController(hasController))
    {
        Controller->Despawn();
    }
    else
    {
        Internal_Despawn();
    }
}

void CAIContainer::QueueAction(queueAction_t&& action)
{
    ActionQueue.pushAction(std::move(action));
}

bool CAIContainer::QueueEmpty()
{
    // Dual-wire: aicontainerhelpers::IsActionQueueEmpty (slice 6323).
    // Go host pure half: aicontainer.QueueEmpty drives actionqueue.Executor.Empty.
    return aicontainerhelpers::IsActionQueueEmpty(ActionQueue.isEmpty());
}

void CAIContainer::ClearActionQueue()
{
    // Go host pure half: aicontainer.ClearActionQueue (slice 6365).
    ActionQueue.clearActionQueue();
}

void CAIContainer::ClearTimerQueue()
{
    // Go host pure half: aicontainer.ClearTimerQueue (slice 6365).
    ActionQueue.clearTimerQueue();
}

void CAIContainer::checkQueueImmediately()
{
    // Go host pure half: aicontainer.CheckQueueImmediately → TickActionQueue (6362/6365).
    ActionQueue.checkAction(timer::now());
}

bool CAIContainer::Internal_Despawn(bool instantDespawn)
{
    const bool isCurrentDespawnState = IsCurrentState<CDespawnState>();
    // Outer not-already-despawning admission (slice 6300 dual-wire).
    if (aicontainerhelpers::InternalDespawnAllowed(isCurrentDespawnState))
    {
        return ForceChangeState<CDespawnState>(PEntity, instantDespawn);
    }
    return false;
}

bool CAIContainer::Internal_Synth(SKILLTYPE synthSkill)
{
    auto*      PChar               = dynamic_cast<CCharEntity*>(PEntity);
    const bool hasCharEntity       = PChar != nullptr;
    const bool isCurrentSynthState = IsCurrentState<CSynthState>();
    // Char-entity + not-already-synth admission (slice 6304 dual-wire).
    // Go host pure half: aicontainer.InternalSynth / InternalSynthOn (slice 6370)
    // composes admission + ForceChangeState.
    if (aicontainerhelpers::InternalSynthAllowed(hasCharEntity, isCurrentSynthState))
    {
        return ForceChangeState<CSynthState>(PChar, synthSkill);
    }
    return false;
}

void CAIContainer::CheckCompletedStates()
{
    // Dual-wire: aicontainerhelpers::ShouldCleanupCompletedState (slice 6322).
    // CState::IsCompleted is const-safe; keep a pointer for the inject.
    while (true)
    {
        auto* const current         = m_currentState.get();
        const bool  hasCurrentState = current != nullptr;
        const bool  isCompleted     = hasCurrentState && current->IsCompleted();
        if (!aicontainerhelpers::ShouldCleanupCompletedState(hasCurrentState, isCompleted))
        {
            break;
        }
        m_currentState->Cleanup(timer::now());
        resumeNextState();
    }
}

bool CAIContainer::Accept_Raise()
{
    const bool isCurrentDeathState = IsCurrentState<CDeathState>();
    // Death-state admission (slice 6303 dual-wire).
    if (aicontainerhelpers::AcceptRaiseShouldInvoke(isCurrentDeathState))
    {
        static_cast<CDeathState*>(PEntity->PAI->GetCurrentState())->acceptRaise();
    }
    return false;
}

size_t CAIContainer::stateCount() const
{
    return m_stateStack.size() + (m_currentState ? 1 : 0);
}
