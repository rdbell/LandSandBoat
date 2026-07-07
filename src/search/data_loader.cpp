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
#include "common/database.h"
#include "common/earth_time.h"
#include "common/logging.h"
#include "common/settings.h"

#include "data_loader.h"
#include "search.h"
#include "search_auction_rows.h"
#include "search_player_filter.h"
#include "search_player_query_filter.h"
#include "search_player_state.h"

CDataLoader::CDataLoader()
{
}

CDataLoader::~CDataLoader()
{
}

/************************************************************************
 *                                                                       *
 *  Returns the auction house sale history for a given item.             *
 *                                                                       *
 ************************************************************************/

std::vector<ahHistory*> CDataLoader::GetAHItemHistory(uint16 ItemID, bool stack)
{
    std::vector<ahHistory*> HistoryList;

    const auto query = BuildAuctionHistoryQuery(ItemID, stack);
    auto       rset  = db::preparedStmt(query.sql, query.itemID, query.stack);

    if (rset && rset->rowsCount())
    {
        while (rset->next())
        {
            ahHistory* PAHHistory = new ahHistory;

            PAHHistory->Price = rset->get<uint32>("sale");
            PAHHistory->Data  = rset->get<uint32>("sell_date");

            PAHHistory->Name1 = rset->get<std::string>("seller_name");
            PAHHistory->Name2 = rset->get<std::string>("buyer_name");

            HistoryList.emplace_back(PAHHistory);
        }
        OrderAuctionHistoryForPacket(HistoryList);
    }
    return HistoryList;
}

/************************************************************************
 *                                                                       *
 *  The list of items sold in this category                              *
 *                                                                       *
 ************************************************************************/

std::vector<ahItem*> CDataLoader::GetAHItemsToCategory(uint8 ahCategoryID, const std::string& orderByString)
{
    ShowDebugFmt("Try find category: {}", ahCategoryID);

    std::vector<ahItem*> ItemList;

    const auto rset = [&]()
    {
        const auto queryStr = BuildAuctionCategoryListQuery(settings::get<bool>("search.OMIT_NO_HISTORY"), orderByString);
        return db::preparedStmt(queryStr, ahCategoryID);
    }();

    if (rset && rset->rowsCount())
    {
        while (rset->next())
        {
            ahItem* PAHItem = new ahItem(BuildAuctionCategoryItem(rset->get<uint16>("itemid"),
                                                                  rset->getOrDefault<uint32>("COUNT(*)-SUM(stack)", 0),
                                                                  rset->getOrDefault<uint32>("SUM(stack)", 0),
                                                                  rset->get<uint32>("stackSize"),
                                                                  ahCategoryID));
            ItemList.emplace_back(PAHItem);
        }
    }

    return ItemList;
}

// Return single item including category and how many are listed
ahItem CDataLoader::GetAHItemFromItemID(uint16 ItemID)
{
    ahItem CAHItem = BuildAuctionItemFromIDRow(ItemID, 0, 0, 0);

    const auto query = BuildAuctionItemFromIDQuery(ItemID);
    auto       rset  = db::preparedStmt(query.sql, query.itemID);
    FOR_DB_SINGLE_RESULT(rset)
    {
        CAHItem = BuildAuctionItemFromIDRow(ItemID,
                                            rset->get<uint16>("aH"),
                                            rset->getOrDefault<uint32>("COUNT(*)-SUM(stack)", 0),
                                            rset->getOrDefault<uint32>("SUM(stack)", 0));
    }
    return CAHItem;
}

/************************************************************************
 *                                                                       *
 *  Returns the number of active players in the world.                   *
 *                                                                       *
 ************************************************************************/

uint32 CDataLoader::GetPlayersCount(const search_req& sr)
{
    const auto query = BuildSearchPlayerCountQuery(sr);
    if (query.filtersJob)
    {
        auto rset = db::preparedStmt(query.sql, query.jobID);
        if (rset && rset->rowsCount() && rset->next())
        {
            return rset->get<uint32>("COUNT(*)");
        }
    }
    else
    {
        auto rset = db::preparedStmt(query.sql);
        if (rset && rset->rowsCount() && rset->next())
        {
            return rset->get<uint32>("COUNT(*)");
        }
    }
    return 0;
}

/************************************************************************
 *                                                                       *
 *  Returns the list of characters found in the world that match the     *
 *  search request.                                                      *
 *          Job ID is 0 for none specified.                              *
 ************************************************************************/

std::list<SearchEntity*> CDataLoader::GetPlayersList(search_req sr, int* count)
{
    std::list<SearchEntity*> PlayersList;
    std::string              filterQry = BuildSearchPlayerQueryFilter(sr);
    std::string              fmtQuery = BuildSearchPlayerListQuery(filterQry);

    auto rset = db::preparedStmt(fmtQuery);
    if (rset && rset->rowsCount())
    {
        int totalResults   = 0; // gives ALL matching criteria (total)
        int visibleResults = 0; // capped at first 20
        while (rset->next())
        {
            SearchEntity* PPlayer = new SearchEntity();

            PPlayer->name = rset->get<std::string>("charname");

            PPlayer->id       = rset->get<uint32>("charid");
            PPlayer->zone     = rset->get<uint16>("pos_zone");
            PPlayer->prevzone = rset->get<uint16>("pos_prevzone");
            PPlayer->nation   = rset->get<uint8>("nation");
            PPlayer->mjob     = rset->get<uint8>("mjob");
            PPlayer->sjob     = rset->get<uint8>("sjob");
            PPlayer->mlvl     = rset->get<uint8>("mlvl");
            PPlayer->slvl     = rset->get<uint8>("slvl");
            PPlayer->race     = rset->get<uint8>("race");

            // TODO: Use a nation enum?
            switch (PPlayer->nation)
            {
                case 0:
                    PPlayer->rank = rset->get<uint8>("rank_sandoria");
                    break;
                case 1:
                    PPlayer->rank = rset->get<uint8>("rank_bastok");
                    break;
                case 2:
                    PPlayer->rank = rset->get<uint8>("rank_windurst");
                    break;
                default:
                    ShowWarningFmt("Inconsistent player nation allegiance : {}", PPlayer->nation);
                    PPlayer->rank = static_cast<uint8>(0U);
                    break;
            }

            const auto settingsInt = rset->get<uint32>("settings");
            PPlayer->languages     = rset->get<uint8>("languages");
            PPlayer->linkshellid1  = rset->get<uint32>("linkshellid1");
            PPlayer->linkshellid2  = rset->get<uint32>("linkshellid2");
            PPlayer->seacom_type   = rset->get<uint8>("seacom_type");
            PPlayer->disconnecting = rset->get<bool>("disconnecting");
            PPlayer->gmHidden      = rset->get<bool>("gmHiddenEnabled");
            PPlayer->muted         = rset->get<bool>("muted");
            PPlayer->unityLeader   = rset->get<uint8>("unity_leader");
            const auto partyid     = rset->getOrDefault<uint32>("partyid", 0);

            NormalizeSearchPlayerForList(*PPlayer, settingsInt, partyid);

            if (!SearchPlayerMatchesRequest(*PPlayer, sr))
            {
                continue;
            }

            if (visibleResults < 40)
            {
                PlayersList.emplace_back(PPlayer);
                visibleResults++;
            }
            totalResults++;
        }
        if (totalResults > 0)
        {
            *count = totalResults;
        }
        ShowInfoFmt("Found {} results, displaying {}", totalResults, visibleResults);
    }

    return PlayersList;
}

/************************************************************************
 *                                                                       *
 *  Returns the list of characters in a given party/alliance group.      *
 *                                                                       *
 ************************************************************************/

std::list<SearchEntity*> CDataLoader::GetPartyList(uint32 PartyID, uint32 AllianceID)
{
    std::list<SearchEntity*> PartyList;

    const auto query = BuildSearchPartyListQuery(PartyID, AllianceID);
    auto       rset  = db::preparedStmt(query.sql, query.firstParam, query.secondParam);
    if (rset && rset->rowsCount())
    {
        while (rset->next())
        {
            SearchEntity* PPlayer = new SearchEntity();

            PPlayer->name   = rset->get<std::string>("charname");
            PPlayer->id     = rset->get<uint32>("charid");
            PPlayer->zone   = rset->get<uint16>("pos_zone");
            PPlayer->nation = rset->get<uint8>("nation");
            PPlayer->mjob   = rset->get<uint8>("mjob");
            PPlayer->sjob   = rset->get<uint8>("sjob");
            PPlayer->mlvl   = rset->get<uint8>("mlvl");
            PPlayer->slvl   = rset->get<uint8>("slvl");
            PPlayer->race   = rset->get<uint8>("race");

            // TODO: Use a nation enum?
            switch (PPlayer->nation)
            {
                case 0:
                    PPlayer->rank = rset->get<uint8>("rank_sandoria");
                    break;
                case 1:
                    PPlayer->rank = rset->get<uint8>("rank_bastok");
                    break;
                case 2:
                    PPlayer->rank = rset->get<uint8>("rank_windurst");
                    break;
                default:
                    ShowWarningFmt("Inconsistent player nation allegiance : {}", PPlayer->nation);
                    PPlayer->rank = static_cast<uint8>(0U);
                    break;
            }

            const auto settingsInt = rset->get<uint32>("settings");
            PPlayer->languages     = rset->get<uint8>("languages");
            PPlayer->seacom_type   = rset->get<uint8>("seacom_type");
            PPlayer->disconnecting = rset->get<bool>("disconnecting");

            NormalizeSearchPartyMemberForList(*PPlayer, settingsInt, PartyID);

            PartyList.emplace_back(PPlayer);
        }
    }
    return PartyList;
}

/************************************************************************
 *                                                                       *
 *  Returns the list of characters in a given linkshell.                 *
 *                                                                       *
 ************************************************************************/

std::list<SearchEntity*> CDataLoader::GetLinkshellList(uint32 LinkshellID)
{
    std::list<SearchEntity*> LinkshellList;

    const auto query = BuildSearchLinkshellListQuery(LinkshellID);
    auto       rset  = db::preparedStmt(query.sql, query.firstParam, query.secondParam);
    if (rset && rset->rowsCount())
    {
        while (rset->next())
        {
            SearchEntity* PPlayer = new SearchEntity();

            PPlayer->name   = rset->get<std::string>("charname");
            PPlayer->id     = rset->get<uint32>("charid");
            PPlayer->zone   = rset->get<uint16>("pos_zone");
            PPlayer->nation = rset->get<uint8>("nation");
            PPlayer->mjob   = rset->get<uint8>("mjob");
            PPlayer->sjob   = rset->get<uint8>("sjob");
            PPlayer->mlvl   = rset->get<uint8>("mlvl");
            PPlayer->slvl   = rset->get<uint8>("slvl");
            PPlayer->race   = rset->get<uint8>("race");

            // TODO: Use a nation enum?
            switch (PPlayer->nation)
            {
                case 0:
                    PPlayer->rank = rset->get<uint8>("rank_sandoria");
                    break;
                case 1:
                    PPlayer->rank = rset->get<uint8>("rank_bastok");
                    break;
                case 2:
                    PPlayer->rank = rset->get<uint8>("rank_windurst");
                    break;
                default:
                    ShowWarningFmt("Inconsistent player nation allegiance : {}", PPlayer->nation);
                    PPlayer->rank = (uint8)0;
                    break;
            }

            PPlayer->linkshellid1   = rset->get<uint32>("linkshellid1");
            PPlayer->linkshellid2   = rset->get<uint32>("linkshellid2");
            PPlayer->linkshellrank1 = rset->get<uint8>("linkshellrank1");
            PPlayer->linkshellrank2 = rset->get<uint8>("linkshellrank2");
            PPlayer->disconnecting  = rset->get<bool>("disconnecting");

            const auto partyid = rset->getOrDefault<uint32>("partyid", 0);

            const auto settingsInt = rset->get<uint32>("settings");
            NormalizeSearchLinkshellMemberForList(*PPlayer, settingsInt, partyid);

            LinkshellList.emplace_back(PPlayer);
        }
    }

    return LinkshellList;
}

std::string CDataLoader::GetSearchComment(uint32 playerId)
{
    const auto query = BuildSearchCommentQuery(playerId);
    auto       rset  = db::preparedStmt(query.sql, query.playerID);
    if (rset && rset->rowsCount() && rset->next())
    {
        return rset->get<std::string>("seacom_message");
    }
    return std::string();
}

void CDataLoader::ExpireAHItems(uint16 expireAgeInDays)
{
    ShowInfoFmt("Expiring auction house listings over {} days old", expireAgeInDays);

    std::vector<ListingToExpire> listingsToExpire;

    const auto query = BuildExpiredAuctionListingsQuery(expireAgeInDays, earth_time::timestamp());
    const auto rset0 = db::preparedStmt(query.sql, query.cutoff);

    const auto expiredAuctions = rset0->rowsCount();

    if (rset0 && expiredAuctions > 0)
    {
        while (rset0->next())
        {
            // Collect the items we're going to expire
            uint32 saleID    = rset0->get<uint32>("id");
            uint32 itemID    = rset0->get<uint32>("itemid");
            uint8  itemStack = rset0->get<uint8>("stacksize");
            uint8  ahStack   = rset0->get<uint8>("stack");
            uint32 sellerID  = rset0->get<uint32>("seller");
            // NOTE: seller name left out for now, we'll populate this later

            listingsToExpire.emplace_back(BuildListingToExpire(saleID, itemID, itemStack, ahStack, sellerID));
        }

        for (auto listing : listingsToExpire)
        {
            // Populate name now
            const auto rset1 = db::preparedStmt("SELECT charname FROM chars WHERE charid = ?", listing.sellerID);
            if (rset1 && rset1->rowsCount() && rset1->next())
            {
                listing.sellerName = rset1->get<std::string>("charname");
            }

            const auto rset2 = db::preparedStmt("INSERT INTO delivery_box (charid, charname, box, itemid, itemsubid, quantity, senderid, sender) VALUES "
                                                "(?, ?, 1, ?, 0, ?, 0, 'AH-Jeuno')",
                                                listing.sellerID,
                                                listing.sellerName,
                                                listing.itemID,
                                                AuctionExpiredDeliveryQuantity(listing));
            if (rset2 && rset2->rowsAffected())
            {
                // delete the item from the auction house
                db::preparedStmt("DELETE FROM auction_house WHERE id = ?", listing.saleID);
            }
        }
    }
    ShowInfoFmt("Sent {} expired auction house listings back to sellers", expiredAuctions);
}
