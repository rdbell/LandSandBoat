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

#include "player_controller.h"
#include "player_charm_controller_action_capacity.h"

#ifndef _PLAYERCHARMCONTROLLER_H
#define _PLAYERCHARMCONTROLLER_H

class CCharEntity;

class CPlayerCharmController : public CPlayerController
{
public:
    CPlayerCharmController(CCharEntity*);
    virtual ~CPlayerCharmController();

    virtual auto Tick(timer::time_point tick) -> Task<void> override;

    virtual bool Cast(uint16 targid, SpellID spellid) override
    {
        return playercharmcontrolleraction::CanDispatch(playercharmcontrolleraction::Action::Cast);
    }

    virtual bool ChangeTarget(uint16 targid) override
    {
        return playercharmcontrolleraction::CanDispatch(playercharmcontrolleraction::Action::ChangeTarget);
    }

    virtual bool WeaponSkill(uint16 targid, uint16 wsid) override
    {
        return playercharmcontrolleraction::CanDispatch(playercharmcontrolleraction::Action::WeaponSkill);
    }

    virtual bool Ability(uint16 targid, uint16 abilityid) override
    {
        return playercharmcontrolleraction::CanDispatch(playercharmcontrolleraction::Action::Ability);
    }

    virtual bool RangedAttack(uint16 targid) override
    {
        return playercharmcontrolleraction::CanDispatch(playercharmcontrolleraction::Action::RangedAttack);
    }

private:
    static constexpr float RoamDistance{ 2.1f };
    void                   DoCombatTick(timer::time_point tick);
    void                   DoRoamTick(timer::time_point tick);
};

#endif // _PLAYERCONTROLLER
