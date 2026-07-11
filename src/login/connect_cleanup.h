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

namespace loginHelpers
{

// SessionCleanInterval matches ConnectEngine's kSessionCleanTime (15 minutes).
// Used both as the scheduler interval and the idle session TTL.
constexpr auto SessionCleanInterval = std::chrono::minutes(15);

// ShouldEraseIdleSession mirrors periodicCleanup's per-session erase condition:
// both data and view peers are null and authorizedTime + SessionCleanInterval
// has elapsed (strict greater-than against timer::now()).
// nowAfterExpiry is host-evaluated: now > authorizedTime + SessionCleanInterval.
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

} // namespace loginHelpers
