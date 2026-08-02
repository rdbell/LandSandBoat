/*
===========================================================================

  Copyright (c) 2018 Darkstar Dev Teams

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

#include "trust_controller.h"
#include "trust_controller_ability_capacity.h"
#include "trust_controller_cast_coordination_capacity.h"
#include "trust_controller_combat_declump_admission_capacity.h"
#include "trust_controller_combat_declump_displacement_capacity.h"
#include "trust_controller_combat_attack_lookup_capacity.h"
#include "trust_controller_combat_movement_admission_capacity.h"
#include "trust_controller_combat_movement_capacity.h"
#include "trust_controller_combat_master_engagement_capacity.h"
#include "trust_controller_combat_post_movement_capacity.h"
#include "trust_controller_combat_warp_admission_capacity.h"
#include "trust_controller_despawn_capacity.h"
#include "trust_controller_master_enmity_capacity.h"
#include "trust_controller_melee_path_result_capacity.h"
#include "trust_controller_target_sync_admission_capacity.h"
#include "trust_controller_engage_capacity.h"
#include "trust_controller_noncombat_follow_capacity.h"
#include "trust_controller_noncombat_declump_admission_capacity.h"
#include "trust_controller_noncombat_gambit_admission_capacity.h"
#include "trust_controller_party_position_capacity.h"
#include "trust_controller_reposition_candidate_capacity.h"
#include "trust_controller_ranged_attack_dispatch_capacity.h"
#include "trust_controller_cast_target_source_capacity.h"
#include "trust_controller_cast_recast_admission_capacity.h"
#include "trust_controller_top_enmity_target_capacity.h"
#include "trust_controller_noncombat_movement_capacity.h"
#include "trust_controller_noncombat_movement_dispatch_capacity.h"
#include "trust_controller_recovery_capacity.h"
#include "trust_controller_ranged_attack_capacity.h"
#include "trust_controller_reposition_capacity.h"
#include "trust_controller_roam_formation_capacity.h"
#include "trust_controller_roam_movement_dispatch_capacity.h"
#include "trust_controller_tick_capacity.h"
#include "trust_controller_target_sync_capacity.h"

#include "ability.h"
#include "ai/helpers/gambits_container.h"
#include "ai/states/despawn_state.h"
#include "ai/states/magic_state.h"
#include "ai/states/range_state.h"
#include "enmity_container.h"
#include "entities/char_entity.h"
#include "entities/trust_entity.h"
#include "items/item_weapon.h"
#include "mob_modifier.h"
#include "mob_spell_container.h"
#include "player_controller.h"
#include "recast_container.h"
#include "status_effect_container.h"
#include "utils/charutils.h"

namespace
{

enum TRUST_MOVEMENT_TYPE : int8
{
    // NOTE: If you need to add special movement types, add descending into the minus values.
    //     : All of the positive values are taken for the ranged movement range.
    // NOTE: You can use any positive value as a distance, and it will act as MID_RANGE or LONG_RANGE, but with the value you've provided.
    //     : For example:
    //     :     mob:setMobMod(xi.mobMod.TRUST_DISTANCE, 20)
    //     : Will set the combat distance the trust tries to stick to to 20'
    // NOTE: If a Trust doesn't immediately sprint to a certain distance at the start of battle, it's probably NO_MOVE or MELEE.
    NO_MOVE    = -1, // Will stand still providing they're within casting distance of their master and target when the fight starts. Otherwise will reposition to be within 9.0' of both
    NON_COMBAT = -2, // Will follow the master if first trust in party and will follow the trust in front if lower in the list.
    MELEE      = 0,  // Default: will continually reposition to stay within melee range of the target
    MID_RANGE  = 6,  // Will path at the start of battle to 6' away from the target, and try to stay at that distance
    LONG_RANGE = 12, // Will path at the start of battle to 12' away from the target, and try to stay at that distance
};

} // namespace

CTrustController::CTrustController(CCharEntity* PChar, CTrustEntity* PTrust)
: CMobController(PTrust)
, m_GambitsContainer(std::make_unique<gambits::CGambitsContainer>(PTrust))
, m_LastTopEnmity(nullptr)
, m_failedRepositionAttempts(0)
, m_InTransit(false)
{
}

CTrustController::~CTrustController()
{
    if (POwner->PAI)
    {
        if (POwner->PAI->IsEngaged())
        {
            POwner->PAI->Internal_Disengage();
        }
        POwner->PAI->PathFind.reset();
    }

    POwner->allegiance = ALLEGIANCE_TYPE::PLAYER;
    POwner->status     = STATUS_TYPE::DISAPPEAR;
    m_LastTopEnmity    = nullptr;
}

void CTrustController::Despawn()
{
    trustcontrollerdespawn::Apply(
        [&]() { POwner->PMaster = nullptr; },
        [&]() { POwner->animation = ANIMATION_DESPAWN; },
        [&]() { CMobController::Despawn(); });
}

auto CTrustController::Tick(timer::time_point tick) -> Task<void>
{
    TracyZoneScoped;
    TracyZoneString(POwner->getName());

    m_Tick = tick;

    auto* PTrust = static_cast<CTrustEntity*>(POwner);

    const bool hasMaster = PTrust->PMaster != nullptr;
    if (!hasMaster)
    {
        co_return;
    }
    const bool masterCharmed = hasMaster && PTrust->PMaster->isCharmed;
    const bool masterEngaged = hasMaster && PTrust->PMaster->PAI->IsEngaged();
    const bool nonCombatFollowTrust = PTrust->getMobMod(MOBMOD_TRUST_DISTANCE) == TRUST_MOVEMENT_TYPE::NON_COMBAT;
    switch (trustcontrollertick::Resolve(hasMaster, masterCharmed, masterEngaged, nonCombatFollowTrust, POwner->PAI->IsEngaged(), POwner->isDead()))
    {
        case trustcontrollertick::Route::Despawn:
            this->Despawn();
            break;
        case trustcontrollertick::Route::NonCombat:
            co_await DoNonCombatTick(tick);
            break;
        case trustcontrollertick::Route::Combat:
            co_await DoCombatTick(tick);
            break;
        case trustcontrollertick::Route::Roam:
            co_await DoRoamTick(tick);
            break;
        case trustcontrollertick::Route::None:
            break;
    }

    co_return;
}

auto CTrustController::DoCombatTick(timer::time_point tick) -> Task<void>
{
    TracyZoneScoped;

    CTrustEntity* PTrust  = static_cast<CTrustEntity*>(POwner);
    CCharEntity*  PMaster = static_cast<CCharEntity*>(POwner->PMaster);
    CMobEntity*   PMob    = dynamic_cast<CMobEntity*>(PMaster->GetBattleTarget());
    PTarget               = POwner->GetBattleTarget();

    const auto masterEngagementPlan = trustcontrollercombatmasterengagement::Resolve(PMaster->PAI->IsEngaged());
    if (masterEngagementPlan.disengage)
    {
        PTrust->PAI->Internal_Disengage();
        m_LastTopEnmity = nullptr;
        m_CombatEndTime = m_Tick;
    }

    const bool targetMismatch = trustcontrollertargetsyncadmission::CanCheck(
        PMaster != nullptr, PMob != nullptr,
        [&]() { return PTrust->GetBattleTargetID() != PMaster->GetBattleTargetID(); });
    if (targetMismatch)
    {
        auto  masterID   = PMaster->id;
        auto* enmityList = PMob->PEnmityContainer->GetEnmityList();
        const auto masterEnmity = trustcontrollermasterenmity::Resolve(*enmityList, masterID);

        const auto targetSyncPlan = trustcontrollertargetsync::Resolve(
            targetMismatch, masterEnmity.active, masterEnmity.cumulative, masterEnmity.volatileEnmity);
        if (targetSyncPlan.changeTarget)
        {
            PTrust->PAI->Internal_ChangeTarget(PMaster->GetBattleTargetID());
            if (targetSyncPlan.clearTopEnmity)
            {
                m_LastTopEnmity = nullptr;
            }
        }
    }

    // If busy, don't run around!
    if (!trustcontrollercombatmovementadmission::CanMove(
            [&]() { return PTrust->PAI->IsCurrentState<CMagicState>(); },
            [&]() { return PTrust->PAI->IsCurrentState<CRangeState>(); }))
    {
        co_return;
    }

    if (PTarget)
    {
        const bool canFollowPath = PTrust->PAI->CanFollowPath();
        const bool hasSpeed      = PTrust->GetSpeed() > 0;
        if (trustcontrollercombatmovement::CanRun(canFollowPath, hasSpeed))
        {
            float currentDistanceToTarget = distance(PTrust->loc.p, PTarget->loc.p);
            float currentDistanceToMaster = distance(PTrust->loc.p, PMaster->loc.p);

            if (trustcontrollercombatwarpadmission::ShouldWarp(
                    PMaster->PAI->IsEngaged(), currentDistanceToTarget))
            {
                PTrust->PAI->PathFind->WarpTo(PTarget->loc.p);
            }

            PTrust->PAI->PathFind->LookAt(PTarget->loc.p);

            int16 movementDistance = PTrust->getMobMod(MOBMOD_TRUST_DISTANCE);

            const bool canAttack = trustcontrollercombatattacklookup::Resolve(
                movementDistance,
                [&]()
                {
                    std::unique_ptr<CBasicPacket> err;
                    return PTrust->CanAttack(PTarget, err);
                });

            const auto movementPlan = trustcontrollercombatmovement::Resolve(
                canFollowPath, hasSpeed, movementDistance, currentDistanceToTarget,
                currentDistanceToMaster, canAttack);
            switch (movementPlan.action)
            {
                case trustcontrollercombatmovement::Action::PathOut:
                    PathOutToDistance(PTarget, movementPlan.desiredDistance);
                    break;
                case trustcontrollercombatmovement::Action::MeleePath:
                    switch (trustcontrollermeleepathresult::Resolve(
                        [&]() { return PTrust->PAI->PathFind->PathAround(PTarget->loc.p, movementPlan.desiredDistance, PATHFLAG_RUN | PATHFLAG_WALLHACK); },
                        [&]() { return PTrust->GetSpeed() > 0; }))
                    {
                        case trustcontrollermeleepathresult::Action::Follow:
                            PTrust->PAI->PathFind->FollowPath(m_Tick);
                            break;
                        case trustcontrollermeleepathresult::Action::Step:
                            PTrust->PAI->PathFind->StepTo(PTarget->loc.p, true);
                            break;
                        case trustcontrollermeleepathresult::Action::Hold:
                            break;
                    }
                    break;
                case trustcontrollercombatmovement::Action::MeleeStep:
                    PTrust->PAI->PathFind->StepTo(PTarget->loc.p, true);
                    break;
                case trustcontrollercombatmovement::Action::Hold:
                    break;
            }

            if (trustcontrollercombatpostmovement::ShouldDeclump(PTrust->PAI->PathFind->IsFollowingPath()))
            {
                Declump(PMaster, PTarget);
            }
        }

        if (trustcontrollercombatpostmovement::ShouldFollowPath(m_InTransit))
        {
            PTrust->PAI->PathFind->FollowPath(m_Tick);
        }

        co_await m_GambitsContainer->Tick(tick);

        PTrust->PAI->EventHandler.triggerListener("COMBAT_TICK", PTrust, PMaster, PTarget);
    }

    co_return;
}

auto CTrustController::DoNonCombatTick(timer::time_point tick) -> Task<void>
{
    TracyZoneScoped;

    auto* PTrust  = static_cast<CTrustEntity*>(POwner);
    auto* PMaster = static_cast<CCharEntity*>(POwner->PMaster);

    if (!PMaster)
    {
        co_return;
    }

    // Keep COMBAT_TICK target valid for listeners/gambits.
    PTarget = PMaster->GetBattleTarget();

    // Non-combat trust follow order:
    // - first trust follows master
    // - others follow the trust directly in front of them
    uint8 currentPartyPos = GetPartyPosition();

    CBattleEntity* PFollowTarget = PMaster;
    bool           previousExists = false;
    bool           previousIsSelf = false;
    if (currentPartyPos > 0 && static_cast<size_t>(currentPartyPos - 1) < PMaster->PTrusts.size())
    {
        if (auto* PLeadTrust = PMaster->PTrusts.at(currentPartyPos - 1); PLeadTrust)
        {
            previousExists = true;
            previousIsSelf = PLeadTrust == PTrust;
            if (!previousIsSelf)
            {
                PFollowTarget = PLeadTrust;
            }
        }
    }

    const auto plan = trustcontrollernoncombatfollow::Resolve(currentPartyPos, previousExists, previousIsSelf);
    const float desiredFollowDistance = plan.distance;

    float currentDistance = distance(PTrust->loc.p, PFollowTarget->loc.p);

    // Simple declump so non-combat trusts don't stack on each other.
    for (auto* POtherTrust : PMaster->PTrusts)
    {
        if (trustcontrollernoncombatdeclumpadmission::ShouldDeclump(
                POtherTrust == PTrust,
                [&]() { return distance(POtherTrust->loc.p, PTrust->loc.p) < 1.0f; },
                [&]() { return PTrust->PAI->PathFind->IsFollowingPath(); }))
        {
            auto diff_angle = worldAngle(PTrust->loc.p, POtherTrust->loc.p) + 64;
            auto amount     = (currentPartyPos % 2) ? 1.0f : -1.0f;

            position_t new_pos = {
                PTrust->loc.p.x - (cosf(rotationToRadian(diff_angle)) * amount),
                POtherTrust->loc.p.y,
                PTrust->loc.p.z + (sinf(rotationToRadian(diff_angle)) * amount),
                0,
                0,
            };

            if (PTrust->PAI->PathFind->ValidPosition(new_pos) &&
                PTrust->PAI->PathFind->PathAround(new_pos, desiredFollowDistance, PATHFLAG_RUN | PATHFLAG_WALLHACK))
            {
                PTrust->PAI->PathFind->FollowPath(m_Tick);
            }
            break;
        }
    }

    const auto movementAction = trustcontrollernoncombatmovement::Resolve(currentDistance, desiredFollowDistance);
    const bool pathFound = movementAction == trustcontrollernoncombatmovement::Action::Path &&
                           PTrust->PAI->PathFind->PathAround(PFollowTarget->loc.p, desiredFollowDistance, PATHFLAG_RUN | PATHFLAG_WALLHACK);
    switch (trustcontrollernoncombatmovementdispatch::Resolve(
        movementAction, pathFound, [&]() { return PTrust->GetSpeed() > 0; }))
    {
        case trustcontrollernoncombatmovementdispatch::Action::Warp:
            PTrust->PAI->PathFind->WarpTo(PFollowTarget->loc.p);
            break;
        case trustcontrollernoncombatmovementdispatch::Action::Follow:
            PTrust->PAI->PathFind->FollowPath(m_Tick);
            break;
        case trustcontrollernoncombatmovementdispatch::Action::Step:
            PTrust->PAI->PathFind->StepTo(PFollowTarget->loc.p, true);
            break;
        case trustcontrollernoncombatmovementdispatch::Action::Hold:
            break;
    }

    if (PTrust->PAI->PathFind->IsFollowingPath())
    {
        PTrust->PAI->PathFind->FollowPath(m_Tick);
    }

    // Keep gambits active in combat, but only while stationary.
    if (trustcontrollernoncombatgambitadmission::CanRun(
            PMaster->PAI->IsEngaged(),
            [&]() { return PTrust->PAI->PathFind->IsFollowingPath(); }))
    {
        co_await m_GambitsContainer->Tick(tick);
        PTrust->PAI->EventHandler.triggerListener("COMBAT_TICK", PTrust, PMaster, PTarget);
    }

    co_return;
}

auto CTrustController::DoRoamTick(timer::time_point tick) -> Task<void>
{
    TracyZoneScoped;

    auto* PMaster              = static_cast<CCharEntity*>(POwner->PMaster);
    auto  masterLastAttackTime = static_cast<CPlayerController*>(PMaster->PAI->GetController())->getLastAttackTime();
    bool  masterMeleeSwing     = masterLastAttackTime > timer::now() - 1s;

    const bool masterHasTarget = PMaster->GetBattleTarget();
    // TrustEngageType 1 engages with a target alone; all other values use the retail swing requirement.
    const auto engageType = charutils::GetCharVar(PMaster, "TrustEngageType");
    if (trustcontrollerengage::ShouldEngage(
            PMaster->PAI->IsEngaged(), masterHasTarget, masterMeleeSwing, engageType, POwner->GetModelId()))
    {
        POwner->PAI->Internal_Engage(PMaster->GetBattleTargetID());
    }

    uint8          currentPartyPos = GetPartyPosition();
    CBattleEntity* PFollowTarget   = (GetPartyPosition() > 0) ? (CBattleEntity*)PMaster->PTrusts.at(currentPartyPos - 1) : POwner->PMaster;
    float          currentDistance = distance(POwner->loc.p, PFollowTarget->loc.p);

    const auto formationPlan = trustcontrollerroamformation::Resolve(
        currentPartyPos, currentDistance, POwner->PAI->PathFind->IsFollowingPath());
    bool pathFound = false;
    switch (formationPlan.action)
    {
        case trustcontrollerroamformation::Action::Declump:
            pathFound = PFollowTarget && POwner->PAI->PathFind->PathAround(PFollowTarget->loc.p, formationPlan.targetDistance + 0.5f, PATHFLAG_RUN | PATHFLAG_WALLHACK);
            break;
        case trustcontrollerroamformation::Action::Path:
            pathFound = POwner->PAI->PathFind->PathAround(PFollowTarget->loc.p, formationPlan.targetDistance, PATHFLAG_RUN | PATHFLAG_WALLHACK);
            break;
        case trustcontrollerroamformation::Action::Step:
        case trustcontrollerroamformation::Action::Warp:
        case trustcontrollerroamformation::Action::Clear:
        case trustcontrollerroamformation::Action::None:
            break;
    }

    switch (trustcontrollerroammovementdispatch::Resolve(
        formationPlan.action, pathFound, [&]() { return POwner->GetSpeed() > 0; }))
    {
        case trustcontrollerroammovementdispatch::Action::Warp:
            POwner->PAI->PathFind->WarpTo(PFollowTarget->loc.p);
            break;
        case trustcontrollerroammovementdispatch::Action::Follow:
            POwner->PAI->PathFind->FollowPath(m_Tick);
            break;
        case trustcontrollerroammovementdispatch::Action::Step:
            POwner->PAI->PathFind->StepTo(PFollowTarget->loc.p, true);
            break;
        case trustcontrollerroammovementdispatch::Action::Clear:
            POwner->PAI->PathFind->Clear();
            break;
        case trustcontrollerroammovementdispatch::Action::Hold:
            break;
    }

    const auto recoveryPlan = trustcontrollerrecovery::Resolve(
        POwner->CanRest(), POwner->health.hp != POwner->health.maxhp || POwner->health.mp != POwner->health.maxmp,
        m_Tick - POwner->LastAttacked, m_Tick - m_CombatEndTime, m_Tick - m_LastHealTickTime, m_NumHealingTicks);
    if (recoveryPlan.recover)
    {
        POwner->addHP(trustcontrollerrecovery::Amount(POwner->health.maxhp));
        POwner->addMP(trustcontrollerrecovery::Amount(POwner->health.maxmp));
        m_LastHealTickTime = m_Tick;
        POwner->updatemask |= UPDATE_HP;
        m_NumHealingTicks = recoveryPlan.nextHealingTick;
    }

    co_return;
}

void CTrustController::Declump(CCharEntity* PMaster, CBattleEntity* PTarget)
{
    TracyZoneScoped;

    uint8 currentPartyPos = GetPartyPosition();
    for (auto* POtherTrust : PMaster->PTrusts)
    {
        if (trustcontrollercombatdeclumpadmission::ShouldDeclump(
                POtherTrust == POwner,
                [&]() { return POtherTrust->PAI->PathFind->IsFollowingPath(); },
                [&]() { return distance(POtherTrust->loc.p, POwner->loc.p) < 1.5f; }))
        {
            auto diffAngle  = worldAngle(POwner->loc.p, PTarget->loc.p) + 64;
            auto moveAmount = trustcontrollercombatdeclumpdisplacement::Amount(
                xirand::GetRandomNumber(0.0f, 1.5f), currentPartyPos);

            // clang-format off
            position_t newPos =
            {
                POwner->loc.p.x - (cosf(rotationToRadian(diffAngle)) * moveAmount),
                PTarget->loc.p.y,
                POwner->loc.p.z + (sinf(rotationToRadian(diffAngle)) * moveAmount),
                0,
                0,
            };
            // clang-format on

            if (POwner->PAI->PathFind->ValidPosition(newPos))
            {
                POwner->PAI->PathFind->PathTo(newPos, PATHFLAG_RUN | PATHFLAG_WALLHACK);
            }
            break;
        }
    }
}

void CTrustController::PathOutToDistance(CBattleEntity* PTarget, float amount)
{
    TracyZoneScoped;

    float      currentDistanceToTarget = distance(POwner->loc.p, PTarget->loc.p);
    position_t target_position         = POwner->loc.p;

    m_failedRepositionAttempts = trustcontrollerreposition::NextFailedAttempts(
        GetTopEnmity() == POwner, m_failedRepositionAttempts);

    // Invalidate position and pick new one (limit: every 3s)
    if (trustcontrollerreposition::ShouldSelect(
            currentDistanceToTarget, amount, POwner->PAI->PathFind->ValidPosition(POwner->loc.p),
            m_Tick - m_LastRepositionTime, m_InTransit))
    {
        std::vector<position_t> positions(5);
        for (auto& position : positions)
        {
            int        random_angle       = xirand::GetRandomNumber(256);
            position_t potential_position = {
                PTarget->loc.p.x - (cosf(rotationToRadian(random_angle)) * amount),
                PTarget->loc.p.y,
                PTarget->loc.p.z + (sinf(rotationToRadian(random_angle)) * amount),
                0,
                0,
            };
            position = potential_position;
        }

        bool position_found = false;
        for (auto& potential_position : positions)
        {
            // Validate position
            if (trustcontrollerrepositioncandidate::ShouldSelect(
                    position_found,
                    [&]() { return POwner->PAI->PathFind->ValidPosition(potential_position); },
                    [&]() { return POwner->CanSeeTarget(potential_position); }))
            {
                position_found  = true;
                target_position = potential_position;
                m_InTransit     = true;
            }
        }

        m_LastRepositionTime = m_Tick;
    }

    // Get somewhat close to the target destination
    const auto completionPlan = trustcontrollerreposition::ResolveCompletion(
        distance(POwner->loc.p, target_position), m_failedRepositionAttempts);
    if (completionPlan.action == trustcontrollerreposition::CompletionAction::Path)
    {
        POwner->PAI->PathFind->PathTo(target_position, PATHFLAG_RUN | PATHFLAG_WALLHACK);
    }
    else
    {
        FaceTarget(PTarget->targid);
        if (completionPlan.clearTransit)
        {
            m_InTransit = false;
        }
    }
}

bool CTrustController::Ability(uint16 targid, uint16 abilityid)
{
    TracyZoneScoped;

    const bool hasRecast = static_cast<CMobEntity*>(POwner)->PRecastContainer->HasRecast(RECAST_ABILITY, static_cast<Recast>(abilityid), 0s);
    if (!trustcontrollerability::CanStart(hasRecast, [&]() { return POwner->PAI->CanChangeState(); }))
    {
        return false;
    }

    return POwner->PAI->Internal_Ability(targid, abilityid);
}

bool CTrustController::RangedAttack(uint16 targid)
{
    TracyZoneScoped;

    timer::duration rangedDelay{};
    bool            hasRangedWeapon = false;
    if (CItemWeapon* PRange = dynamic_cast<CItemWeapon*>(POwner->m_Weapons[SLOT_RANGED]))
    {
        hasRangedWeapon = true;
        rangedDelay     = std::chrono::milliseconds(PRange->getDelay());
    }

    if (trustcontrollerrangedattack::CanStart(m_Tick - m_LastRangedAttackTime, hasRangedWeapon, rangedDelay, m_InTransit))
    {
        FaceTarget(PTarget->targid);
        const auto dispatch = trustcontrollerrangedattackdispatch::Resolve(
            true,
            [&]() { return POwner->PAI->CanChangeState(); },
            [&]() { return POwner->PAI->Internal_RangedAttack(targid); });
        if (dispatch.updateCooldown)
        {
            m_LastRangedAttackTime = m_Tick;
        }
        return dispatch.handled;
    }
    return false;
}

bool CTrustController::Cast(uint16 targid, SpellID spellid)
{
    TracyZoneScoped;

    FaceTarget(targid);

    if (!trustcontrollercastrecastadmission::CanStart(
            static_cast<CMobEntity*>(POwner)->PRecastContainer->Has(RECAST_MAGIC, static_cast<Recast>(spellid))))
    {
        return false;
    }

    auto* PSpell = spell::GetSpell(spellid);
    if (trustcontrollercasttargetsource::ShouldUseSelf(PSpell->getValidTarget() == TARGET_SELF))
    {
        targid = POwner->targid;
    }

    auto PTarget      = (CBattleEntity*)POwner->GetEntity(targid, TYPE_MOB | TYPE_PC | TYPE_PET | TYPE_TRUST);
    auto PSpellFamily = PSpell->getSpellFamily();
    bool canCast      = true;

    // clang-format off
    static_cast<CCharEntity*>(POwner->PMaster)->ForPartyWithTrusts([&](CBattleEntity* PMember)
    {
        if (PMember->objtype == TYPE_TRUST && PMember->PAI->IsCurrentState<CMagicState>())
        {
            auto MState = static_cast<CMagicState*>(PMember->PAI->GetCurrentState());

            if (MState)
            {
                auto MSpell       = MState->GetSpell();
                auto MTarget      = MState->GetTarget();
                auto MSpellFamily = MSpell->getSpellFamily();
                auto MSpellID     = MSpell->getID();
                auto sameTarget   = PTarget == MTarget;
                auto targetHPP    = (PSpell->isCure() && sameTarget) ? PTarget->GetHPP() : 0;

                if (!trustcontrollercastcoordination::CanCast(
                        PSpell->isBuff(), PSpell->isCure(), PSpell->isDebuff(), PSpell->isNa(), PSpellFamily,
                        static_cast<uint16>(spellid), MSpellFamily, static_cast<uint16>(MSpellID), sameTarget,
                        targetHPP))
                {
                    canCast = false;
                }
            }
        }
    });
    // clang-format on

    if (!canCast)
    {
        return false;
    }

    return CMobController::Cast(targid, spellid);
}

CBattleEntity* CTrustController::GetTopEnmity()
{
    TracyZoneScoped;

    CBattleEntity* PEntity = nullptr;
    if (auto* PMob = dynamic_cast<CMobEntity*>(POwner->PMaster->GetBattleTarget());
        trustcontrollertopenmitytarget::ShouldRead(PMob != nullptr))
    {
        return PMob->PEnmityContainer->GetHighestEnmity();
    }
    return PEntity;
}

uint8 CTrustController::GetPartyPosition()
{
    TracyZoneScoped;

    auto& trustList = static_cast<CCharEntity*>(POwner->PMaster)->PTrusts;
    return trustcontrollerpartyposition::Resolve(
        trustList, POwner->id, [](const auto* PTrust) { return PTrust->id; });
}
