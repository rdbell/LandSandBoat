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

#include "0x061_clistatus.h"
#include "clistatus_runtime.h"

#include <cstring>

#include "entities/char_entity.h"
#include "items/item_weapon.h"
#include "modifier.h"
#include "roe.h"
#include "utils/charutils.h"

GP_SERV_COMMAND_CLISTATUS::GP_SERV_COMMAND_CLISTATUS(CCharEntity* PChar)
{
    auto  facts  = clistatushelpers::Facts{};
    auto& status = facts.status;

    status.hpmax    = PChar->GetMaxHP();
    status.mpmax    = PChar->GetMaxMP();
    status.mjob_no  = PChar->GetMJob();
    status.mjob_lv  = PChar->GetMLevel();
    status.sjob_no  = PChar->GetSJob();
    status.sjob_lv  = PChar->GetSLevel();
    status.exp_now  = PChar->jobs.exp[PChar->GetMJob()];
    status.exp_next = charutils::GetExpNEXTLevel(PChar->jobs.job[PChar->GetMJob()]);

    std::memcpy(status.bp_base, &PChar->stats, sizeof(status.bp_base));

    facts.mainWeaponTwoHanded = false;
    if (const auto* weapon = dynamic_cast<CItemWeapon*>(PChar->m_Weapons[SLOT_MAIN]); weapon && weapon->isTwoHanded())
    {
        facts.mainWeaponTwoHanded = true;
    }
    facts.baseParamModifiers = { PChar->getMod(Mod::STR), PChar->getMod(Mod::DEX), PChar->getMod(Mod::VIT), PChar->getMod(Mod::AGI), PChar->getMod(Mod::INT), PChar->getMod(Mod::MND), PChar->getMod(Mod::CHR) };
    facts.twoHandedSTR       = PChar->getMod(Mod::TWOHAND_STR);

    status.atk         = PChar->ATT(SLOT_MAIN);
    status.def         = PChar->DEF();
    status.def_elem[0] = PChar->getMod(Mod::FIRE_MEVA);
    status.def_elem[1] = PChar->getMod(Mod::ICE_MEVA);
    status.def_elem[2] = PChar->getMod(Mod::WIND_MEVA);
    status.def_elem[3] = PChar->getMod(Mod::EARTH_MEVA);
    status.def_elem[4] = PChar->getMod(Mod::THUNDER_MEVA);
    status.def_elem[5] = PChar->getMod(Mod::WATER_MEVA);
    status.def_elem[6] = PChar->getMod(Mod::LIGHT_MEVA);
    status.def_elem[7] = PChar->getMod(Mod::DARK_MEVA);

    status.designation  = PChar->profile.title;
    status.rank         = PChar->profile.rank[PChar->profile.nation];
    status.rankbar      = PChar->profile.rankpoints;
    status.BindZoneNo   = PChar->profile.home_point.destination;
    status.nation       = PChar->profile.nation;
    status.su_lv        = PChar->getMod(Mod::SUPERIOR_LEVEL);
    status.highest_ilvl = charutils::getMaxItemLevel(PChar);
    status.ilvl         = charutils::getItemLevelDifference(PChar);
    status.ilvl_mhand   = charutils::getMainhandItemLevel(PChar);
    status.ilvl_ranged  = charutils::getRangedItemLevel(PChar);

    facts.unityLeader       = PChar->profile.unity_leader;
    facts.unityRank         = PChar->profile.unity_leader > 0 ? roeutils::RoeSystem.unityLeaderRank[PChar->profile.unity_leader - 1] : 0;
    facts.unityAccolades    = charutils::GetPoints(PChar, "unity_accolades");
    facts.currentAccolades  = charutils::GetPoints(PChar, "current_accolades");
    facts.previousAccolades = charutils::GetPoints(PChar, "prev_accolades");
    facts.unityChat         = PChar->PUnityChat;

    this->data() = clistatushelpers::PlanFor(facts);
}
