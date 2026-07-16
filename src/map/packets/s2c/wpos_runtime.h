#pragma once

#include "0x05b_wpos.h"

namespace wposhelpers
{

struct State
{
    position_t current{};
    bool       character{};
    bool       locked{};
};

[[nodiscard]] inline auto PlanFor(State s, const position_t requested, const POSMODE mode) -> State
{
    switch (mode)
    {
        case POSMODE::NORMAL:
        case POSMODE::EVENT:
        case POSMODE::POP:
        case POSMODE::RESET:
        case POSMODE::MATERIALIZE:
            s.current = requested;
            if (s.character && mode == POSMODE::RESET)
                s.locked = false;
            break;
        case POSMODE::ROTATE:
            s.current.rotation = requested.rotation;
            break;
        case POSMODE::LOCK:
            if (s.character)
                s.locked = true;
            break;
        case POSMODE::UNLOCK:
            if (s.character)
                s.locked = false;
            break;
        default:
            break;
    }
    return s;
}

} // namespace wposhelpers
