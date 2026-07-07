/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

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

#include "test_search_packet_buffer.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <unordered_set>

#include "common/blowfish.h"
#include "common/logging.h"
#include "common/md52.h"
#include "common/mmo.h"
#include "common/types/maybe.h"
#include "search/data_loader.h"
#include "search/search_application_config.h"
#include "search/search_engine_config.h"
#include "search/search_packet_crypto.h"
#include "search/search_packet_hash.h"
#include "search/search_player_filter.h"
#include "search/search_player_query_filter.h"
#include "search/search_player_state.h"
#include "search/search.h"
#include "search/search_request_type.h"
#include "search/search_session_tracker.h"

namespace
{

auto expectEqualInt(std::uint64_t actual, std::uint64_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "search packet buffer self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "search packet buffer self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(const std::string& actual, const std::string& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "search packet buffer self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto defaultSearchKey() -> std::array<std::uint8_t, 24>
{
    return {
        0x30,
        0x73,
        0x3D,
        0x6D,
        0x3C,
        0x31,
        0x49,
        0x5A,
        0x32,
        0x7A,
        0x42,
        0x43,
        0x63,
        0x38,
        0x7B,
        0x7E,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
    };
}

auto testRequestTypeConstants() -> bool
{
    bool ok = true;
    ok      = expectEqualInt(TCP_SEARCH_ALL, 0x00, "TCP_SEARCH_ALL value") && ok;
    ok      = expectEqualInt(TCP_GROUP_LIST, 0x02, "TCP_GROUP_LIST value") && ok;
    ok      = expectEqualInt(TCP_SEARCH, 0x03, "TCP_SEARCH value") && ok;
    ok      = expectEqualInt(TCP_AH_HISTORY_SINGLE, 0x05, "TCP_AH_HISTORY_SINGLE value") && ok;
    ok      = expectEqualInt(TCP_AH_HISTORY_STACK, 0x06, "TCP_AH_HISTORY_STACK value") && ok;
    ok      = expectEqualInt(TCP_SEARCH_COMMENT, 0x08, "TCP_SEARCH_COMMENT value") && ok;
    ok      = expectEqualInt(TCP_AH_REQUEST_MORE, 0x10, "TCP_AH_REQUEST_MORE value") && ok;
    ok      = expectEqualInt(TCP_AH_REQUEST, 0x15, "TCP_AH_REQUEST value") && ok;
    return ok;
}

auto testSearchApplicationServerName() -> bool
{
    return expectEqualString(SearchApplicationServerName(), "search", "search application server name");
}

auto testSearchApplicationConsoleCommandDescriptors() -> bool
{
    const auto commands = SearchApplicationConsoleCommandDescriptors(14);

    bool ok = true;
    ok      = expectEqualInt(commands.size(), 2, "search application command count") && ok;
    ok      = expectEqualString(commands[0].name, "ah_cleanup", "search ah cleanup command name") && ok;
    ok      = expectEqualString(commands[0].description, "AH task to return items older than 14 days", "search ah cleanup command description") && ok;
    ok      = expectEqualString(commands[1].name, "expire_all", "search expire all command name") && ok;
    ok      = expectEqualString(commands[1].description, "Force-expire all items on the AH, returning to sender", "search expire all command description") && ok;
    return ok;
}

auto testSearchAuctionExpirationPlanDisabled() -> bool
{
    const auto plan = BuildSearchAuctionExpirationPlan({
        .enabled         = false,
        .intervalSeconds = 30,
    });

    bool ok = true;
    ok      = expectTrue(!plan.schedulePeriodicCleanup, "disabled search auction expiration schedule") && ok;
    ok      = expectEqualInt(plan.intervalSeconds, 0, "disabled search auction expiration interval") && ok;
    return ok;
}

auto testSearchAuctionExpirationPlanEnabled() -> bool
{
    const auto plan = BuildSearchAuctionExpirationPlan({
        .enabled         = true,
        .intervalSeconds = 900,
    });

    bool ok = true;
    ok      = expectTrue(plan.schedulePeriodicCleanup, "enabled search auction expiration schedule") && ok;
    ok      = expectEqualInt(plan.intervalSeconds, 900, "enabled search auction expiration interval") && ok;
    return ok;
}

auto testSearchAuctionInitializationPlanDisabled() -> bool
{
    const auto plan = BuildSearchAuctionInitializationPlan({
        .enabled        = false,
        .expirationDays = 14,
    });

    bool ok = true;
    ok      = expectTrue(!plan.runInitialCleanup, "disabled search auction initial cleanup") && ok;
    ok      = expectEqualInt(plan.expirationDays, 0, "disabled search auction initial cleanup days") && ok;
    return ok;
}

auto testSearchAuctionInitializationPlanEnabled() -> bool
{
    const auto plan = BuildSearchAuctionInitializationPlan({
        .enabled        = true,
        .expirationDays = 21,
    });

    bool ok = true;
    ok      = expectTrue(plan.runInitialCleanup, "enabled search auction initial cleanup") && ok;
    ok      = expectEqualInt(plan.expirationDays, 21, "enabled search auction initial cleanup days") && ok;
    return ok;
}

auto testSearchAuctionExpirationDaysUsesOptionalFallback() -> bool
{
    bool ok = true;
    ok      = expectEqualInt(SearchAuctionExpirationDays(Maybe<uint16>(21)), 21, "search auction expiration explicit days") && ok;
    ok      = expectEqualInt(SearchAuctionExpirationDays(std::nullopt), 0, "search auction expiration all-days fallback") && ok;
    return ok;
}

auto testRequestTypeStrings() -> bool
{
    bool ok = true;
    ok      = expectEqualString(SearchRequestTypeToString(TCP_SEARCH_ALL), "SEARCH_ALL", "TCP_SEARCH_ALL string") && ok;
    ok      = expectEqualString(SearchRequestTypeToString(TCP_GROUP_LIST), "GROUP_LIST", "TCP_GROUP_LIST string") && ok;
    ok      = expectEqualString(SearchRequestTypeToString(TCP_SEARCH), "SEARCH", "TCP_SEARCH string") && ok;
    ok      = expectEqualString(SearchRequestTypeToString(TCP_AH_HISTORY_SINGLE), "AH_HISTORY_SINGLE", "TCP_AH_HISTORY_SINGLE string") && ok;
    ok      = expectEqualString(SearchRequestTypeToString(TCP_AH_HISTORY_STACK), "AH_HISTORY_STACK", "TCP_AH_HISTORY_STACK string") && ok;
    ok      = expectEqualString(SearchRequestTypeToString(TCP_SEARCH_COMMENT), "SEARCH_COMMENT", "TCP_SEARCH_COMMENT string") && ok;
    ok      = expectEqualString(SearchRequestTypeToString(TCP_AH_REQUEST_MORE), "AH_REQUEST_MORE", "TCP_AH_REQUEST_MORE string") && ok;
    ok      = expectEqualString(SearchRequestTypeToString(TCP_AH_REQUEST), "AH_REQUEST", "TCP_AH_REQUEST string") && ok;
    ok      = expectEqualString(SearchRequestTypeToString(0xFF), "UNKNOWN", "unknown request type string") && ok;
    return ok;
}

void writeSearchPacketHash(std::uint8_t* packet, const std::uint16_t length)
{
    std::uint8_t digest[16]{};
    md5(packet + 8, digest, length - 28);
    std::memcpy(packet + length - 0x14, digest, sizeof(digest));
}

void encipherSearchBlocks(std::uint8_t* packet, const std::uint16_t length, const std::uint8_t* key, const std::int32_t keyLength, const bool truncateBlockCount)
{
    auto blowfish = blowfish_t{};
    md5(const_cast<std::uint8_t*>(key), blowfish.hash, keyLength);
    blowfish_init(reinterpret_cast<int8*>(blowfish.hash), 16, blowfish.P, blowfish.S[0]);

    auto tmp = static_cast<std::uint16_t>((length - 12) / 4);
    if (truncateBlockCount)
    {
        tmp = static_cast<std::uint8_t>(tmp);
    }
    tmp -= tmp % 2;

    for (std::uint16_t i = 0; i < tmp; i += 2)
    {
        blowfish_encipher(reinterpret_cast<std::uint32_t*>(packet) + i + 2, reinterpret_cast<std::uint32_t*>(packet) + i + 3, blowfish.P, blowfish.S[0]);
    }
}

auto testPacketHashValidationAcceptsMatchingDigest() -> bool
{
    auto input = std::array<std::uint8_t, 64>{};
    for (std::size_t i = 8; i < input.size() - 0x14; ++i)
    {
        input[i] = static_cast<std::uint8_t>((i * 19U) + 3U);
    }

    writeSearchPacketHash(input.data(), input.size());

    return expectTrue(ValidateSearchPacketHash(input.data(), input.size()), "matching packet hash accepted");
}

auto testPacketHashValidationRejectsDigestMismatch() -> bool
{
    auto input = std::array<std::uint8_t, 64>{};
    for (std::size_t i = 8; i < input.size() - 0x14; ++i)
    {
        input[i] = static_cast<std::uint8_t>((i * 23U) + 5U);
    }

    writeSearchPacketHash(input.data(), input.size());
    input[input.size() - 0x14 + 7] ^= 0x80;

    return expectTrue(!ValidateSearchPacketHash(input.data(), input.size()), "mismatched packet hash rejected");
}

auto testPacketHashValidationIgnoresTrailingKeyBytes() -> bool
{
    auto input = std::array<std::uint8_t, 64>{};
    for (std::size_t i = 8; i < input.size() - 0x14; ++i)
    {
        input[i] = static_cast<std::uint8_t>((i * 29U) + 11U);
    }

    writeSearchPacketHash(input.data(), input.size());
    input[input.size() - 4] = 0xAA;
    input[input.size() - 3] = 0xBB;
    input[input.size() - 2] = 0xCC;
    input[input.size() - 1] = 0xDD;

    return expectTrue(ValidateSearchPacketHash(input.data(), input.size()), "trailing key bytes ignored by hash");
}

auto testPacketHashValidationAcceptsMinimumFrame() -> bool
{
    auto input = std::array<std::uint8_t, 28>{};
    writeSearchPacketHash(input.data(), input.size());

    return expectTrue(ValidateSearchPacketHash(input.data(), input.size()), "minimum packet hash frame accepted");
}

auto testSearchPacketEncryptMatchesManualFrame() -> bool
{
    auto input = std::array<std::uint8_t, 64>{};
    for (std::size_t i = 8; i < input.size() - 0x14; ++i)
    {
        input[i] = static_cast<std::uint8_t>((i * 37U) + 13U);
    }
    input[input.size() - 0x18] = 0xAB;
    input[input.size() - 0x17] = 0xCD;
    input[input.size() - 0x16] = 0xEF;
    input[input.size() - 0x15] = 0x12;

    auto expected = input;
    auto key = defaultSearchKey();
    expected[0] = static_cast<std::uint8_t>(expected.size());
    expected[1] = 0x00;
    std::memcpy(expected.data() + 4, "IXFF", 4);
    writeSearchPacketHash(expected.data(), expected.size());
    encipherSearchBlocks(expected.data(), expected.size(), key.data(), 24, true);
    std::memcpy(expected.data() + expected.size() - 4, key.data() + 16, 4);

    auto actual = input;
    auto encryptBlowfish = blowfish_t{};
    EncryptSearchPacket(actual.data(), actual.size(), key.data(), encryptBlowfish);

    bool ok = true;
    ok      = expectTrue(std::memcmp(actual.data(), expected.data(), actual.size()) == 0, "encrypted packet matches manual frame") && ok;
    ok      = expectTrue(std::memcmp(actual.data() + 8, input.data() + 8, 8) != 0, "encrypted payload changed") && ok;
    ok      = expectTrue(std::memcmp(actual.data() + actual.size() - 4, defaultSearchKey().data() + 16, 4) == 0, "encrypted packet trailing key") && ok;
    return ok;
}

auto testSearchPacketEncryptUsesUint8BlockCount() -> bool
{
    auto input = std::array<std::uint8_t, 1100>{};
    for (std::size_t i = 8; i < input.size() - 0x14; ++i)
    {
        input[i] = static_cast<std::uint8_t>((i * 43U) + 19U);
    }

    auto expected = input;
    auto key = defaultSearchKey();
    expected[0] = static_cast<std::uint8_t>(expected.size());
    expected[1] = static_cast<std::uint8_t>(expected.size() >> 8);
    std::memcpy(expected.data() + 4, "IXFF", 4);
    writeSearchPacketHash(expected.data(), expected.size());
    encipherSearchBlocks(expected.data(), expected.size(), key.data(), 24, true);
    std::memcpy(expected.data() + expected.size() - 4, key.data() + 16, 4);

    auto actual = input;
    auto encryptBlowfish = blowfish_t{};
    EncryptSearchPacket(actual.data(), actual.size(), key.data(), encryptBlowfish);

    bool ok = true;
    ok      = expectTrue(std::memcmp(actual.data(), expected.data(), actual.size()) == 0, "large encrypted packet matches uint8 block count") && ok;
    ok      = expectTrue(std::memcmp(actual.data() + 72, input.data() + 72, 8) == 0, "large encrypted packet leaves overflow blocks unchanged") && ok;
    return ok;
}

auto testSearchPacketDecryptMatchesManualFrame() -> bool
{
    auto plain = std::array<std::uint8_t, 64>{};
    for (std::size_t i = 8; i < plain.size(); ++i)
    {
        plain[i] = static_cast<std::uint8_t>((i * 41U) + 17U);
    }
    plain[plain.size() - 0x18] = 0x24;
    plain[plain.size() - 0x17] = 0x68;
    plain[plain.size() - 0x16] = 0xAC;
    plain[plain.size() - 0x15] = 0xE0;
    plain[plain.size() - 4] = 0x13;
    plain[plain.size() - 3] = 0x57;
    plain[plain.size() - 2] = 0x9B;
    plain[plain.size() - 1] = 0xDF;

    auto decryptKey = defaultSearchKey();
    std::memcpy(decryptKey.data() + 16, plain.data() + plain.size() - 4, 4);
    auto encrypted = plain;
    encipherSearchBlocks(encrypted.data(), encrypted.size(), decryptKey.data(), 20, false);

    auto actual = encrypted;
    auto actualKey = defaultSearchKey();
    auto decryptBlowfish = blowfish_t{};
    DecryptSearchPacket(actual.data(), actual.size(), actualKey.data(), decryptBlowfish);

    bool ok = true;
    ok      = expectTrue(std::memcmp(actual.data(), plain.data(), actual.size()) == 0, "decrypted packet matches manual frame") && ok;
    ok      = expectTrue(std::memcmp(actualKey.data() + 16, plain.data() + plain.size() - 4, 4) == 0, "decrypt key reads trailing packet key") && ok;
    ok      = expectTrue(std::memcmp(actualKey.data() + 20, plain.data() + plain.size() - 0x18, 4) == 0, "decrypt key reads decrypted packet key") && ok;
    return ok;
}

auto testSearchSessionTrackerCountsActiveIP() -> bool
{
    auto sessions = std::map<std::string, uint16>{};
    auto whitelist = std::unordered_set<std::string>{};

    AddSearchSession(sessions, whitelist, "198.51.100.7");
    AddSearchSession(sessions, whitelist, "198.51.100.7");

    bool ok = true;
    ok      = expectEqualInt(GetSearchSessionsInUse(sessions, whitelist, "198.51.100.7"), 2, "active search sessions count") && ok;
    ok      = expectEqualInt(GetSearchSessionsInUse(sessions, whitelist, "203.0.113.9"), 0, "missing search sessions count") && ok;
    return ok;
}

auto testSearchSessionTrackerRemovesAndErasesIP() -> bool
{
    auto sessions = std::map<std::string, uint16>{ { "198.51.100.7", 2 } };
    auto whitelist = std::unordered_set<std::string>{};

    RemoveSearchSession(sessions, whitelist, "203.0.113.9");
    RemoveSearchSession(sessions, whitelist, "198.51.100.7");

    bool ok = true;
    ok      = expectEqualInt(GetSearchSessionsInUse(sessions, whitelist, "198.51.100.7"), 1, "decremented search sessions count") && ok;
    ok      = expectEqualInt(sessions.size(), 1, "decremented search sessions retained") && ok;

    RemoveSearchSession(sessions, whitelist, "198.51.100.7");
    ok = expectEqualInt(GetSearchSessionsInUse(sessions, whitelist, "198.51.100.7"), 0, "removed search sessions count") && ok;
    ok = expectEqualInt(sessions.size(), 0, "removed search sessions erased") && ok;
    return ok;
}

auto testSearchSessionTrackerIgnoresWhitelistedIP() -> bool
{
    auto sessions = std::map<std::string, uint16>{ { "198.51.100.7", 3 } };
    auto whitelist = std::unordered_set<std::string>{ "198.51.100.7" };

    AddSearchSession(sessions, whitelist, "198.51.100.7");
    RemoveSearchSession(sessions, whitelist, "198.51.100.7");

    bool ok = true;
    ok      = expectEqualInt(GetSearchSessionsInUse(sessions, whitelist, "198.51.100.7"), 0, "whitelisted search sessions count") && ok;
    ok      = expectEqualInt(sessions.at("198.51.100.7"), 3, "whitelisted stored count unchanged") && ok;
    return ok;
}

auto defaultSearchRequest() -> search_req
{
    auto request = search_req{};
    request.race = 255;
    request.nation = 255;
    return request;
}

auto defaultSearchEntity() -> SearchEntity
{
    auto player = SearchEntity{};
    player.name = "Alpha";
    player.id = 0x101;
    player.mjob = 7;
    player.mlvl = 75;
    player.sjob = 3;
    player.slvl = 37;
    player.nation = 1;
    player.rank = 6;
    player.race = 5;
    player.flags1 = 0x0100;
    player.flags2 = 0x0100;
    player.linkshellid1 = 0x1234;
    player.linkshellid2 = 0x5678;
    player.unityLeader = 9;
    return player;
}

auto testSearchPlayerFilterAcceptsDefaultRequest() -> bool
{
    const auto request = defaultSearchRequest();
    const auto player = defaultSearchEntity();

    return expectTrue(SearchPlayerMatchesRequest(player, request), "default search request accepts visible player");
}

auto testSearchPlayerFilterLinkshellAndAnonRules() -> bool
{
    bool ok = true;

    auto player = defaultSearchEntity();
    auto request = defaultSearchRequest();
    request.lsId = Maybe<uint32>(0);
    ok = expectTrue(!SearchPlayerMatchesRequest(player, request), "zero linkshell request rejects player") && ok;

    request = defaultSearchRequest();
    request.lsId = Maybe<uint32>(0x5678);
    ok = expectTrue(SearchPlayerMatchesRequest(player, request), "matching secondary linkshell accepts player") && ok;

    request.lsId = Maybe<uint32>(0x9999);
    ok = expectTrue(!SearchPlayerMatchesRequest(player, request), "nonmatching linkshell rejects player") && ok;

    player = defaultSearchEntity();
    player.flags1 |= 0x4000;
    request = defaultSearchRequest();
    ok = expectTrue(SearchPlayerMatchesRequest(player, request), "anon player survives broad search") && ok;

    request.jobid = player.mjob;
    ok = expectTrue(!SearchPlayerMatchesRequest(player, request), "anon player rejects private-field search") && ok;

    return ok;
}

auto testSearchPlayerFilterRaceRankLevelNameAndHiddenRules() -> bool
{
    bool ok = true;

    auto player = defaultSearchEntity();
    auto request = defaultSearchRequest();
    request.race = 2; // tarutaru male/female
    ok = expectTrue(SearchPlayerMatchesRequest(player, request), "tarutaru grouped race accepts race 5") && ok;

    request.race = 3; // mithra only
    ok = expectTrue(!SearchPlayerMatchesRequest(player, request), "mithra grouped race rejects race 5") && ok;

    request = defaultSearchRequest();
    request.minRank = 4;
    request.maxRank = 6;
    ok = expectTrue(SearchPlayerMatchesRequest(player, request), "rank range accepts boundary player") && ok;

    request.minRank = 7;
    request.maxRank = 6;
    ok = expectTrue(SearchPlayerMatchesRequest(player, request), "inverted rank range ignored") && ok;

    request = defaultSearchRequest();
    request.minlvl = 76;
    request.maxlvl = 99;
    ok = expectTrue(!SearchPlayerMatchesRequest(player, request), "level range rejects lower player") && ok;

    request.minlvl = 80;
    request.maxlvl = 79;
    ok = expectTrue(SearchPlayerMatchesRequest(player, request), "inverted level range ignored") && ok;

    request = defaultSearchRequest();
    request.name = "alP";
    request.nameLen = 3;
    ok = expectTrue(SearchPlayerMatchesRequest(player, request), "case-insensitive name prefix accepts player") && ok;

    request.name = "alphabet";
    request.nameLen = 8;
    ok = expectTrue(!SearchPlayerMatchesRequest(player, request), "overlong name prefix rejects player") && ok;

    request = defaultSearchRequest();
    player.gmHidden = true;
    ok = expectTrue(!SearchPlayerMatchesRequest(player, request), "GM-hidden player rejects broad search") && ok;

    return ok;
}

auto testSearchPlayerFilterFlagsAndUnityRules() -> bool
{
    bool ok = true;
    auto player = defaultSearchEntity();
    auto request = defaultSearchRequest();

    request.flags = 0x0100;
    ok = expectTrue(SearchPlayerMatchesRequest(player, request), "normal flag match accepts player") && ok;

    request.flags = 0x0200;
    ok = expectTrue(!SearchPlayerMatchesRequest(player, request), "normal flag miss rejects player") && ok;

    request.flags = 9U << 22;
    ok = expectTrue(SearchPlayerMatchesRequest(player, request), "unity flag match accepts player") && ok;

    request.flags = (9U << 22) | 0x0200;
    ok = expectTrue(SearchPlayerMatchesRequest(player, request), "unity flag match ignores lower flag miss") && ok;

    request.flags = 8U << 22;
    ok = expectTrue(!SearchPlayerMatchesRequest(player, request), "unity flag miss rejects player") && ok;

    return ok;
}

auto testSearchPlayerQueryFilterBuildsRepresentativeFragment() -> bool
{
    auto request = defaultSearchRequest();
    request.jobid = 7;
    request.zoneid[0] = 230;
    request.zoneid[1] = 231;
    request.zoneid[2] = 0;
    request.commentType = 0xA0;

    return expectEqualString(
        BuildSearchPlayerQueryFilter(request),
        " AND  mjob = 7 AND (pos_zone IN (230, 231) OR (pos_zone = 0 AND pos_prevzone IN (230, 231)))  AND (seacom_type & 0xF0) = 160",
        "representative player query filter");
}

auto testSearchPlayerCountQueryUsesJobFilterForValidJobs() -> bool
{
    auto request  = defaultSearchRequest();
    request.jobid = 7;
    const auto query = BuildSearchPlayerCountQuery(request);

    bool ok = true;
    ok      = expectEqualString(query.sql, "SELECT COUNT(*) FROM accounts_sessions LEFT JOIN char_stats USING (charid) WHERE mjob = ?", "valid job count query") && ok;
    ok      = expectTrue(query.filtersJob, "valid job count query filters job") && ok;
    ok      = expectEqualInt(query.jobID, 7, "valid job count query job id") && ok;

    request.jobid = 20;
    const auto highestJobQuery = BuildSearchPlayerCountQuery(request);
    ok                        = expectTrue(highestJobQuery.filtersJob, "highest valid job count query filters job") && ok;
    ok                        = expectEqualInt(highestJobQuery.jobID, 20, "highest valid job count query job id") && ok;

    return ok;
}

auto testSearchPlayerCountQueryCountsAllSessionsForInvalidJobs() -> bool
{
    auto request = defaultSearchRequest();

    bool ok = true;
    for (const auto jobID : { static_cast<uint8>(0), static_cast<uint8>(21), static_cast<uint8>(255) })
    {
        request.jobid    = jobID;
        const auto query = BuildSearchPlayerCountQuery(request);
        ok               = expectEqualString(query.sql, "SELECT COUNT(*) FROM accounts_sessions", "invalid job count query") && ok;
        ok               = expectTrue(!query.filtersJob, "invalid job count query does not filter job") && ok;
        ok               = expectEqualInt(query.jobID, 0, "invalid job count query clears job id") && ok;
    }

    return ok;
}

auto testSearchPlayerListQueryBuildsBaseQuery() -> bool
{
    return expectEqualString(
        BuildSearchPlayerListQuery(""),
        "SELECT charid, partyid, charname, pos_zone, pos_prevzone, nation, rank_sandoria, rank_bastok, unity_leader, "
        "rank_windurst, race, mjob, sjob, mlvl, slvl, languages, settings, seacom_type, disconnecting, gmHiddenEnabled, muted, "
        "linkshellid1, linkshellid2 "
        "FROM accounts_sessions "
        "LEFT JOIN accounts_parties USING (charid) "
        "LEFT JOIN chars USING (charid) "
        "LEFT JOIN char_look USING (charid) "
        "LEFT JOIN char_stats USING (charid) "
        "LEFT JOIN char_profile USING(charid) "
        "LEFT JOIN char_flags USING(charid) "
        "WHERE charname IS NOT NULL "
        " ORDER BY charname ASC",
        "player list base query");
}

auto testSearchPlayerListQueryAppendsFilterBeforeOrder() -> bool
{
    return expectEqualString(
        BuildSearchPlayerListQuery(" AND  mjob = 7"),
        "SELECT charid, partyid, charname, pos_zone, pos_prevzone, nation, rank_sandoria, rank_bastok, unity_leader, "
        "rank_windurst, race, mjob, sjob, mlvl, slvl, languages, settings, seacom_type, disconnecting, gmHiddenEnabled, muted, "
        "linkshellid1, linkshellid2 "
        "FROM accounts_sessions "
        "LEFT JOIN accounts_parties USING (charid) "
        "LEFT JOIN chars USING (charid) "
        "LEFT JOIN char_look USING (charid) "
        "LEFT JOIN char_stats USING (charid) "
        "LEFT JOIN char_profile USING(charid) "
        "LEFT JOIN char_flags USING(charid) "
        "WHERE charname IS NOT NULL "
        " AND  mjob = 7"
        " ORDER BY charname ASC",
        "player list filtered query");
}

auto testSearchPartyListQueryBuildsSQLAndPartyParams() -> bool
{
    const auto query = BuildSearchPartyListQuery(123, 0);

    bool ok = true;
    ok      = expectEqualString(
             query.sql,
             "SELECT charid, partyid, charname, pos_zone, nation, rank_sandoria, rank_bastok, rank_windurst, race, settings, mjob, sjob, mlvl, slvl, languages, seacom_type, disconnecting "
             "FROM accounts_sessions "
             "LEFT JOIN accounts_parties USING(charid) "
             "LEFT JOIN chars USING(charid) "
             "LEFT JOIN char_look USING(charid) "
             "LEFT JOIN char_stats USING(charid) "
             "LEFT JOIN char_profile USING(charid) "
             "LEFT JOIN char_flags USING(charid) "
             "WHERE IF (allianceid <> 0, allianceid IN (SELECT allianceid FROM accounts_parties WHERE charid = ?) , partyid = ?) "
             "ORDER BY charname ASC "
             "LIMIT 64",
             "party list query") &&
         ok;
    ok = expectEqualInt(query.firstParam, 123, "party list party first param") && ok;
    ok = expectEqualInt(query.secondParam, 123, "party list party second param") && ok;
    return ok;
}

auto testSearchPartyListQueryChoosesAllianceAndFallbackParams() -> bool
{
    const auto partyWithAlliance = BuildSearchPartyListQuery(123, 456);
    const auto allianceOnly      = BuildSearchPartyListQuery(0, 456);

    bool ok = true;
    ok      = expectEqualInt(partyWithAlliance.firstParam, 456, "party list alliance first param") && ok;
    ok      = expectEqualInt(partyWithAlliance.secondParam, 123, "party list alliance second param") && ok;
    ok      = expectEqualInt(allianceOnly.firstParam, 456, "party list alliance-only first param") && ok;
    ok      = expectEqualInt(allianceOnly.secondParam, 456, "party list alliance-only second param") && ok;
    return ok;
}

auto testSearchLinkshellListQueryBuildsSQLAndParams() -> bool
{
    const auto query = BuildSearchLinkshellListQuery(789);

    bool ok = true;
    ok      = expectEqualString(
             query.sql,
             "SELECT charid, partyid, charname, pos_zone, nation, rank_sandoria, rank_bastok, rank_windurst, race, settings, mjob, sjob, "
             "mlvl, slvl, linkshellid1, linkshellid2, "
             "linkshellrank1, linkshellrank2, disconnecting "
             "FROM accounts_sessions "
             "LEFT JOIN accounts_parties USING (charid) "
             "LEFT JOIN chars USING (charid) "
             "LEFT JOIN char_look USING (charid) "
             "LEFT JOIN char_stats USING (charid) "
             "LEFT JOIN char_profile USING(charid) "
             "LEFT JOIN char_flags USING(charid) "
             "WHERE linkshellid1 = ? OR linkshellid2 = ? "
             "ORDER BY charname ASC "
             "LIMIT 64",
             "linkshell list query") &&
         ok;
    ok = expectEqualInt(query.firstParam, 789, "linkshell list first param") && ok;
    ok = expectEqualInt(query.secondParam, 789, "linkshell list second param") && ok;
    return ok;
}

auto testSearchCommentQueryBuildsSQLAndParam() -> bool
{
    const auto query = BuildSearchCommentQuery(0x01020304);

    bool ok = true;
    ok      = expectEqualString(
             query.sql,
             "SELECT seacom_message FROM accounts_sessions WHERE charid = ?",
             "search comment query") &&
         ok;
    ok = expectEqualInt(query.playerID, 0x01020304, "search comment player id") && ok;
    return ok;
}

auto testSearchPlayerQueryFilterIgnoresInvalidOrAbsentInputs() -> bool
{
    bool ok = true;

    auto request = defaultSearchRequest();
    ok = expectEqualString(BuildSearchPlayerQueryFilter(request), "", "empty player query filter") && ok;

    request.jobid = 21;
    request.zoneid[0] = 0;
    request.commentType = 0;
    ok = expectEqualString(BuildSearchPlayerQueryFilter(request), "", "invalid job player query filter") && ok;

    request.jobid = 20;
    ok = expectEqualString(BuildSearchPlayerQueryFilter(request), " AND  mjob = 20", "highest valid job player query filter") && ok;

    return ok;
}

auto testSearchPlayerQueryFilterCapsZoneListAtTenAndStopsAtZero() -> bool
{
    auto request = defaultSearchRequest();
    for (std::size_t i = 0; i < 15; ++i)
    {
        request.zoneid[i] = static_cast<uint16>(200 + i);
    }

    bool ok = true;
    ok = expectEqualString(
             BuildSearchPlayerQueryFilter(request),
             " AND (pos_zone IN (200, 201, 202, 203, 204, 205, 206, 207, 208, 209) OR (pos_zone = 0 AND pos_prevzone IN (200, 201, 202, 203, 204, 205, 206, 207, 208, 209))) ",
             "ten-zone player query filter") &&
         ok;

    request = defaultSearchRequest();
    request.zoneid[0] = 230;
    request.zoneid[1] = 0;
    request.zoneid[2] = 231;
    ok = expectEqualString(
             BuildSearchPlayerQueryFilter(request),
             " AND (pos_zone IN (230) OR (pos_zone = 0 AND pos_prevzone IN (230))) ",
             "zero-terminated player query filter") &&
         ok;

    return ok;
}

auto testSearchPlayerStateNormalizesFlagsAndZone() -> bool
{
    auto player = defaultSearchEntity();
    player.zone = 0;
    player.prevzone = 245;
    player.seacom_type = 0x40;
    player.disconnecting = true;
    player.muted = true;
    player.flags1 = 0x0004;

    auto settings = SAVE_CONF{};
    settings.MentorFlg = 1;
    settings.AwayFlg = 1;
    settings.AnonymityFlg = 1;
    settings.InviteFlg = 1;
    const auto settingsBytes = settings;
    uint32 settingsInt = 0;
    std::memcpy(&settingsInt, &settingsBytes, sizeof(uint32));

    NormalizeSearchPlayerForList(player, settingsInt, player.id);

    bool ok = true;
    ok = expectEqualInt(player.zone, 245, "normalized previous zone") && ok;
    ok = expectTrue(player.mentor, "normalized mentor flag") && ok;
    ok = expectEqualInt(player.flags1, 0x2000E91D, "normalized search flags1") && ok;
    ok = expectEqualInt(player.flags2, player.flags1, "normalized search flags2 mirror") && ok;
    return ok;
}

auto testSearchPlayerStateSetsPartyMemberWithoutLeader() -> bool
{
    auto player = defaultSearchEntity();

    NormalizeSearchPlayerForList(player, 0, player.id + 1);

    bool ok = true;
    ok = expectEqualInt(player.flags1, 0x2100, "party member flag without leader flag") && ok;
    ok = expectEqualInt(player.flags2, player.flags1, "party member flags2 mirror") && ok;
    return ok;
}

auto testSearchPlayerStateLeavesCurrentZoneAndClearsMonstrosityJobs() -> bool
{
    auto player = defaultSearchEntity();
    player.zone = 230;
    player.prevzone = 245;
    player.mjob = 23;
    player.sjob = 3;

    NormalizeSearchPlayerForList(player, 0, 0);

    bool ok = true;
    ok = expectEqualInt(player.zone, 230, "current zone retained") && ok;
    ok = expectEqualInt(player.mjob, 0, "monstrosity main job cleared") && ok;
    ok = expectEqualInt(player.sjob, 0, "monstrosity sub job cleared") && ok;

    player = defaultSearchEntity();
    player.mjob = 7;
    player.sjob = 23;
    NormalizeSearchPlayerForList(player, 0, 0);
    ok = expectEqualInt(player.mjob, 0, "monstrosity sub clears main job") && ok;
    ok = expectEqualInt(player.sjob, 0, "monstrosity sub job cleared") && ok;
    return ok;
}

auto testSearchRankForNationSelectsNationRank() -> bool
{
    bool ok = true;
    ok      = expectEqualInt(SearchRankForNation(0, 3, 5, 7), 3, "san d'oria rank selected") && ok;
    ok      = expectEqualInt(SearchRankForNation(1, 3, 5, 7), 5, "bastok rank selected") && ok;
    ok      = expectEqualInt(SearchRankForNation(2, 3, 5, 7), 7, "windurst rank selected") && ok;
    return ok;
}

auto testSearchRankForNationDefaultsInvalidNationToZero() -> bool
{
    return expectEqualInt(SearchRankForNation(3, 3, 5, 7), 0, "invalid nation rank");
}

auto testSearchPartyMemberStateNormalizesPartyFlagsOnly() -> bool
{
    auto player = defaultSearchEntity();
    player.zone = 0;
    player.prevzone = 245;
    player.mjob = 23;
    player.seacom_type = 0x40;
    player.disconnecting = true;

    auto settings = SAVE_CONF{};
    settings.MentorFlg = 1;
    settings.AwayFlg = 1;
    settings.AnonymityFlg = 1;
    settings.InviteFlg = 1;
    const auto settingsBytes = settings;
    uint32 settingsInt = 0;
    std::memcpy(&settingsInt, &settingsBytes, sizeof(uint32));

    NormalizeSearchPartyMemberForList(player, settingsInt, player.id);

    bool ok = true;
    ok = expectEqualInt(player.zone, 0, "party member zone left unchanged") && ok;
    ok = expectEqualInt(player.mjob, 23, "party member monstrosity job left unchanged") && ok;
    ok = expectTrue(player.mentor, "party member mentor flag") && ok;
    ok = expectEqualInt(player.flags1, 0xE919, "party member flags1") && ok;
    ok = expectEqualInt(player.flags2, player.flags1, "party member flags2 mirror") && ok;
    return ok;
}

auto testSearchPartyMemberStateSetsMemberWithoutLeader() -> bool
{
    auto player = defaultSearchEntity();

    NormalizeSearchPartyMemberForList(player, 0, player.id + 1);

    bool ok = true;
    ok = expectEqualInt(player.flags1, 0x2100, "party member flag without leader flag") && ok;
    ok = expectEqualInt(player.flags2, player.flags1, "party member flags2 mirror") && ok;
    return ok;
}

auto testSearchLinkshellMemberStateNormalizesLinkshellFlagsOnly() -> bool
{
    auto player = defaultSearchEntity();
    player.seacom_type = 0x40;
    player.disconnecting = true;

    auto settings = SAVE_CONF{};
    settings.MentorFlg = 1;
    settings.AwayFlg = 1;
    settings.AnonymityFlg = 1;
    settings.InviteFlg = 1;
    const auto settingsBytes = settings;
    uint32 settingsInt = 0;
    std::memcpy(&settingsInt, &settingsBytes, sizeof(uint32));

    NormalizeSearchLinkshellMemberForList(player, settingsInt, player.id);

    bool ok = true;
    ok = expectTrue(!player.mentor, "linkshell member mentor left unset") && ok;
    ok = expectEqualInt(player.flags1, 0xE908, "linkshell member flags1") && ok;
    ok = expectEqualInt(player.flags2, player.flags1, "linkshell member flags2 mirror") && ok;
    return ok;
}

auto testSearchLinkshellMemberStatePreservesUnrelatedFields() -> bool
{
    auto player = defaultSearchEntity();
    player.zone = 0;
    player.prevzone = 245;
    player.mjob = 23;
    player.languages = 7;
    player.seacom_type = 0x40;

    NormalizeSearchLinkshellMemberForList(player, 0, player.id + 1);

    bool ok = true;
    ok = expectEqualInt(player.zone, 0, "linkshell member zone left unchanged") && ok;
    ok = expectEqualInt(player.mjob, 23, "linkshell member monstrosity job left unchanged") && ok;
    ok = expectEqualInt(player.languages, 7, "linkshell member languages left unchanged") && ok;
    ok = expectEqualInt(player.flags1, 0x2100, "linkshell member flag without leader flag") && ok;
    ok = expectEqualInt(player.flags2, player.flags1, "linkshell member flags2 mirror") && ok;
    return ok;
}

auto testAcceptedPacketCopiesBytesAndSize() -> bool
{
    const auto expected = std::array<std::uint8_t, 5>{ 0x10, 0x20, 0x30, 0x40, 0x50 };
    auto input          = expected;
    auto packet         = searchPacket(input.data(), static_cast<std::uint16_t>(input.size()));

    std::fill(input.begin(), input.end(), 0xEE);

    bool ok = true;
    ok      = expectEqualInt(packet.getSize(), expected.size(), "accepted packet size") && ok;
    ok      = expectTrue(std::memcmp(packet.getData(), expected.data(), expected.size()) == 0, "accepted packet bytes copied") && ok;
    return ok;
}

auto testMaxSizePacketIsAccepted() -> bool
{
    auto input = std::array<std::uint8_t, searchPacket::max_size>{};
    for (std::size_t i = 0; i < input.size(); ++i)
    {
        input[i] = static_cast<std::uint8_t>((i * 31U) + 7U);
    }

    auto packet = searchPacket(input.data(), static_cast<std::uint16_t>(input.size()));

    bool ok = true;
    ok      = expectEqualInt(packet.getSize(), searchPacket::max_size, "max-size packet size") && ok;
    ok      = expectTrue(std::memcmp(packet.getData(), input.data(), input.size()) == 0, "max-size packet bytes copied") && ok;
    return ok;
}

auto testShortPacketCopiesPrefixAndSize() -> bool
{
    const auto input = std::array<std::uint8_t, 3>{ 0xAA, 0xBB, 0xCC };
    auto mutableInput = input;
    auto packet       = searchPacket(mutableInput.data(), static_cast<std::uint16_t>(mutableInput.size()));

    bool ok = true;
    ok      = expectEqualInt(packet.getSize(), input.size(), "short packet size") && ok;
    ok      = expectTrue(std::memcmp(packet.getData(), input.data(), input.size()) == 0, "short packet bytes copied") && ok;
    return ok;
}

auto testOversizedPacketIsRejected() -> bool
{
    auto input = std::array<std::uint8_t, searchPacket::max_size + 1>{};
    auto packet = searchPacket(input.data(), static_cast<std::uint16_t>(input.size()));

    return expectEqualInt(packet.getSize(), 0, "oversized packet size");
}

} // namespace

auto runSearchPacketBufferSelfTests() -> bool
{
    return testRequestTypeConstants() &&
           testSearchApplicationServerName() &&
           testSearchApplicationConsoleCommandDescriptors() &&
           testSearchAuctionExpirationPlanDisabled() &&
           testSearchAuctionExpirationPlanEnabled() &&
           testSearchAuctionInitializationPlanDisabled() &&
           testSearchAuctionInitializationPlanEnabled() &&
           testSearchAuctionExpirationDaysUsesOptionalFallback() &&
           testRequestTypeStrings() &&
           testPacketHashValidationAcceptsMatchingDigest() &&
           testPacketHashValidationRejectsDigestMismatch() &&
           testPacketHashValidationIgnoresTrailingKeyBytes() &&
           testPacketHashValidationAcceptsMinimumFrame() &&
           testSearchPacketEncryptMatchesManualFrame() &&
           testSearchPacketEncryptUsesUint8BlockCount() &&
           testSearchPacketDecryptMatchesManualFrame() &&
           testSearchSessionTrackerCountsActiveIP() &&
           testSearchSessionTrackerRemovesAndErasesIP() &&
           testSearchSessionTrackerIgnoresWhitelistedIP() &&
           testSearchPlayerFilterAcceptsDefaultRequest() &&
           testSearchPlayerFilterLinkshellAndAnonRules() &&
           testSearchPlayerFilterRaceRankLevelNameAndHiddenRules() &&
           testSearchPlayerFilterFlagsAndUnityRules() &&
           testSearchPlayerQueryFilterBuildsRepresentativeFragment() &&
           testSearchPlayerCountQueryUsesJobFilterForValidJobs() &&
           testSearchPlayerCountQueryCountsAllSessionsForInvalidJobs() &&
           testSearchPlayerListQueryBuildsBaseQuery() &&
           testSearchPlayerListQueryAppendsFilterBeforeOrder() &&
           testSearchPartyListQueryBuildsSQLAndPartyParams() &&
           testSearchPartyListQueryChoosesAllianceAndFallbackParams() &&
           testSearchLinkshellListQueryBuildsSQLAndParams() &&
           testSearchCommentQueryBuildsSQLAndParam() &&
           testSearchPlayerQueryFilterIgnoresInvalidOrAbsentInputs() &&
           testSearchPlayerQueryFilterCapsZoneListAtTenAndStopsAtZero() &&
           testSearchPlayerStateNormalizesFlagsAndZone() &&
           testSearchPlayerStateSetsPartyMemberWithoutLeader() &&
           testSearchPlayerStateLeavesCurrentZoneAndClearsMonstrosityJobs() &&
           testSearchRankForNationSelectsNationRank() &&
           testSearchRankForNationDefaultsInvalidNationToZero() &&
           testSearchPartyMemberStateNormalizesPartyFlagsOnly() &&
           testSearchPartyMemberStateSetsMemberWithoutLeader() &&
           testSearchLinkshellMemberStateNormalizesLinkshellFlagsOnly() &&
           testSearchLinkshellMemberStatePreservesUnrelatedFields() &&
           testAcceptedPacketCopiesBytesAndSize() &&
           testMaxSizePacketIsAccepted() &&
           testShortPacketCopiesPrefixAndSize() &&
           testOversizedPacketIsRejected();
}
