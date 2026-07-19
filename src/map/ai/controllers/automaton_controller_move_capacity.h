#pragma once

#include <cstdint>

namespace automatoncontrollermove
{
inline auto ShouldClearStandBack(bool shouldStandBack, bool within15, uint16_t mp, uint16_t maxMP) -> bool
{
    return (shouldStandBack && !within15) || (mp < 8 && maxMP > 8);
}
} // namespace automatoncontrollermove
