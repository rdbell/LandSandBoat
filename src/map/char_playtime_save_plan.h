#pragma once

#include <cstdint>

namespace playtimesavehelpers
{
constexpr std::int64_t NewAdventurerPlayTimeSeconds = 240 * 60 * 60;
struct PlayTimeSavePlan
{
    std::uint32_t persistPlayTime{};
    bool clearNewAdventurer{};
    bool setUpdateHP{};
    bool savePlayerSettings{};
    constexpr auto operator==(const PlayTimeSavePlan&) const -> bool = default;
};
constexpr auto MakePlayTimeSavePlan(const std::int64_t playSeconds, const bool isNewPlayer) -> PlayTimeSavePlan
{
    const bool clearNewAdventurer = isNewPlayer && playSeconds >= NewAdventurerPlayTimeSeconds;
    return {
        .persistPlayTime = static_cast<std::uint32_t>(playSeconds),
        .clearNewAdventurer = clearNewAdventurer,
        .setUpdateHP = clearNewAdventurer,
        .savePlayerSettings = clearNewAdventurer,
    };
}
} // namespace playtimesavehelpers
