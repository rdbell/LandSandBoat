#pragma once

#include <cstdint>

namespace automatoncontrollerspelladmission
{
inline auto CanUseSpell(uint16 magicSkill, uint16 requiredSkill, uint8 head, uint8 headMask) -> bool
{
    return magicSkill >= requiredSkill && (headMask & (1 << (head - 1)));
}
} // namespace automatoncontrollerspelladmission
