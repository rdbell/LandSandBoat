#pragma once

#include "character_select.h"

// Pure VIEW 0x1F acquire-player response selection after data-session presence.

namespace login
{

struct view_acquire_player_response_plan
{
    bool writeAcquireNotify{};
    bool writeLobbyError{};
    bool returnFromRead{};
};

// PlanViewAcquirePlayerResponse mirrors view_session's 0x1F branch. A present
// data peer receives the acquire-player notify; a missing peer writes the lobby
// error on the current view buffer and returns from read.
inline auto PlanViewAcquirePlayerResponse(
    const loginHelpers::data_session_presence_gate presence) -> view_acquire_player_response_plan
{
    if (presence == loginHelpers::data_session_presence_gate::PRESENT)
    {
        return { .writeAcquireNotify = true };
    }
    return { .writeLobbyError = true, .returnFromRead = true };
}

} // namespace login
