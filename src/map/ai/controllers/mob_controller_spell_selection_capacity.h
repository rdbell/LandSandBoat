#pragma once

#include <cstdint>

namespace mobcontrollerspellselection
{
enum class Source : std::uint8_t
{
    Random,
    Buff,
    Aggro,
};

inline auto Select(bool engaged, bool hasBuffSpells, bool firstSpell) -> Source
{
    if (!engaged)
    {
        return hasBuffSpells ? Source::Buff : Source::Random;
    }
    return firstSpell ? Source::Aggro : Source::Random;
}
} // namespace mobcontrollerspellselection
