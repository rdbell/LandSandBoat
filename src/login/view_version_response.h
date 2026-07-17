#pragma once

#include "version_lock.h"

// Pure VIEW 0x26 response selection after version-lock evaluation. This keeps
// the unusual missing-view-peer path directly testable without sockets.

namespace login
{

struct view_version_response_plan
{
    bool shapeKeyPacket{};
    bool writeKeyPacket{};
    bool writeVersionError{};
    bool returnFromRead{};
};

// PlanViewVersionResponse mirrors view_session's response branch. A fatal
// version response exits only after it is written to an existing view peer.
// Without that peer, production continues to shape the normal key packet but
// has no write target.
inline auto PlanViewVersionResponse(
    const version_lock::ResponseLength responseLength,
    const bool                         hasViewSession) -> view_version_response_plan
{
    if (responseLength == version_lock::ResponseLength::VersionError && hasViewSession)
    {
        return { .writeVersionError = true, .returnFromRead = true };
    }

    return { .shapeKeyPacket = true, .writeKeyPacket = hasViewSession };
}

} // namespace login
