#pragma once

namespace mapnetworkinghelpers
{

// ShouldOpenSocket mirrors MapNetworking construction: embedded test servers
// skip UDP socket creation.
inline auto ShouldOpenSocket(const bool isTestServer) -> bool
{
    return !isTestServer;
}

} // namespace mapnetworkinghelpers
