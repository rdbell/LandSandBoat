#include "ipc_lookup.h"

#include "common/database.h"

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
    if (!rset || !rset->rowsCount())
    {
        return {};
    }

    std::vector<IPP> endpoints;
    while (rset->next())
    {
        endpoints.emplace_back(rset->get<uint32>("server_addr"), rset->get<uint16>("server_port"));
    }
    return endpoints;
}
