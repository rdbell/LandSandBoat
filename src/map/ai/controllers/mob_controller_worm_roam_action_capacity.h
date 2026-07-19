#pragma once

namespace mobcontrollerwormroamaction
{
enum class Action
{
    RoamAround,
    Burrow,
    Wait,
};

inline auto Select(const bool worm, const bool nameHidden, const bool magicState) -> Action
{
    if (!worm || nameHidden)
    {
        return Action::RoamAround;
    }

    return magicState ? Action::Wait : Action::Burrow;
}
} // namespace mobcontrollerwormroamaction
