#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

// Pure CCommandHandler::call permission / audit policy halves.
// Lua table lookup (xi.commands / cmdprops / onTrigger) stays host-side.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 2792: permission / audit plan suite (null/name/perm/audit + post-props)
//   - 2836: ShouldRejectEmptyCommandLine (ParseCommandLine empty after trim)
//   - 2940: ShouldAllowCommandPermission (permission <= m_GMlevel)
//   - 2982: ShouldRejectNullChar (charNull identity)
//
// Production host: CCommandHandler::call injects PChar->m_GMlevel and Lua
// cmdprops permission into PlanCommandCallPostProps / ShouldAllowCommandPermission.
// Go dual-wire: command.ShouldAllowCommandPermission
// (internal/command/permission.go).
//
// Production host: CCommandHandler::call injects charNull = (PChar == nullptr)
// before name parse / Lua table lookup.
// Go dual-wire: command.ShouldRejectNullChar
// (internal/command/reject_null_char.go).

namespace commandhandlerhelpers
{

// ShouldRejectNullChar mirrors !PChar early Failure.
//
// Formula (slice 2982 dual-wire):
//   charNull
//
// true  → host logs error and returns CommandResult::Failure
// false → proceed to name parse / empty-name gate
//
// Dual-wire of Go command.ShouldRejectNullChar.
// Call site: CCommandHandler::call before ParseCommandLine.
//   if (ShouldRejectNullChar(PChar == nullptr)) return Failure;
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
// Formula (slice 2940 dual-wire):
//   static_cast<int>(permission) <= static_cast<int>(gmLevel)
// which is equivalent to !(permission > gmLevel) after usual arithmetic
// conversions promote int8 / uint8 to int.
//
// LSB types: permission is int8 from Lua cmdprops; m_GMlevel is uint8.
// Negative permission never exceeds any gmLevel after signed promotion.
// Host fails the call when this returns false (warn Failure).
//
// Dual-wire of Go command.ShouldAllowCommandPermission.
// Call site: CCommandHandler::call via PlanCommandCallPostProps after cmdprops load.
inline auto ShouldAllowCommandPermission(const uint8 gmLevel, const int8 permission) -> bool
{
    return static_cast<int>(permission) <= static_cast<int>(gmLevel);
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

// ShouldRejectEmptyCommandLine mirrors ParseCommandLine after trimLeft:
//   if (view.empty()) return {};
// Host dual-wires:
//   if (ShouldRejectEmptyCommandLine(view.empty())) return {};
// Identity pure on the post-trim empty flag (true → empty commandline reject).
inline auto ShouldRejectEmptyCommandLine(const bool viewEmptyAfterTrim) -> bool
{
    return viewEmptyAfterTrim;
}

} // namespace commandhandlerhelpers
