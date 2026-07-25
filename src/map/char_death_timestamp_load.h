#pragma once

#include "zone_out_capacity.h"

// Pure loadDeathTimestamp policy from charutils.

namespace deathtimestamploadhelpers
{
struct DeathTimestampLoadPlan
{
    bool         setDeathTime{};
    bool         callDie{};
    std::uint32_t deathTimeSecondsAgo{};
    std::int64_t  dieRemainingSeconds{};

    constexpr auto operator==(const DeathTimestampLoadPlan&) const -> bool = default;
};

// MakeDeathTimestampLoadPlan mirrors loadDeathTimestamp after a database row
// has been read. It deliberately leaves elapsed durations beyond deathDuration
// negative for Die, matching upstream.
constexpr auto MakeDeathTimestampLoadPlan(
    const bool hasRow,
    const std::uint32_t currentHP,
    const std::uint32_t secondsSinceDeath,
    const std::int64_t deathDurationSeconds) -> DeathTimestampLoadPlan
{
    if (!hasRow || !zoneouthelpers::ShouldApplyDeathTimestamp(currentHP))
    {
        return {};
    }

    return {
        .setDeathTime          = true,
        .callDie               = true,
        .deathTimeSecondsAgo   = secondsSinceDeath,
        .dieRemainingSeconds   = zoneouthelpers::RemainingDeathDurationSeconds(deathDurationSeconds, secondsSinceDeath),
    };
}
} // namespace deathtimestamploadhelpers
