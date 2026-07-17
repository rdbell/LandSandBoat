#pragma once

#include "view_lobby_ack.h"

// Pure VIEW 0x21 character-create response selection after createCharacter
// returns success/failure. SQL/socket hosts remain outside this plan.

namespace login
{

struct view_character_create_response_plan
{
    bool closeViewSocket{};
    bool returnFromRead{};
    bool addCharIntoDataSession{};
    bool setJustCreatedNewChar{};
    bool logCharacterCreated{};
    bool writeLobbyAck{};
};

// PlanViewCharacterCreateResponse mirrors view_session's 0x21 branch after
// createCharacter. Failure closes the view socket; success optionally inserts
// into the data peer char list, marks justCreatedNewChar, logs, and acks.
inline auto PlanViewCharacterCreateResponse(
    const bool createSucceeded,
    const bool hasDataSession) -> view_character_create_response_plan
{
    if (!createSucceeded)
    {
        return { .closeViewSocket = true, .returnFromRead = true };
    }
    return {
        .addCharIntoDataSession = hasDataSession,
        .setJustCreatedNewChar  = true,
        .logCharacterCreated    = true,
        .writeLobbyAck          = true,
    };
}

} // namespace login
