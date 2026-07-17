#pragma once

#include "character_name.h"
#include "view_lobby_ack.h"

// Pure VIEW 0x22 name-check response selection across the creation-enable gate
// and post-validation name outcome.

namespace login
{

struct view_name_check_response_plan
{
    bool writeRegisterError{};
    bool returnFromRead{};
    bool logInvalidName{};
    bool writeNameUnavailableError{};
    bool setRequestedNewCharacterName{};
    bool writeLobbyAck{};
};

// PlanViewNameCheckEnableResponse handles the MAINT_MODE / CHARACTER_CREATION
// gate before any name extract/validation.
inline auto PlanViewNameCheckEnableResponse(
    const loginHelpers::character_creation_gate gate) -> view_name_check_response_plan
{
    if (gate == loginHelpers::character_creation_gate::DENIED)
    {
        return { .writeRegisterError = true, .returnFromRead = true };
    }
    // ALLOW: host continues with name extract + local/DB/banned-word validation.
    return {};
}

// PlanViewNameCheckNameResponse handles the outcome after name validation.
// An invalid reason logs and writes name-unavailable; a valid name records the
// request and acks the lobby.
inline auto PlanViewNameCheckNameResponse(const bool nameValid) -> view_name_check_response_plan
{
    if (!nameValid)
    {
        return {
            .returnFromRead            = true,
            .logInvalidName            = true,
            .writeNameUnavailableError = true,
        };
    }
    return {
        .setRequestedNewCharacterName = true,
        .writeLobbyAck                = true,
    };
}

} // namespace login
