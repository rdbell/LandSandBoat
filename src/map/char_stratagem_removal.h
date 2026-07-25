#pragma once

#include <array>
#include <cstdint>

namespace stratagemremovalhelpers
{
enum class SpellGroup : std::uint8_t
{
    Other,
    White,
    Black,
};

enum class Effect : std::uint8_t
{
    Penury,
    Celerity,
    Enlightenment,
    Altruism,
    Tranquility,
    Accession,
    Perpetuance,
    Parsimony,
    Alacrity,
    Focalization,
    Equanimity,
    Manifestation,
};

struct Plan
{
    std::array<Effect, 7> effects{};
    std::uint8_t count{};
    constexpr auto operator==(const Plan&) const -> bool = default;
};

// MakePlan preserves the ordered status-effect removals in RemoveStratagems.
constexpr auto MakePlan(const SpellGroup group, const bool accessionAOE, const bool manifestationAOE, const bool enhancingMagic) -> Plan
{
    if (group == SpellGroup::White)
    {
        Plan plan{
            .effects = { Effect::Penury, Effect::Celerity, Effect::Enlightenment, Effect::Altruism, Effect::Tranquility },
            .count   = 5,
        };
        if (accessionAOE)
        {
            plan.effects[plan.count++] = Effect::Accession;
        }
        if (enhancingMagic)
        {
            plan.effects[plan.count++] = Effect::Perpetuance;
        }
        return plan;
    }
    if (group == SpellGroup::Black)
    {
        Plan plan{
            .effects = { Effect::Parsimony, Effect::Alacrity, Effect::Enlightenment, Effect::Focalization, Effect::Equanimity },
            .count   = 5,
        };
        if (manifestationAOE)
        {
            plan.effects[plan.count++] = Effect::Manifestation;
        }
        return plan;
    }
    return {};
}
} // namespace stratagemremovalhelpers
