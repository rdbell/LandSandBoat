#pragma once

#include <cstdint>

namespace historyloadhelpers
{
struct CharHistory
{
    std::uint32_t enemiesDefeated{};
    std::uint32_t timesKnockedOut{};
    std::uint32_t mhEntrances{};
    std::uint32_t joinedParties{};
    std::uint32_t joinedAlliances{};
    std::uint32_t spellsCast{};
    std::uint32_t abilitiesUsed{};
    std::uint32_t wsUsed{};
    std::uint32_t itemsUsed{};
    std::uint32_t chatsSent{};
    std::uint32_t npcInteractions{};
    std::uint32_t battlesFought{};
    std::uint32_t gmCalls{};
    std::uint32_t distanceTravelled{};
    constexpr auto operator==(const CharHistory&) const -> bool = default;
};
struct HistoryLoadPlan
{
    bool apply{};
    CharHistory history{};
    constexpr auto operator==(const HistoryLoadPlan&) const -> bool = default;
};
constexpr auto MakeHistoryLoadPlan(const bool hasCharacter, const bool hasRow, const CharHistory history) -> HistoryLoadPlan
{
    if (!hasCharacter || !hasRow) return {};
    return { .apply = true, .history = history };
}
} // namespace historyloadhelpers
