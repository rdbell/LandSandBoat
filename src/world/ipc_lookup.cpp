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
