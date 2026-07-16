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

#include "0x063_miscdata_monstrosity.h"

#include "entities/char_entity.h"
#include "monstrosity.h"
#include "utils/charutils.h"

auto miscdatamonstrosityhelpers::Plan1(const Facts& facts) -> GP_SERV_COMMAND_MISCDATA::MONSTROSITY1::PacketData
{
    auto packet      = GP_SERV_COMMAND_MISCDATA::MONSTROSITY1::PacketData{};
    packet.type      = GP_SERV_COMMAND_MISCDATA_TYPE::Monstrosity1;
    packet.unknown06 = sizeof(packet) - 8;
    if (!facts.hasMonstrosity)
    {
        return packet;
    }

    packet.species     = facts.species;
    packet.flags       = facts.flags;
    packet.rank        = static_cast<uint8>(std::min(2, (facts.infamy - 1) / 10000));
    packet.unknown1[0] = 0xEC;
    packet.infamy      = facts.infamy;
    packet.unknown2    = 0x2C;
    std::memcpy(packet.instincts, facts.instincts.data(), sizeof(packet.instincts));
    std::memcpy(packet.levels, facts.levels.data(), sizeof(packet.levels));
    return packet;
}

auto miscdatamonstrosityhelpers::Plan2(const Facts& facts) -> GP_SERV_COMMAND_MISCDATA::MONSTROSITY2::PacketData
{
    auto packet      = GP_SERV_COMMAND_MISCDATA::MONSTROSITY2::PacketData{};
    packet.type      = GP_SERV_COMMAND_MISCDATA_TYPE::Monstrosity2;
    packet.unknown06 = sizeof(packet) - 8;
    if (!facts.hasMonstrosity)
    {
        return packet;
    }

    packet.slimeLevel    = facts.levels[126];
    packet.sprigganLevel = facts.levels[127];
    std::memcpy(packet.instincts2, facts.instincts.data() + 20, sizeof(packet.instincts2));
    std::memcpy(packet.variants, facts.variants.data(), sizeof(packet.variants));
    return packet;
}

GP_SERV_COMMAND_MISCDATA::MONSTROSITY1::MONSTROSITY1(CCharEntity* PChar)
{
    auto facts = miscdatamonstrosityhelpers::Facts{};
    if (PChar->m_PMonstrosity)
    {
        facts.hasMonstrosity = true;
        facts.species        = PChar->m_PMonstrosity->Species;
        facts.flags          = PChar->m_PMonstrosity->Flags;
        facts.infamy         = charutils::GetPoints(PChar, "infamy");
        facts.instincts      = PChar->m_PMonstrosity->instincts;
        facts.levels         = PChar->m_PMonstrosity->levels;
    }
    this->data() = miscdatamonstrosityhelpers::Plan1(facts);
}

GP_SERV_COMMAND_MISCDATA::MONSTROSITY2::MONSTROSITY2(const CCharEntity* PChar)
{
    auto facts = miscdatamonstrosityhelpers::Facts{};
    if (PChar->m_PMonstrosity)
    {
        facts.hasMonstrosity = true;
        facts.instincts      = PChar->m_PMonstrosity->instincts;
        facts.levels         = PChar->m_PMonstrosity->levels;
        facts.variants       = PChar->m_PMonstrosity->variants;
    }
    this->data() = miscdatamonstrosityhelpers::Plan2(facts);
}
