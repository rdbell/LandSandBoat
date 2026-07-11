#pragma once

#include "common/cbasetypes.h"

#include <cstddef>

namespace attackloophelpers
{

constexpr std::size_t MaxActionResults = 8;

template <typename SwingCount, typename TargetAlive, typename AttackerAlive>
inline auto ShouldContinue(SwingCount&& swingCount, TargetAlive&& targetAlive, AttackerAlive&& attackerAlive) -> bool
{
    return swingCount() != 0 && targetAlive() && attackerAlive();
}

inline auto ShouldStopAtResultCap(const std::size_t resultCount) -> bool
{
    return resultCount == MaxActionResults;
}

template <typename DeleteSwing, typename ResultCount>
inline auto FinishSwing(DeleteSwing&& deleteSwing, ResultCount&& resultCount) -> bool
{
    deleteSwing();
    return ShouldStopAtResultCap(resultCount());
}

template <typename AttackListener, typename AttackedListener, typename StripDetectable, typename ProcessActionFlags>
inline void ApplyFinalization(
    AttackListener&& attackListener,
    AttackedListener&& attackedListener,
    StripDetectable&& stripDetectable,
    ProcessActionFlags&& processActionFlags)
{
    attackListener();
    attackedListener();
    stripDetectable();
    processActionFlags();
}

} // namespace attackloophelpers
