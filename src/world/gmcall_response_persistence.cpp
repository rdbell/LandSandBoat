#include "gmcall_response_persistence.h"

#include "common/database.h"

#include <string>

void world::gmcall::PersistResponse(const uint32_t callId, const std::string_view response)
{
    db::preparedStmt("UPDATE help_desk "
                     "SET response = ?, responded_at = NOW() "
                     "WHERE id = ?",
                     std::string(response),
                     callId);
}
