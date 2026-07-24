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

#pragma once

#include "common/cbasetypes.h"

class CCharEntity;
class CInstance;
class CZone;

namespace instanceloader
{

struct SpawnQueryPlan
{
    uint32 realZoneId;
    uint32 effectiveZoneId;
    uint32 npcMin;
    uint32 npcMax;
};

struct MobLoadQueryPlan
{
    uint32 templateZoneId;
    uint32 instanceId;
    uint32 effectiveZoneId;
};

struct NpcLoadQueryPlan
{
    uint32 instanceId;
    uint32 npcMin;
    uint32 npcMax;
};

auto SpawnQueryPlanFor(uint32 realZoneId, uint32 overlayId) -> SpawnQueryPlan;
auto MobLoadQueryPlanFor(SpawnQueryPlan spawn, uint32 instanceId) -> MobLoadQueryPlan;
auto NpcLoadQueryPlanFor(SpawnQueryPlan spawn, uint32 instanceId) -> NpcLoadQueryPlan;

} // namespace instanceloader

class CInstanceLoader
{
public:
    CInstanceLoader(uint32 instanceid, CCharEntity* PRequester);
    ~CInstanceLoader();

    auto LoadInstance() const -> CInstance*;

private:
    CInstance*   m_PInstance;
    CZone*       m_PZone;
    CCharEntity* m_PRequester;
};
