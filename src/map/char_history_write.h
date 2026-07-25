#pragma once

#include "char_history_load.h"

namespace historywritehelpers
{
struct HistoryWritePlan
{
    bool                            persist{};
    historyloadhelpers::CharHistory history{};

    constexpr auto operator==(const HistoryWritePlan&) const -> bool = default;
};

// MakeHistoryWritePlan preserves WriteHistory's null-character behavior.
constexpr auto MakeHistoryWritePlan(const bool hasCharacter, const historyloadhelpers::CharHistory history) -> HistoryWritePlan
{
    if (!hasCharacter)
    {
        return {};
    }

    return {
        .persist = true,
        .history = history,
    };
}
} // namespace historywritehelpers
