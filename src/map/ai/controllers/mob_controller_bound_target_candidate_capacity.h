#pragma once

#include <cstdint>

namespace mobcontrollerboundtargetcandidate
{
inline auto ShouldSelect(std::int32_t currentEnmity, std::int32_t candidateEnmity, float currentDistance, float candidateDistance, bool attackable) -> bool
{
    return candidateEnmity > currentEnmity && candidateDistance < currentDistance && attackable;
}
} // namespace mobcontrollerboundtargetcandidate
