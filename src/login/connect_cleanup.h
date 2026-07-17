#pragma once

#include "common/cbasetypes.h"
#include "common/ipp.h"
#include "common/timer.h"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <fmt/format.h>
#include <string>

// Pure ConnectEngine periodic cleanup and ZMQ dealer setup helpers extracted so
// native tests can pin policy without sockets/scheduler hosts.
//
// Dual-wire pure free functions (OmegaXI slices expand individual helpers):
//   - 1325: SessionCleanInterval, ShouldEraseIdleSession, IsSessionExpired,
//           ShouldEraseEmptyIPEntry, ZMQ endpoint / routing ID
//   - 1326: ShouldEraseOnClearCommand, authorized-time / stats helpers
//   - 2963: ShouldEraseIdleSession (!hasDataSession && !hasViewSession &&
//           nowAfterExpiry) dual-wire expansion
//
// Production host: ConnectEngine::periodicCleanup (connect_engine.cpp) injects
// session.data_session != nullptr, session.view_session != nullptr, and
// IsSessionExpired(timer::now(), authorizedTime) into ShouldEraseIdleSession
// before erasing map[ip][hash].
// Go dual-wire: loginsession.ShouldEraseIdleSession
// (internal/loginsession/erase_idle.go).

namespace loginHelpers
{

// SessionCleanInterval matches ConnectEngine's kSessionCleanTime (15 minutes).
// Used both as the scheduler interval and the idle session TTL.
constexpr auto SessionCleanInterval = std::chrono::minutes(15);

// ShouldEraseIdleSession mirrors periodicCleanup's per-session erase condition:
// both data and view peers are null and authorizedTime + SessionCleanInterval
// has elapsed (strict greater-than against timer::now()).
//
// Formula (slice 2963 dual-wire):
//   !hasDataSession && !hasViewSession && nowAfterExpiry
//
// hasDataSession  — host-evaluated session.data_session != nullptr
// hasViewSession  — host-evaluated session.view_session != nullptr
// nowAfterExpiry  — host-evaluated now > authorizedTime + SessionCleanInterval
//                   (production injects IsSessionExpired(timer::now(), authorizedTime))
// true  → erase map[ip][hash] entry
// false → keep session (any live peer, or not yet past idle TTL)
//
// Dual-wire of Go loginsession.ShouldEraseIdleSession.
// Call site: ConnectEngine::periodicCleanup host inject (peer nulls + expiry).
// Sibling clear-console path (no +15min offset): ShouldEraseOnClearCommand (1326).
inline auto ShouldEraseIdleSession(
    const bool hasDataSession,
    const bool hasViewSession,
    const bool nowAfterExpiry) -> bool
{
    return !hasDataSession && !hasViewSession && nowAfterExpiry;
}

// IsSessionExpired mirrors timer::now() > authorizedTime + SessionCleanInterval.
// Exposed so tests can pin the strict > (not >=) comparison without a live clock.
inline auto IsSessionExpired(
    const timer::time_point now,
    const timer::time_point authorizedTime,
    const std::chrono::minutes cleanInterval = SessionCleanInterval) -> bool
{
    return now > authorizedTime + cleanInterval;
}

// ShouldEraseEmptyIPEntry mirrors ipAddrIterator->second.size() == 0 after
// inner-map session erases in periodicCleanup.
inline auto ShouldEraseEmptyIPEntry(const bool ipMapEmpty) -> bool
{
    return ipMapEmpty;
}

// FormatZMQEndpointString mirrors getZMQEndpointString's format.
inline auto FormatZMQEndpointString(
    const std::string& transport,
    const std::string& ip,
    const uint16       port) -> std::string
{
    return fmt::format("{}://{}:{}", transport, ip, port);
}

// ConnectDealerRoutingID mirrors getZMQRoutingId: IPP(loginAuthIP, loginAuthPort).getRawIPP().
// loginAuthIP is already the host-order/network uint32 from str2ip.
inline auto ConnectDealerRoutingID(const uint32 loginAuthIP, const uint16 loginAuthPort) -> uint64
{
    return IPP(loginAuthIP, loginAuthPort).getRawIPP();
}

// ShouldEraseOnClearCommand mirrors ConnectApplication's "clear" console path.
// Unlike periodicCleanup, production compares now > authorizedTime with NO
// +SessionCleanInterval offset — preserved for parity.
// nowAfterAuthorized is host-evaluated: now > authorizedTime.
inline auto ShouldEraseOnClearCommand(
    const bool hasDataSession,
    const bool hasViewSession,
    const bool nowAfterAuthorized) -> bool
{
    return !hasDataSession && !hasViewSession && nowAfterAuthorized;
}

// IsAuthorizedTimePassed mirrors timer::now() > authorizedTime (strict >).
inline auto IsAuthorizedTimePassed(
    const timer::time_point now,
    const timer::time_point authorizedTime) -> bool
{
    return now > authorizedTime;
}

// SumAuthenticatedAccountSessions sums each IP's inner session-map size.
// Production uniqueAccounts is this sum; uniqueIPs is the outer map size.
inline auto SumAuthenticatedAccountSessions(const std::size_t* perIPSessionCounts, const std::size_t countLength) -> std::size_t
{
    std::size_t uniqueAccounts = 0;
    for (std::size_t i = 0; i < countLength; ++i)
    {
        uniqueAccounts += perIPSessionCounts[i];
    }
    return uniqueAccounts;
}

// FormatConnectStats mirrors the ShowInfo text from the "stats" console command
// (printf-style "Serving %u IP addresses with %u accounts").
inline auto FormatConnectStats(const std::size_t uniqueIPs, const std::size_t uniqueAccounts) -> std::string
{
    return fmt::format("Serving {} IP addresses with {} accounts", uniqueIPs, uniqueAccounts);
}

// ConnectServerName is ApplicationConfig.serverName for ConnectApplication.
inline constexpr const char* ConnectServerName = "connect";

// Console command registration names/help for ConnectApplication::registerCommands.
inline constexpr const char* ConnectStatsCommandName = "stats";
inline constexpr const char* ConnectStatsCommandHelp = "Print server runtime statistics";
inline constexpr const char* ConnectClearCommandName = "clear";
inline constexpr const char* ConnectClearCommandHelp = "Run periodic session cleanup routine";

// ConnectMainExitCode is the fixed process exit code after connectApp->run().
// Production always returns 0 regardless of run outcome.
inline constexpr int ConnectMainExitCode = 0;

} // namespace loginHelpers
