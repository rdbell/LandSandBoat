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

#include "0x05a_motionmes.h"
#include "motionmes_runtime.h"

#include "entities/char_entity.h"
#include "entities/npc_entity.h"
#include "item_container.h"
#include "items/item_weapon.h"

GP_SERV_COMMAND_MOTIONMES::GP_SERV_COMMAND_MOTIONMES(const CCharEntity* PChar, const uint32 targetId, const uint16 targetIndex, Emote emoteId, EmoteMode emoteMode, const uint16 extra)
{
    auto& packet = this->data();

    auto facts = motionmeshelpers::CharacterFacts{ .nation = PChar->profile.nation };
    if (emoteId == Emote::Hurray)
    {
        if (const auto* mainWeapon = PChar->getEquip(SLOT_MAIN))
        {
            facts.hasMainWeapon = true;
            facts.mainWeaponID  = mainWeapon->getID();
        }
    }
    else if (emoteId == Emote::Aim)
    {
        if (const auto* rangedWeapon = static_cast<CItemWeapon*>(PChar->getEquip(SLOT_RANGED)))
        {
            facts.hasRangedWeapon                     = true;
            facts.rangedWeaponID                      = rangedWeapon->getID();
            facts.rangedWeaponIsThrowing              = rangedWeapon->getSkillType() == SKILL_THROWING;
            facts.rangedWeaponIsArcheryOrMarksmanship = rangedWeapon->getSkillType() == SKILL_MARKSMANSHIP || rangedWeapon->getSkillType() == SKILL_ARCHERY;
        }

        if (facts.hasRangedWeapon && facts.rangedWeaponID != 65535 && facts.rangedWeaponIsArcheryOrMarksmanship)
        {
            if (const auto* ammoWeapon = static_cast<CItemWeapon*>(PChar->getEquip(SLOT_AMMO)))
            {
                facts.hasAmmoWeapon = true;
                facts.ammoWeaponID  = ammoWeapon->getID();
            }
        }
    }

    const auto plan = motionmeshelpers::CharacterPlanFor(emoteId, emoteMode, extra, facts);

    packet.CasUniqueNo = PChar->id;
    packet.CasActIndex = PChar->targid;
    packet.TarUniqueNo = targetId;
    packet.TarActIndex = targetIndex;
    packet.MesNum      = plan.messageNumber;
    packet.Param       = plan.parameter;
    packet.Mode        = plan.mode;
}

GP_SERV_COMMAND_MOTIONMES::GP_SERV_COMMAND_MOTIONMES(const CNpcEntity* PEntity, const uint32 targetId, const uint16 targetIndex, Emote emoteId, EmoteMode emoteMode)
{
    auto& packet = this->data();
    const auto plan = motionmeshelpers::NPCPlanFor(emoteId, emoteMode);

    packet.CasUniqueNo = PEntity->id;
    packet.TarUniqueNo = targetId;
    packet.CasActIndex = PEntity->targid;
    packet.TarActIndex = targetIndex;
    packet.MesNum      = plan.messageNumber;
    packet.Param       = plan.parameter;
    packet.Mode        = plan.mode;
}
