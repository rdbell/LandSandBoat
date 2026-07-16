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

#include "npc_entity.h"

#include "npc_post_tick.h"

#include "ai/ai_container.h"

#include "packets/entity_update.h"

#include "utils/zoneutils.h"

/************************************************************************
 *                                                                       *
 *                                                                       *
 *                                                                       *
 ************************************************************************/

CNpcEntity::CNpcEntity()
{
    TracyZoneScoped;

    objtype    = TYPE_NPC;
    look.face  = 0x32;
    allegiance = ALLEGIANCE_TYPE::MOB;

    PAI = std::make_unique<CAIContainer>(this);
}

CNpcEntity::~CNpcEntity()
{
    TracyZoneScoped;
}

uint32 CNpcEntity::entityFlags() const
{
    return m_flags;
}

void CNpcEntity::setEntityFlags(uint32 EntityFlags)
{
    m_flags = EntityFlags;
}

void CNpcEntity::hideHP(bool hide)
{
    if (hide)
    {
        m_flags |= 0x100;
    }
    else
    {
        m_flags &= ~0x100;
    }
}

bool CNpcEntity::hpHidden() const
{
    return (m_flags & 0x800) == 0x800;
}

void CNpcEntity::setUntargetable(bool untargetable)
{
    if (untargetable)
    {
        m_flags |= FLAG_UNTARGETABLE;
    }
    else
    {
        m_flags &= ~FLAG_UNTARGETABLE;
    }
}

bool CNpcEntity::GetUntargetable() const
{
    return (m_flags & FLAG_UNTARGETABLE) == FLAG_UNTARGETABLE;
}

bool CNpcEntity::triggerable() const
{
    return triggerable_;
}

void CNpcEntity::setTriggerable(bool triggerable)
{
    triggerable_ = triggerable;
}

auto CNpcEntity::widescan() const -> uint8
{
    return widescan_;
}

void CNpcEntity::setWidescan(uint8 widescan)
{
    widescan_ = widescan;
}

bool CNpcEntity::alwaysRelevant() const
{
    return alwaysRelevant_;
}

void CNpcEntity::setAlwaysRelevant(bool alwaysRelevant)
{
    alwaysRelevant_ = alwaysRelevant;
}

bool CNpcEntity::isWideScannable()
{
    return widescan_ == 1 && status == STATUS_TYPE::NORMAL && CBaseEntity::isWideScannable();
}

void CNpcEntity::PostTick()
{
    const auto now  = timer::now();
    const auto plan = npcentity::PlanPostTick(loc.zone != nullptr, updatemask, status == STATUS_TYPE::DISAPPEAR, now, m_nextUpdateTimer);
    if (plan.sendUpdate)
    {
        m_nextUpdateTimer = plan.nextUpdateTimer;
        loc.zone->UpdateEntityPacket(this, ENTITY_UPDATE, updatemask);
        updatemask = 0;
    }
}
