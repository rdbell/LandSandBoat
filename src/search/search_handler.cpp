/*
===========================================================================

  Copyright (c) 2023 LandSandBoat Dev Teams

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

#include "search_handler.h"

#include "common/md52.h"
#include "common/timer.h"
#include "common/utils.h"

#include "data_loader.h"

#include <map>
#include <unordered_set>

#include "auction_request_order.h"
#include "packets/auction_history.h"
#include "packets/auction_list.h"
#include "packets/linkshell_list.h"
#include "packets/party_list.h"
#include "packets/search_comment.h"
#include "packets/search_list.h"
#include "search_packet_crypto.h"
#include "search_packet_hash.h"
#include "search_handler_dispatch.h"
#include "search_session_tracker.h"

SearchHandler::SearchHandler(Scheduler& scheduler, asio::ip::tcp::socket socket, SynchronizedShared<std::map<std::string, uint16_t>>& IPAddressesInUseList, SynchronizedShared<std::unordered_set<std::string>>& IPAddressWhitelist)
: scheduler_(scheduler)
, socket_(std::move(socket))
, buffer_{}
, IPAddressesInUse_(IPAddressesInUseList)
, IPAddressWhitelist_(IPAddressWhitelist)
{
    DebugSocketsFmt("New connection from IP {}", socket_.lowest_layer().remote_endpoint().address().to_string());

    asio::error_code ec = {};
    socket_.lowest_layer().set_option(asio::socket_base::reuse_address(true));
    ipAddress_ = socket_.lowest_layer().remote_endpoint(ec).address().to_string();

    if (ec)
    {
        ipAddress_ = "error";
        socket_.lowest_layer().close();
    }
    else
    {
        addToUsedIPAddresses(ipAddress_);

        if (getNumSessionsInUse(ipAddress_) > 5)
        {
            ShowErrorFmt("More than 5 simultaneous connections from {}. Closing socket.", ipAddress_);
            socket_.lowest_layer().close();
            return;
        }
    }
}

SearchHandler::~SearchHandler()
{
    DebugSocketsFmt("Connection from IP {} closed", ipAddress_);
    removeFromUsedIPAddresses(ipAddress_);
}

auto SearchHandler::run() -> Task<void>
{
    auto self = shared_from_this();

    try
    {
        while (socket_.lowest_layer().is_open() && !scheduler_.closeRequested())
        {
            std::memset(buffer_.data(), 0, buffer_.size());

            auto result = co_await scheduler_.withTimeout(
                socket_.async_read_some(asio::buffer(buffer_.data(), buffer_.size()), asio::use_awaitable),
                10s);

            if (!result.has_value()) // timed out
            {
                DebugSocketsFmt("Socket timed out from {}", ipAddress_);
                break;
            }

            const auto length = result.value();
            if (length == 0) // EOF
            {
                break;
            }

            DebugSocketsFmt("Received packet from IP {} ({} bytes)", ipAddress_, length);

            read_func(static_cast<uint16_t>(length));

            while (!searchPackets_.empty())
            {
                auto packet    = searchPackets_.front();
                auto write_len = packet.getSize();

                std::memset(buffer_.data(), 0, buffer_.size());
                std::memcpy(buffer_.data(), packet.getData(), write_len);

                searchPackets_.pop_front();

                encrypt(write_len);

                DebugSocketsFmt("Sending packet to IP {} ({} bytes)", ipAddress_, write_len);

                co_await socket_.async_write_some(asio::buffer(buffer_.data(), write_len), asio::use_awaitable);
            }
        }
    }
    catch (const std::exception& e)
    {
        DebugSocketsFmt("Socket error from IP {}: {}", ipAddress_, e.what());
    }

    asio::error_code ec;
    socket_.lowest_layer().close(ec);
}

void SearchHandler::decrypt(uint16_t length)
{
    DebugSocketsFmt("Decrypting packet from IP {} ({} bytes)", ipAddress_, length);

    DecryptSearchPacket(buffer_.data(), length, key, blowfish_);
}

void SearchHandler::encrypt(uint16_t length)
{
    DebugSocketsFmt("Encrypting packet for IP {} ({} bytes)", ipAddress_, length);

    EncryptSearchPacket(buffer_.data(), length, key, blowfish_);
}

bool SearchHandler::validatePacket(uint16_t length)
{
    DebugSocketsFmt("Validating packet from IP {} ({} bytes)", ipAddress_, length);

    return ValidateSearchPacketHash(buffer_.data(), length);
}

void SearchHandler::read_func(uint16_t length)
{
    if (!IsSearchPacketFrameLengthValid(length, ref<uint16>(buffer_.data(), 0x00)))
    {
        ShowErrorFmt("Search packetsize wrong. Size {} should be {}.", length, ref<uint16>(buffer_.data(), 0x00));
        return;
    }

    decrypt(length);

    if (validatePacket(length))
    {
        uint8 packetType = buffer_[0x0B];

        ShowInfoFmt("Search Request: {} ({}), size: {}, ip: {}", SearchRequestTypeToString(packetType), packetType, length, ipAddress_);

        switch (SearchHandlerDispatchForRequestType(packetType))
        {
            case SearchHandlerDispatch::SearchRequest:
            {
                HandleSearchRequest();
            }
            break;
            case SearchHandlerDispatch::SearchComment:
            {
                HandleSearchComment();
            }
            break;
            case SearchHandlerDispatch::GroupList:
            {
                HandleGroupListRequest();
            }
            break;
            case SearchHandlerDispatch::AuctionHouseRequest:
            {
                HandleAuctionHouseRequest();
            }
            break;
            case SearchHandlerDispatch::AuctionHouseHistory:
            {
                HandleAuctionHouseHistory();
            }
            break;
            case SearchHandlerDispatch::Unknown:
            {
                ShowErrorFmt("Unknown packet type: {}", packetType);
            }
        }
    }
}

// Mostly copy-pasted DSP era code. It works, so why change it?
/************************************************************************
 *                                                                       *
 *  Prints the contents of the packet in `data` to the console.          *
 *                                                                       *
 ************************************************************************/

void DebugPrintPacket(char* data, uint16_t size)
{
    if (!settings::get<bool>("logging.DEBUG_PACKETS"))
    {
        return;
    }

    std::string outStr = "\n";
    for (int32 y = 0; y < size; y++)
    {
        outStr += fmt::format("{:02X} ", (uint8)data[y]);
        if (((y + 1) % 16) == 0)
        {
            outStr += "\n";
        }
    }

    ShowDebug(outStr);
}

/************************************************************************
 *                                                                       *
 *  Character list request (party/linkshell)                             *
 *                                                                       *
 ************************************************************************/

void SearchHandler::HandleGroupListRequest()
{
    uint32 partyid      = ref<uint32>(buffer_.data(), 0x10);
    uint32 allianceid   = ref<uint32>(buffer_.data(), 0x14);
    uint32 linkshellid1 = ref<uint32>(buffer_.data(), 0x18);
    uint32 linkshellid2 = ref<uint32>(buffer_.data(), 0x1C);

    ShowInfoFmt("SEARCH::PartyID = {}", partyid);
    ShowInfoFmt("SEARCH::LinkshellIDs = {}, {}", linkshellid1, linkshellid2);

    CDataLoader PDataLoader;

    if (partyid != 0 || allianceid != 0)
    {
        std::list<SearchEntity*> PartyList = PDataLoader.GetPartyList(partyid, allianceid);

        CPartyListPacket PPartyPacket(partyid, (uint32)PartyList.size());

        for (auto& it : PartyList)
        {
            PPartyPacket.AddPlayer(it);
        }

        uint16_t length = PPartyPacket.GetSize();

        DebugPrintPacket((char*)PPartyPacket.GetData(), length);
        searchPackets_.emplace_back(PPartyPacket.GetData(), length);
    }
    else if (linkshellid1 != 0 || linkshellid2 != 0)
    {
        uint32                   linkshellid   = linkshellid1 == 0 ? linkshellid2 : linkshellid1;
        std::list<SearchEntity*> LinkshellList = PDataLoader.GetLinkshellList(linkshellid);

        uint32 totalResults  = (uint32)LinkshellList.size();
        uint32 currentResult = 0;

        // Iterate through the linkshell list, splitting up the results into
        // smaller chunks.
        std::list<SearchEntity*>::iterator it = LinkshellList.begin();

        do
        {
            CLinkshellListPacket PLinkshellPacket(linkshellid, totalResults);

            while (currentResult < totalResults)
            {
                bool success = PLinkshellPacket.AddPlayer(*it);
                if (!success)
                {
                    break;
                }

                currentResult++;
                ++it;
            }

            if (currentResult == totalResults)
            {
                PLinkshellPacket.SetFinal();
            }

            uint16_t length = PLinkshellPacket.GetSize();

            DebugPrintPacket((char*)PLinkshellPacket.GetData(), length);
            searchPackets_.emplace_back(PLinkshellPacket.GetData(), length);

        } while (currentResult < totalResults);
    }
}

void SearchHandler::HandleSearchComment()
{
    uint32 playerId = ref<uint32>(buffer_.data(), 0x10);

    CDataLoader PDataLoader;
    std::string comment = PDataLoader.GetSearchComment(playerId);
    if (comment.empty())
    {
        return;
    }

    SearchCommentPacket commentPacket(playerId, comment);

    uint16_t length = commentPacket.GetSize();

    DebugPrintPacket((char*)commentPacket.GetData(), length);
    searchPackets_.emplace_back(commentPacket.GetData(), length);
}

void SearchHandler::HandleSearchRequest()
{
    const search_req sr = _HandleSearchRequest();

    CDataLoader PDataLoader;
    int         totalCount = 0;

    std::list<SearchEntity*> SearchList = PDataLoader.GetPlayersList(sr, &totalCount);

    uint32 totalResults  = (uint32)SearchList.size();
    uint32 currentResult = 0;

    // Iterate through the search list, splitting up the results into
    // smaller chunks.
    std::list<SearchEntity*>::iterator it = SearchList.begin();

    do
    {
        CSearchListPacket PSearchPacket(totalCount);

        while (currentResult < totalResults)
        {
            bool success = PSearchPacket.AddPlayer(*it);
            if (!success)
            {
                break;
            }

            currentResult++;
            ++it;
        }

        if (currentResult == totalResults)
        {
            PSearchPacket.SetFinal();
        }

        uint16_t length = PSearchPacket.GetSize();

        DebugPrintPacket((char*)PSearchPacket.GetData(), length);
        searchPackets_.emplace_back(PSearchPacket.GetData(), length);

    } while (currentResult < totalResults);
}

void SearchHandler::HandleAuctionHouseRequest()
{
    uint8 AHCatID = ref<uint8>(buffer_.data(), 0x16);

    uint8       paramCount    = ref<uint8>(buffer_.data(), 0x12);
    std::string OrderByString = BuildAuctionHouseOrderByString(buffer_.data(), paramCount);
    const char* OrderByArray  = OrderByString.data();

    CDataLoader          PDataLoader;
    std::vector<ahItem*> ItemList = PDataLoader.GetAHItemsToCategory(AHCatID, OrderByArray);

    uint8 PacketsCount = (uint8)((ItemList.size() / 20) + (ItemList.size() % 20 != 0) + (ItemList.empty()));

    for (uint8 i = 0; i < PacketsCount; ++i)
    {
        CAHItemsListPacket PAHPacket(20 * i);
        uint16             itemListSize = static_cast<uint16>(ItemList.size());

        PAHPacket.SetItemCount(itemListSize);

        for (uint16 y = 20 * i; (y != 20 * (i + 1)) && (y < itemListSize); ++y)
        {
            PAHPacket.AddItem(ItemList.at(y));
        }

        uint16_t length = PAHPacket.GetSize();
        DebugPrintPacket((char*)PAHPacket.GetData(), length);

        searchPackets_.emplace_back(PAHPacket.GetData(), length);
    }
}

void SearchHandler::HandleAuctionHouseHistory()
{
    uint16 ItemID = ref<uint16>(buffer_.data(), 0x12);
    uint8  stack  = ref<uint8>(buffer_.data(), 0x15);

    CDataLoader             PDataLoader;
    std::vector<ahHistory*> HistoryList = PDataLoader.GetAHItemHistory(ItemID, stack != 0);
    ahItem                  item        = PDataLoader.GetAHItemFromItemID(ItemID);

    CAHHistoryPacket PAHPacket = CAHHistoryPacket(item, stack);

    for (auto& i : HistoryList)
    {
        PAHPacket.AddItem(i);
    }

    uint16_t length = PAHPacket.GetSize();

    DebugPrintPacket((char*)PAHPacket.GetData(), length);
    searchPackets_.emplace_back(PAHPacket.GetData(), length);
}

search_req SearchHandler::_HandleSearchRequest()
{
    return ParseSearchRequestPayload(buffer_.data() + 0x11, ref<uint8>(buffer_.data(), 0x10));
}

uint16_t SearchHandler::getNumSessionsInUse(const std::string& ipAddressStr)
{
    DebugSocketsFmt("Checking if IP is in use: {}", ipAddressStr);

    if (IPAddressWhitelist_.read(
            [ipAddressStr](const auto& ipWhitelist)
            {
                return ipWhitelist.find(ipAddressStr) != ipWhitelist.end();
            }))
    {
        return 0;
    }

    return IPAddressesInUse_.read(
        [ipAddressStr](const auto& ipAddrsInUse) -> uint16_t
        {
            return GetSearchSessionsInUse(ipAddrsInUse, {}, ipAddressStr);
        });
}

void SearchHandler::removeFromUsedIPAddresses(const std::string& ipAddressStr)
{
    DebugSocketsFmt("Removing IP from active set: {}", ipAddressStr);

    if (IPAddressWhitelist_.read(
            [ipAddressStr](const auto& ipWhitelist)
            {
                return ipWhitelist.find(ipAddressStr) != ipWhitelist.end();
            }))
    {
        return;
    }

    IPAddressesInUse_.write(
        [ipAddressStr](auto& ipAddrsInUse)
        {
            RemoveSearchSession(ipAddrsInUse, {}, ipAddressStr);
        });
}

void SearchHandler::addToUsedIPAddresses(const std::string& ipAddressStr)
{
    DebugSocketsFmt("Adding IP to active set: {}", ipAddressStr);

    if (IPAddressWhitelist_.read(
            [ipAddressStr](const auto& ipWhitelist)
            {
                return ipWhitelist.find(ipAddressStr) != ipWhitelist.end();
            }))
    {
        return;
    }

    IPAddressesInUse_.write(
        [ipAddressStr](auto& ipAddrsInUse)
        {
            AddSearchSession(ipAddrsInUse, {}, ipAddressStr);
        });
}
