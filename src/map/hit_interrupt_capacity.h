#pragma once

namespace hitinterrupthelpers
{

template <typename State, typename Attacker>
inline void Route(State* currentState, Attacker* attacker)
{
    if (currentState)
    {
        currentState->TryInterrupt(attacker);
    }
}

} // namespace hitinterrupthelpers
