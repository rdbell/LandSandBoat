#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <string_view>
#include <utility>

// Pure character-variable cache policy from CCharEntity char var helpers.
// Host retains unordered_map/set storage, DB fetch/persist, and earth timestamps.

namespace charvarcachehelpers
{

// Cached entry is value + expiry (0 = never expires).
using Cached = std::pair<std::int32_t, std::uint32_t>;

// IsFresh mirrors expiry == 0 || expiry > now.
constexpr auto IsFresh(const std::uint32_t expiry, const std::uint32_t now) -> bool
{
    return expiry == 0 || expiry > now;
}

// ShouldUseCacheHit reports whether a found cache entry should be returned
// without falling through to the database.
constexpr auto ShouldUseCacheHit(const bool found, const std::uint32_t expiry, const std::uint32_t now) -> bool
{
    return found && IsFresh(expiry, now);
}

// MinClearPrefixLength is the minimum prefix size accepted by clearCharVarsWithPrefix.
constexpr std::size_t MinClearPrefixLength = 5;

// ShouldRejectClearPrefix mirrors prefix.size() < 5.
constexpr auto ShouldRejectClearPrefix(const std::size_t prefixSize) -> bool
{
    return prefixSize < MinClearPrefixLength;
}

// StartsWithPrefix mirrors varname.rfind(prefix, 0) == 0.
inline auto StartsWithPrefix(const std::string_view varname, const std::string_view prefix) -> bool
{
    return varname.size() >= prefix.size() && varname.compare(0, prefix.size(), prefix) == 0;
}

// ClearedEntry is the {0, 0} sentinel written over matching cache keys on clear.
constexpr auto ClearedEntry() -> Cached
{
    return Cached{ 0, 0 };
}

// ShouldIncludeRow mirrors the prefix/suffix query filter for non-expired rows.
constexpr auto ShouldIncludeRow(const std::uint32_t expiry, const std::uint32_t now) -> bool
{
    return IsFresh(expiry, now);
}

// MakeEntry builds a cache entry for set/update/fetch paths.
constexpr auto MakeEntry(const std::int32_t value, const std::uint32_t expiry) -> Cached
{
    return Cached{ value, expiry };
}

} // namespace charvarcachehelpers
