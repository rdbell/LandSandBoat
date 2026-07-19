#pragma once

#include <cstdint>

namespace trustcontrollertargetsync
{
struct Plan
{
    bool changeTarget;
    bool clearTopEnmity;
};

inline auto ShouldSync(bool targetMismatch, bool masterEnmityActive, int32_t cumulativeEnmity, int32_t volatileEnmity) -> bool
{
    return targetMismatch && masterEnmityActive && cumulativeEnmity + volatileEnmity > 0;
}

inline auto Resolve(bool targetMismatch, bool masterEnmityActive, int32_t cumulativeEnmity, int32_t volatileEnmity) -> Plan
{
    const bool shouldSync = ShouldSync(targetMismatch, masterEnmityActive, cumulativeEnmity, volatileEnmity);
    return { shouldSync, shouldSync };
}

} // namespace trustcontrollertargetsync
