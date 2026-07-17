#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

// Pure CCommandHandler::call permission / audit policy halves.
// Lua table lookup (xi.commands / cmdprops / onTrigger) stays host-side.

namespace commandhandlerhelpers
{

// ShouldRejectNullChar mirrors !PChar early Failure.
inline auto ShouldRejectNullChar(const bool charNull) -> bool
{
    return charNull;
}

// ShouldRejectEmptyCommandName mirrors !parsedName.valid after name-only parse.
// valid is ParseCommandLine(...).valid; reject when the name parse failed.
inline auto ShouldRejectEmptyCommandName(const bool valid) -> bool
{
    return !valid;
}

// ShouldAllowCommandPermission mirrors !(permission > PChar->m_GMlevel).
//
// LSB types: permission is int8 from Lua cmdprops; m_GMlevel is uint8.
// The C++ comparison uses usual arithmetic conversions (both promote to int
// on typical platforms), so negative permission never exceeds any gmLevel.
// Host fails the call when this returns false.
inline auto ShouldAllowCommandPermission(const uint8 gmLevel, const int8 permission) -> bool
{
    return !(permission > gmLevel);
}

// ShouldAuditGMCommand mirrors auditLevel <= permission && auditLevel > 0
// after settings::get<uint8>("map.AUDIT_GM_CMD").
// auditLevel is uint8; permission is int8 — same promotion rules as above.
// Host schedules the audit_gm INSERT when this returns true.
inline auto ShouldAuditGMCommand(const uint8 auditLevel, const int8 permission) -> bool
{
    return auditLevel <= permission && auditLevel > 0;
}

// CommandCallPostPropsPlan is the pure disposition after cmdprops permission
// has been loaded: permission gate + audit scheduling decision.
// Name/null gates and Lua table lookup remain separate host steps.
struct CommandCallPostPropsPlan
{
    bool rejectPermission{}; // !ShouldAllowCommandPermission
    bool scheduleAudit{};    // ShouldAuditGMCommand when permission allowed
};

// PlanCommandCallPostProps composes permission allow + audit schedule.
// When permission is rejected, scheduleAudit is forced false (host returns
// before the audit branch).
inline auto PlanCommandCallPostProps(const uint8 gmLevel, const int8 permission, const uint8 auditLevel)
    -> CommandCallPostPropsPlan
{
    if (!ShouldAllowCommandPermission(gmLevel, permission))
    {
        return CommandCallPostPropsPlan{ true, false };
    }
    return CommandCallPostPropsPlan{ false, ShouldAuditGMCommand(auditLevel, permission) };
}

} // namespace commandhandlerhelpers
