#pragma once

namespace mobcontrollerbufftickadmission
{
enum class Action
{
    KeepCasting,
    Reject,
    Cast,
};

// Resolve selects the next action for a mob's buff tick.
template <typename IsSpellReady, typename HasBuffSpell>
constexpr auto Resolve(const bool isCasting, IsSpellReady&& isSpellReady, HasBuffSpell&& hasBuffSpell) -> Action
{
    if (isCasting)
    {
        return Action::KeepCasting;
    }
    if (!isSpellReady() || !hasBuffSpell())
    {
        return Action::Reject;
    }
    return Action::Cast;
}
} // namespace mobcontrollerbufftickadmission
