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

#include "player_charm_controller.h"
#include "player_charm_controller_combat_capacity.h"
#include "player_charm_controller_roam_capacity.h"

#include "ai/ai_container.h"
#include "common/utils.h"
#include "entities/char_entity.h"
#include "status_effect_container.h"

CPlayerCharmController::CPlayerCharmController(CCharEntity* PChar)
: CPlayerController(PChar)
{
    POwner->PAI->PathFind = std::make_unique<CPathFind>(PChar);
}

CPlayerCharmController::~CPlayerCharmController()
{
    if (POwner->PAI->IsEngaged())
    {
        POwner->PAI->Internal_Disengage();
    }
    POwner->PAI->PathFind.reset();
    POwner->allegiance = ALLEGIANCE_TYPE::PLAYER;
}

auto CPlayerCharmController::Tick(timer::time_point tick) -> Task<void>
{
    m_Tick = tick;

    if (POwner->PMaster == nullptr || !POwner->PMaster->isAlive())
    {
        POwner->StatusEffectContainer->DelStatusEffect(xi::StatusEffect::CharmI);
        co_return;
    }

    if (POwner->PAI->IsEngaged())
    {
        DoCombatTick(tick);
    }
    else
    {
        DoRoamTick(tick);
    }
}

void CPlayerCharmController::DoCombatTick(timer::time_point tick)
{
    if (playercharmcontrollercombat::ShouldDisengage(POwner->PMaster->PAI->IsEngaged()))
    {
        POwner->PAI->Internal_Disengage();
    }

    if (playercharmcontrollercombat::ShouldSyncTarget(POwner->PMaster->GetBattleTargetID() != POwner->GetBattleTargetID()))
    {
        POwner->PAI->Internal_ChangeTarget(POwner->PMaster->GetBattleTargetID());
    }

    auto* PTarget = POwner->GetBattleTarget();
    const bool canFollowPath = PTarget && POwner->PAI->CanFollowPath();
    bool canAttack = false;
    bool hasSpeed = false;
    if (canFollowPath)
    {
        std::unique_ptr<CBasicPacket> err;
        canAttack = POwner->CanAttack(PTarget, err);
        hasSpeed = !canAttack && POwner->GetSpeed() > 0;
    }
    const auto movementPlan = playercharmcontrollercombat::Resolve(true, false, PTarget != nullptr, canFollowPath, canAttack, hasSpeed);
    if (movementPlan.lookAtTarget)
    {
        POwner->PAI->PathFind->LookAt(PTarget->loc.p);
    }
    if (movementPlan.pursueTarget)
    {
        POwner->PAI->PathFind->PathAround(PTarget->loc.p, 2.0f, PATHFLAG_WALLHACK | PATHFLAG_RUN);
        POwner->PAI->PathFind->FollowPath(m_Tick);
    }
}

void CPlayerCharmController::DoRoamTick(timer::time_point tick)
{
    const float currentDistance = distance(POwner->loc.p, POwner->PMaster->loc.p);
    const bool pathFinderExists = POwner->PAI->PathFind != nullptr;
    const bool needsDistantWarp = pathFinderExists && currentDistance > RoamDistance && currentDistance >= 35.0f;
    const bool hasSpeed = needsDistantWarp && POwner->GetSpeed() > 0;
    const auto plan = playercharmcontrollerroam::Resolve(
        POwner->PMaster->PAI->IsEngaged(), pathFinderExists, hasSpeed, currentDistance);
    if (plan.engageMasterTarget)
    {
        POwner->PAI->Internal_Engage(POwner->PMaster->GetBattleTargetID());
    }

    switch (plan.action)
    {
        case playercharmcontrollerroam::Action::Path:
            if (POwner->PAI->PathFind->PathAround(POwner->PMaster->loc.p, 2.0f, PATHFLAG_RUN | PATHFLAG_WALLHACK))
            {
                POwner->PAI->PathFind->FollowPath(m_Tick);
            }
            else if (POwner->GetSpeed() > 0)
            {
                POwner->PAI->PathFind->WarpTo(POwner->PMaster->loc.p, RoamDistance);
            }
            break;
        case playercharmcontrollerroam::Action::Warp:
            POwner->PAI->PathFind->WarpTo(POwner->PMaster->loc.p, RoamDistance);
            break;
        case playercharmcontrollerroam::Action::Hold:
            break;
    }
}
