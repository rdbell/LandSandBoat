#pragma once

namespace pathfindfinishedhelpers
{
enum class Action
{
    NextTurn,
    RestartPatrol,
    Clear,
};

constexpr auto Resolve(bool hasNextTurn, bool isPatrolling, bool isRoaming) -> Action
{
    if (hasNextTurn)
    {
        return Action::NextTurn;
    }

    if (isPatrolling && isRoaming)
    {
        return Action::RestartPatrol;
    }

    return Action::Clear;
}
} // namespace pathfindfinishedhelpers
