#pragma once

namespace trustcontrollermeleepathresult
{
enum class Action
{
    Hold,
    Follow,
    Step,
};

template <typename PathSucceeded, typename HasSpeed>
constexpr auto Resolve(PathSucceeded&& pathSucceeded, HasSpeed&& hasSpeed) -> Action
{
    if (pathSucceeded())
    {
        return Action::Follow;
    }
    return hasSpeed() ? Action::Step : Action::Hold;
}
} // namespace trustcontrollermeleepathresult
