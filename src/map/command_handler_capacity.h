#pragma once

#include "common/cbasetypes.h"

#include <cstdint>

// Pure CCommandHandler::call permission / audit policy halves.
// Lua table lookup (xi.commands / cmdprops / onTrigger) stays host-side.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 2792: permission / audit plan suite (null/name/perm/audit + post-props)
//   - 2836: ShouldRejectEmptyCommandLine residual (ParseCommandLine empty after trim)
//   - 2940: ShouldAllowCommandPermission (permission <= m_GMlevel)
//   - 2982: ShouldRejectNullChar (charNull identity)
//   - 2990: ShouldRejectEmptyCommandName (!valid after name parse)
//   - 3005: ShouldRejectEmptyCommandLine (viewEmptyAfterTrim identity dual-wire)
//   - 3011: ShouldAuditGMCommand (auditLevel <= permission && auditLevel > 0)
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
//
// Production host: CCommandHandler::call injects valid = parsedName.valid after
// name-only ParseCommandLine before Lua table lookup.
// Go dual-wire: command.ShouldRejectEmptyCommandName
// (internal/command/reject_empty_command_name.go).
//
// Production host: ParseCommandLine injects viewEmptyAfterTrim = view.empty()
// after trimLeft before name/arg token extraction.
// Go dual-wire: command.ShouldRejectEmptyCommandLine
// (internal/command/reject_empty_command_line.go).
//
// Production host: CCommandHandler::call injects auditLevel =
// settings::get<uint8>("map.AUDIT_GM_CMD") and Lua cmdprops permission into
// PlanCommandCallPostProps / ShouldAuditGMCommand after permission allow.
// Go dual-wire: command.ShouldAuditGMCommand
// (internal/command/audit_gm_command.go).

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
//
// Formula (slice 2990 dual-wire):
//   !valid
//
// true  → host logs error and returns CommandResult::Failure
// false → proceed to Lua table lookup / permission
//
// Dual-wire of Go command.ShouldRejectEmptyCommandName.
// Call site: CCommandHandler::call after name-only ParseCommandLine.
//   if (ShouldRejectEmptyCommandName(parsedName.valid)) return Failure;
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
//
// Formula (slice 3011 dual-wire; residual 2792):
//   static_cast<int>(auditLevel) <= static_cast<int>(permission) && auditLevel > 0
// which is equivalent to (auditLevel <= permission && auditLevel > 0) after
// usual arithmetic conversions promote uint8 / int8 to int.
//
// LSB types: auditLevel is uint8 from map.AUDIT_GM_CMD; permission is int8
// from Lua cmdprops. Negative permission never schedules audit after signed
// promotion (positive auditLevel is never <= negative permission).
// auditLevel == 0 always returns false (audit disabled).
// Host schedules the audit_gm INSERT when this returns true.
//
// Dual-wire of Go command.ShouldAuditGMCommand.
// Call site: CCommandHandler::call via PlanCommandCallPostProps after cmdprops load
// (only consulted for host disposition when permission is allowed).
inline auto ShouldAuditGMCommand(const uint8 auditLevel, const int8 permission) -> bool
{
    return static_cast<int>(auditLevel) <= static_cast<int>(permission) && auditLevel > 0;
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
//
// Formula (slice 3005 dual-wire; residual 2836):
//   viewEmptyAfterTrim
//
// true  → host returns {} (invalid parse) before name/arg token extraction
// false → proceed to popToken / parameter typing
//
// Dual-wire of Go command.ShouldRejectEmptyCommandLine.
// Call site: commandhandler::detail::ParseCommandLine after trimLeft.
//   if (ShouldRejectEmptyCommandLine(view.empty())) return {};
// viewEmptyAfterTrim is view.empty() after space/tab left-trim.
// Identity pure on the post-trim empty flag (true → empty commandline reject).
inline auto ShouldRejectEmptyCommandLine(const bool viewEmptyAfterTrim) -> bool
{
    return viewEmptyAfterTrim;
}

} // namespace commandhandlerhelpers
