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

#include "guild.h"
#include "entities/char_entity.h"
#include "guild_capacity.h"
#include "items/item.h"
#include "lua/luautils.h"

#include "utils/charutils.h"
#include "utils/itemutils.h"

CGuild::CGuild(uint8 id, const std::string& _pointsName)
: m_id(id)
{
    earth_time::duration currentTime = std::chrono::seconds(earth_time::vanadiel_timestamp()) - 24h;
    uint32               elapsedDays = std::chrono::floor<std::chrono::days>(currentTime).count();

    for (size_t i = 0; i < m_GPItemsRank.size(); ++i)
    {
        m_GPItemsRank[i] = guildhelpers::PatternRankFromElapsedDays(elapsedDays, i);
    }

    pointsName = _pointsName;
}

CGuild::~CGuild() = default;

uint8 CGuild::id() const
{
    return m_id;
}

void CGuild::updateGuildPointsPattern(uint8 pattern)
{
    for (auto& GPItems : m_GPItems)
    {
        GPItems.clear();
    }

    for (size_t i = 0; i < m_GPItemsRank.size(); ++i)
    {
        m_GPItemsRank[i] = guildhelpers::PatternRankAfterAdvance(m_GPItemsRank[i], i);

        const auto rset = db::preparedStmt("SELECT itemid, points, max_points FROM guild_item_points WHERE "
                                           "guildid = ? AND pattern = ? AND rank = ?",
                                           m_id,
                                           pattern,
                                           m_GPItemsRank[i]);

        if (rset && rset->rowsCount())
        {
            while (rset->next())
            {
                const auto itemId    = rset->get<uint16>("itemid");
                const auto points    = rset->get<uint16>("points");
                const auto maxPoints = rset->get<uint16>("max_points");

                m_GPItems[i].emplace_back(xi::items::lookup(itemId), maxPoints, points);
            }
        }
    }
}

auto CGuild::addGuildPoints(CCharEntity* PChar, const CItem* PItem) const -> std::pair<uint8, int16>
{
    const uint8 rank = guildhelpers::ClampSkillRank(PChar->RealSkills.rank[guildhelpers::SkillRankCharIndex(m_id)]);
    const auto  curPointsRaw = PChar->getCharVar("[GUILD]daily_points");
    const uint16 curPoints   = static_cast<uint16>(curPointsRaw);

    if (!guildhelpers::IsDailyPointsEligible(curPointsRaw))
    {
        // curPoints set to 1 means the player is not eligible for points
        // due to changing guilds recently.
        return { 0, 0 };
    }

    if (PItem)
    {
        for (auto& GPItem : m_GPItems[guildhelpers::SkillRankToTier(rank)])
        {
            if (GPItem.item->getID() == PItem->getID())
            {
                const auto result = guildhelpers::ComputeAddGuildPoints(
                    false,
                    true,
                    curPoints,
                    GPItem.maxpoints,
                    GPItem.points,
                    PItem->getReserve());

                charutils::AddPoints(PChar, pointsName.c_str(), result.pointsToAdd);
                // Tally of earned points expire at JST midnight.
                PChar->setCharVar(
                    "[GUILD]daily_points",
                    guildhelpers::NewDailyPointsTotal(curPoints, static_cast<uint16>(result.pointsToAdd)),
                    luautils::JstMidnight());

                return { result.quantity, result.pointsToAdd };
            }
        }
    }

    return { 0, 0 };
}

auto CGuild::getDailyGPItem(CCharEntity* PChar) const -> std::pair<uint16, uint16>
{
    const uint8 rank = guildhelpers::ClampSkillRank(PChar->RealSkills.rank[guildhelpers::SkillRankCharIndex(m_id)]);

    const auto GPItem    = m_GPItems[guildhelpers::SkillRankToTier(rank)];
    const auto curPointsRaw = PChar->getCharVar("[GUILD]daily_points");
    const auto curPoints    = static_cast<uint16>(curPointsRaw);

    const auto result = guildhelpers::ComputeDailyGPItem(
        !guildhelpers::IsDailyPointsEligible(curPointsRaw),
        GPItem[0].item->getID(),
        curPoints,
        GPItem[0].maxpoints);

    // a rank-up can land player in a new pattern that rewards fewer max points than they
    // have traded in today. Remaining capacity is non-negative via RemainingDailyCapacity.
    return std::make_pair(result.itemId, result.remainingPoints);
}
