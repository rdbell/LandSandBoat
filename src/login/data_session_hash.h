#pragma once

#include <optional>
#include <string>

namespace loginHelpers
{

// ResolveDataSessionHash mirrors data_session::read_func: prefer a hash from
// the current packet, otherwise reuse the connection's stored hash. No hash
// is accepted only when both sources are empty.
inline auto ResolveDataSessionHash(const std::string& packetHash, const std::string& storedHash) -> std::optional<std::string>
{
    if (!packetHash.empty())
    {
        return packetHash;
    }
    if (!storedHash.empty())
    {
        return storedHash;
    }
    return std::nullopt;
}

} // namespace loginHelpers
