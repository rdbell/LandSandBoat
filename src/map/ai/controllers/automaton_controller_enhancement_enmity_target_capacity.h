#pragma once

#include <cstdint>

namespace automatoncontrollerenhancementenmitytarget
{
inline auto CanSelectHigherEnmity(uint32 current, uint32 candidate) -> bool
{
    return current < candidate;
}
} // namespace automatoncontrollerenhancementenmitytarget
