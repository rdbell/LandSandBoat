#pragma once

#include "common/timer.h"

namespace spawnremainingrespawn
{

inline auto clamp(const timer::duration remaining) -> timer::duration
{
    return remaining > timer::duration::zero() ? remaining : timer::duration::zero();
}

} // namespace spawnremainingrespawn
