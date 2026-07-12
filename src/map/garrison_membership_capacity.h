#pragma once

#include <cstdint>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Pure xi.garrison.isInGarrison membership + process host for zone rosters.
// Parity: internal/garrison membership.go (slice 1591).
// Lua start/stop sync rosters; CBattleEntity::isInGarrison queries the host.

namespace garrisonmembershiphelpers
{

// Pure: utils.contains(playerId, players).
inline auto ContainsPlayerID(const std::uint32_t playerId, const std::vector<std::uint32_t>& players) -> bool
{
    for (const auto id : players)
    {
        if (id == playerId)
        {
            return true;
        }
    }
    return false;
}

// Pure inject of isInGarrison given a resolved roster.
inline auto IsInGarrison(const std::uint16_t zoneId,
                         const std::uint32_t playerId,
                         const bool         zoneHasRoster,
                         const std::vector<std::uint32_t>& players) -> bool
{
    if (zoneId == 0 || !zoneHasRoster)
    {
        return false;
    }
    return ContainsPlayerID(playerId, players);
}

// ---------------------------------------------------------------------------
// Host: per-zone active garrison player sets (synced from Lua start/stop).
// ---------------------------------------------------------------------------

namespace detail
{

inline auto RosterMutex() -> std::mutex&
{
    static std::mutex m;
    return m;
}

inline auto Rosters() -> std::unordered_map<std::uint16_t, std::unordered_set<std::uint32_t>>&
{
    static std::unordered_map<std::uint16_t, std::unordered_set<std::uint32_t>> rosters;
    return rosters;
}

} // namespace detail

inline void SetZonePlayers(const std::uint16_t zoneId, const std::vector<std::uint32_t>& playerIds)
{
    std::lock_guard lock(detail::RosterMutex());
    auto&           set = detail::Rosters()[zoneId];
    set.clear();
    set.insert(playerIds.begin(), playerIds.end());
}

inline void ClearZonePlayers(const std::uint16_t zoneId)
{
    std::lock_guard lock(detail::RosterMutex());
    detail::Rosters().erase(zoneId);
}

// Production query used by CBattleEntity::isInGarrison.
inline auto IsPlayerInGarrison(const std::uint16_t zoneId, const std::uint32_t playerId) -> bool
{
    if (zoneId == 0)
    {
        return false;
    }
    std::lock_guard lock(detail::RosterMutex());
    const auto      it = detail::Rosters().find(zoneId);
    if (it == detail::Rosters().end())
    {
        return false;
    }
    return it->second.find(playerId) != it->second.end();
}

// Test helper: wipe all host rosters.
inline void ClearAllZonePlayers()
{
    std::lock_guard lock(detail::RosterMutex());
    detail::Rosters().clear();
}

} // namespace garrisonmembershiphelpers
