#pragma once

#include <cstdint>

namespace trustcontrollertargetsync
{

inline auto ShouldSync(bool targetMismatch, bool masterEnmityActive, int32_t cumulativeEnmity, int32_t volatileEnmity) -> bool
{
    return targetMismatch && masterEnmityActive && cumulativeEnmity + volatileEnmity > 0;
}

} // namespace trustcontrollertargetsync
