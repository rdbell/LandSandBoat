#pragma once

namespace mobcontrollerdeaggroretarget
{
// Action identifies the battle-target update after selecting highest enmity.
enum class Action
{
    Replacement,
    Clear,
};

// Resolve selects a replacement target when enmity remains, otherwise clears
// the battle target.
constexpr auto Resolve(const bool hasReplacementTarget) -> Action
{
    return hasReplacementTarget ? Action::Replacement : Action::Clear;
}
} // namespace mobcontrollerdeaggroretarget
