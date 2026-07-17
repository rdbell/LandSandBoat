#pragma once

#include "character_delete.h"
#include "view_lobby_ack.h"

// Pure VIEW 0x14 character-delete response selection across the enable gate,
// post-ack ownership gate, and successful soft-delete side effects.

namespace login
{

struct view_character_delete_response_plan
{
    bool writeLobbyError{};
    bool returnFromRead{};
    bool writeLobbyAck{};
    bool logDeleteAttempt{};
    bool runOwnershipQuery{};
    bool logWrongAccount{};
    bool closeViewSocket{};
    bool clearCharFromDataSession{};
    bool softDeleteCharacter{};
    bool bumpDeleteKey{};
};

// PlanViewCharacterDeleteEnableResponse handles the CHARACTER_DELETION settings
// gate before any packet parse/ack.
inline auto PlanViewCharacterDeleteEnableResponse(
    const loginHelpers::character_deletion_gate gate) -> view_character_delete_response_plan
{
    if (gate == loginHelpers::character_deletion_gate::DENIED)
    {
        return { .writeLobbyError = true, .returnFromRead = true };
    }
    return {
        .writeLobbyAck     = true,
        .logDeleteAttempt  = true,
        .runOwnershipQuery = true,
    };
}

// PlanViewCharacterDeleteOwnershipResponse handles ownership after the ack and
// attempt log. DENIED closes the view socket; PROCEED soft-deletes.
inline auto PlanViewCharacterDeleteOwnershipResponse(
    const loginHelpers::character_delete_ownership_gate gate,
    const bool                                          hasDataSession) -> view_character_delete_response_plan
{
    if (gate == loginHelpers::character_delete_ownership_gate::DENIED)
    {
        return { .returnFromRead = true, .logWrongAccount = true, .closeViewSocket = true };
    }
    return {
        .clearCharFromDataSession = hasDataSession,
        .softDeleteCharacter      = true,
        .bumpDeleteKey            = true,
    };
}

} // namespace login
