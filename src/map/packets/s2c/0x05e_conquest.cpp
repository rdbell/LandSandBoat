/*
===========================================================================

  Copyright (c) 2025 LandSandBoat Dev Teams

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

#include "0x05e_conquest.h"
#include "conquest_runtime.h"

#include "besieged_system.h"
#include "conquest_data.h"
#include "conquest_system.h"
#include "entities/char_entity.h"
#include "utils/charutils.h"

GP_SERV_COMMAND_CONQUEST::GP_SERV_COMMAND_CONQUEST(CCharEntity* PChar)
{
    auto facts = conquesthelpers::Facts{};

    const auto& conquestData = conquest::GetConquestData();

    facts.regionControls = {
        conquestData.getRegionControlCount(NATION_SANDORIA),
        conquestData.getRegionControlCount(NATION_BASTOK),
        conquestData.getRegionControlCount(NATION_WINDURST),
    };
    facts.prevRegionControls = {
        conquestData.getPrevRegionControlCount(NATION_SANDORIA),
        conquestData.getPrevRegionControlCount(NATION_BASTOK),
        conquestData.getPrevRegionControlCount(NATION_WINDURST),
    };
    facts.currentRegion  = PChar->loc.zone->GetRegionID();
    facts.nextTally      = conquest::GetNextTally();
    facts.conquestPoints = charutils::GetPoints(PChar, charutils::GetConquestPointsName(PChar).c_str());

    for (auto regionId = static_cast<uint8>(REGION_TYPE::RONFAURE); regionId <= static_cast<uint8>(REGION_TYPE::TAVNAZIA); regionId++)
    {
        facts.regions[regionId] = {
            conquestData.getRegionOwner(static_cast<REGION_TYPE>(regionId)),
            conquestData.getInfluence(static_cast<REGION_TYPE>(regionId), NATION_SANDORIA),
            conquestData.getInfluence(static_cast<REGION_TYPE>(regionId), NATION_BASTOK),
            conquestData.getInfluence(static_cast<REGION_TYPE>(regionId), NATION_WINDURST),
            conquestData.getInfluence(static_cast<REGION_TYPE>(regionId), NATION_BEASTMEN),
        };
    }

    facts.besieged.overview = {
        besieged::GetAstralCandescence(),
        besieged::GetAlZahbiOrders(),
        besieged::GetMamookLevel(),
        besieged::GetHalvungLevel(),
        besieged::GetArrapagoLevel(),
        besieged::GetMamookOrders(),
        besieged::GetHalvungOrders(),
        besieged::GetArrapagoOrders(),
    };
    facts.besieged.mamook = {
        besieged::GetMamookOrders(),
        besieged::GetMamookForces(),
        besieged::GetMamookLevel(),
        besieged::GetMamookMirrorDestroyed(),
        besieged::GetMamookMirrors(),
        besieged::GetMamookPrisoners(),
    };
    facts.besieged.halvung = {
        besieged::GetHalvungOrders(),
        besieged::GetHalvungForces(),
        besieged::GetHalvungLevel(),
        besieged::GetHalvungMirrorDestroyed(),
        besieged::GetHalvungMirrors(),
        besieged::GetHalvungPrisoners(),
    };
    facts.besieged.arrapago = {
        besieged::GetArrapagoOrders(),
        besieged::GetArrapagoForces(),
        besieged::GetArrapagoLevel(),
        besieged::GetArrapagoMirrorDestroyed(),
        besieged::GetArrapagoMirrors(),
        besieged::GetArrapagoPrisoners(),
    };
    facts.besieged.imperialStanding = charutils::GetPoints(PChar, "imperial_standing");

    this->data() = conquesthelpers::PlanFor(facts);
}
