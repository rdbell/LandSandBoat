#pragma once

namespace mobcontrollershareposition
{
enum class Action
{
    None,
    Copy,
    Warn,
};

// Resolve mirrors Move's shared-position branch.
constexpr auto Resolve(const bool enabled, const bool sourceFound) -> Action
{
    if (!enabled)
    {
        return Action::None;
    }
    return sourceFound ? Action::Copy : Action::Warn;
}
} // namespace mobcontrollershareposition
