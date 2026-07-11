#include "ipc_lookup.h"

#include "common/database.h"

namespace
{

template <typename IP, typename Port>
auto CollectEndpoints(db::ResultSet* result) -> std::vector<IPP>
{
    if (!result || !result->rowsCount())
    {
        return {};
    }
    std::vector<IPP> endpoints;
    while (result->next())
    {
        const auto ip   = result->get<IP>("server_addr");
        const auto port = result->get<Port>("server_port");
        endpoints.emplace_back(ip, port);
    }
    return endpoints;
}

} // namespace

auto world::ipc::LookupCharacterEndpoint(const uint32_t charId) -> Maybe<IPP>
{
    const auto rset = db::preparedStmt("SELECT server_addr, server_port FROM accounts_sessions WHERE charid = ? LIMIT 1", charId);
    if (rset && rset->rowsCount() && rset->next())
    {
        return IPP(rset->get<uint32>("server_addr"), rset->get<uint16>("server_port"));
    }
    return std::nullopt;
}

auto world::ipc::LookupCharacterNameEndpoint(const std::string& charName) -> Maybe<IPP>
{
    const auto rset = db::preparedStmt("SELECT server_addr, server_port FROM accounts_sessions LEFT JOIN chars ON "
                                       "accounts_sessions.charid = chars.charid WHERE charname = ? LIMIT 1",
                                       charName);
    if (rset && rset->rowsCount() && rset->next())
    {
        return IPP(rset->get<uint32>("server_addr"), rset->get<uint16>("server_port"));
    }
    return std::nullopt;
}

auto world::ipc::LookupPartyEndpoints(const uint32_t partyId) -> std::vector<IPP>
{
    const auto rset = db::preparedStmt(
        "SELECT server_addr, server_port, MIN(charid) FROM accounts_sessions JOIN accounts_parties USING (charid) "
        "WHERE IF (allianceid <> 0, allianceid = (SELECT MAX(allianceid) FROM accounts_parties WHERE partyid = ?), "
        "partyid = ?) GROUP BY server_addr, server_port",
        partyId,
        partyId);
    return CollectEndpoints<uint32, uint16>(rset.get());
}

auto world::ipc::LookupAllianceEndpoints(const uint32_t allianceId) -> std::vector<IPP>
{
    const auto rset = db::preparedStmt(
        "SELECT server_addr, server_port, MIN(charid) FROM accounts_sessions JOIN accounts_parties USING (charid) "
        "WHERE allianceid = ? GROUP BY server_addr, server_port",
        allianceId);
    return CollectEndpoints<uint64, uint64>(rset.get());
}

auto world::ipc::LookupLinkshellEndpoints(const uint32_t linkshellId) -> std::vector<IPP>
{
    const auto rset = db::preparedStmt(
        "SELECT server_addr, server_port FROM accounts_sessions "
        "WHERE linkshellid1 = ? OR linkshellid2 = ? GROUP BY server_addr, server_port",
        linkshellId,
        linkshellId);
    return CollectEndpoints<uint64, uint64>(rset.get());
}

auto world::ipc::LookupUnityEndpoints(const uint32_t unityId) -> std::vector<IPP>
{
    const auto rset = db::preparedStmt(
        "SELECT server_addr, server_port FROM accounts_sessions "
        "WHERE unitychat = ? GROUP BY server_addr, server_port",
        unityId);
    return CollectEndpoints<uint64, uint64>(rset.get());
}

auto world::ipc::LookupKillSessionZones(const uint32_t victimId) -> Maybe<worldipc::KillSessionZones>
{
    const auto rset = db::preparedStmt("SELECT pos_prevzone, pos_zone from chars where charid = ? LIMIT 1", victimId);
    if (rset && rset->rowsCount() && rset->next())
    {
        return worldipc::KillSessionZones{
            .previous = rset->get<uint32>("pos_prevzone"),
            .current  = rset->get<uint32>("pos_zone"),
        };
    }
    return std::nullopt;
}
