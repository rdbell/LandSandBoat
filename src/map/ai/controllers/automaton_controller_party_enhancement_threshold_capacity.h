#pragma once

#include <cstddef>

namespace automatoncontrollerpartyenhancementthreshold
{
inline auto CanCastPartyEnhancement(size_t memberCount, size_t effectCount) -> bool
{
    return memberCount - effectCount >= 4;
}
} // namespace automatoncontrollerpartyenhancementthreshold
