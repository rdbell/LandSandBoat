#pragma once

#include <cstdint>

// Pure getShieldSize / getShieldDefense policy from CCharEntity. Host injects
// sub-slot equipment presence, IsShield, size, and DEF modifier.

namespace charshieldhelpers
{

// ShieldSize mirrors null/non-shield → 0, else item shield size (signed return).
constexpr auto ShieldSize(const bool hasSubItem, const bool isShield, const std::int8_t size) -> std::int8_t
{
    if (!hasSubItem || !isShield)
    {
        return 0;
    }
    return size;
}

// ShieldDefense mirrors has shield → Mod::DEF, else 0.
constexpr auto ShieldDefense(const bool hasSubItem, const bool isShield, const std::int16_t defense) -> std::int16_t
{
    if (hasSubItem && isShield)
    {
        return defense;
    }
    return 0;
}

} // namespace charshieldhelpers
