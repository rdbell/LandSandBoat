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

#include "0x11a_emote_list.h"

#include <cstring>

#include "entities/char_entity.h"
#include "enums/key_items.h"
#include "utils/charutils.h"

GP_SERV_COMMAND_EMOTE_LIST::GP_SERV_COMMAND_EMOTE_LIST(const CCharEntity* PChar)
{
    auto& packet = this->data();

    const emotelisthelpers::Ownership ownership{
        .jobEmotes = {
            charutils::hasKeyItem(PChar, KeyItem::JOB_GESTURE_WARRIOR),
            charutils::hasKeyItem(PChar, KeyItem::JOB_GESTURE_MONK),
            charutils::hasKeyItem(PChar, KeyItem::JOB_GESTURE_WHITE_MAGE),
            charutils::hasKeyItem(PChar, KeyItem::JOB_GESTURE_BLACK_MAGE),
            charutils::hasKeyItem(PChar, KeyItem::JOB_GESTURE_RED_MAGE),
            charutils::hasKeyItem(PChar, KeyItem::JOB_GESTURE_THIEF),
            charutils::hasKeyItem(PChar, KeyItem::JOB_GESTURE_PALADIN),
            charutils::hasKeyItem(PChar, KeyItem::JOB_GESTURE_DARK_KNIGHT),
            charutils::hasKeyItem(PChar, KeyItem::JOB_GESTURE_BEASTMASTER),
            charutils::hasKeyItem(PChar, KeyItem::JOB_GESTURE_BARD),
            charutils::hasKeyItem(PChar, KeyItem::JOB_GESTURE_RANGER),
            charutils::hasKeyItem(PChar, KeyItem::JOB_GESTURE_SAMURAI),
            charutils::hasKeyItem(PChar, KeyItem::JOB_GESTURE_NINJA),
            charutils::hasKeyItem(PChar, KeyItem::JOB_GESTURE_DRAGOON),
            charutils::hasKeyItem(PChar, KeyItem::JOB_GESTURE_SUMMONER),
            charutils::hasKeyItem(PChar, KeyItem::JOB_GESTURE_BLUE_MAGE),
            charutils::hasKeyItem(PChar, KeyItem::JOB_GESTURE_CORSAIR),
            charutils::hasKeyItem(PChar, KeyItem::JOB_GESTURE_PUPPETMASTER),
            charutils::hasKeyItem(PChar, KeyItem::JOB_GESTURE_DANCER),
            charutils::hasKeyItem(PChar, KeyItem::JOB_GESTURE_SCHOLAR),
            charutils::hasKeyItem(PChar, KeyItem::JOB_GESTURE_GEOMANCER),
            charutils::hasKeyItem(PChar, KeyItem::JOB_GESTURE_RUNE_FENCER),
        },
        .chairs = {
            charutils::hasKeyItem(PChar, KeyItem::IMPERIAL_CHAIR),
            charutils::hasKeyItem(PChar, KeyItem::DECORATIVE_CHAIR),
            charutils::hasKeyItem(PChar, KeyItem::ORNATE_STOOL),
            charutils::hasKeyItem(PChar, KeyItem::REFINED_CHAIR),
            charutils::hasKeyItem(PChar, KeyItem::PORTABLE_CONTAINER),
            charutils::hasKeyItem(PChar, KeyItem::CHOCOBO_CHAIR),
            charutils::hasKeyItem(PChar, KeyItem::EPHRAMADIAN_THRONE),
            charutils::hasKeyItem(PChar, KeyItem::SHADOW_THRONE),
            charutils::hasKeyItem(PChar, KeyItem::LEAF_BENCH),
            charutils::hasKeyItem(PChar, KeyItem::ASTRAL_CUBE),
            charutils::hasKeyItem(PChar, KeyItem::CHOCOBO_CHAIR_II),
        },
    };
    const auto plan = emotelisthelpers::PlanFor(ownership);
    std::memcpy(&packet.JobEmotes, &plan.jobEmotes, sizeof(packet.JobEmotes));
    std::memcpy(&packet.Chairs, &plan.chairs, sizeof(packet.Chairs));
}
