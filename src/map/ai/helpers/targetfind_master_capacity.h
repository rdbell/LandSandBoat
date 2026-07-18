#pragma once

#include "targetfind.h"

namespace targetfindmasterhelpers
{
enum class MasteredTargetDecision : uint8
{
    CONTINUE,
    REJECT,
    ACCEPT,
};

// ClassifyMasteredTarget mirrors validEntity's mastered-target gate.
constexpr auto ClassifyMasteredTarget(const bool hasMaster, const FIND_TYPE findType, const bool masterIsMob, const bool masterIsPlayer,
                                     const bool targetIsTrust, const bool targetIsPet) -> MasteredTargetDecision
{
    if (!hasMaster)
    {
        return MasteredTargetDecision::CONTINUE;
    }

    if (findType == FIND_TYPE::MONSTER_PLAYER)
    {
        return masterIsMob ? MasteredTargetDecision::REJECT : MasteredTargetDecision::CONTINUE;
    }

    if (findType == FIND_TYPE::PLAYER_MONSTER)
    {
        return masterIsPlayer ? MasteredTargetDecision::REJECT : MasteredTargetDecision::CONTINUE;
    }

    if (findType == FIND_TYPE::MONSTER_MONSTER || findType == FIND_TYPE::PLAYER_PLAYER)
    {
        return targetIsTrust || targetIsPet ? MasteredTargetDecision::ACCEPT : MasteredTargetDecision::REJECT;
    }

    return MasteredTargetDecision::CONTINUE;
}
} // namespace targetfindmasterhelpers
