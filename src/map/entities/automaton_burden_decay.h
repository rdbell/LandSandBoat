#pragma once

#include "common/cbasetypes.h"

#include <algorithm>
#include <array>

namespace automatonburdenhelpers
{

// Mirrors CAutomatonEntity::burdenTick. The explicit uint8 clamp conversion
// is intentional: the summed signed modifiers are converted before clamping
// against the current burden.
inline void Decay(std::array<uint8, 8>& burdens, const int16 masterDecayMod, const int16 automatonDecayMod)
{
    for (auto& burden : burdens)
    {
        if (burden > 0)
        {
            burden -= std::clamp<uint8>(1 + masterDecayMod + automatonDecayMod, 1, burden);
        }
    }
}

} // namespace automatonburdenhelpers
