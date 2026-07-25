#pragma once

#include <cstdint>

namespace charvarpersisthelpers
{
enum class Action : std::uint8_t
{
    Delete,
    Upsert,
};

// ActionFor preserves PersistCharVar's zero-value operation choice.
constexpr auto ActionFor(const std::int32_t value) -> Action
{
    if (value == 0)
    {
        return Action::Delete;
    }

    return Action::Upsert;
}
} // namespace charvarpersisthelpers
