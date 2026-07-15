#pragma once

#include <cstdint>

// Pure CBattleEntity addModifier / setModifier / delModifier amount math.
// Parity: internal/modstat (slice 1676).
//
// Reference: src/map/entities/battle_entity.cpp
//   CBattleEntity::addModifier (~1811–1817)
//   CBattleEntity::setModifier (~1920–1926)
//   CBattleEntity::delModifier (~1947–1953)
//
// Host retains m_modStat map storage and multi-modifier list loops.
// Helpers take injected current value, amount, and mod type (as uint16 so
// this header stays free of the full Mod enum include).

namespace modstathelpers
{

// ModNone is Mod::NONE — essential sentinel that does nothing.
constexpr std::uint16_t ModNone = 0;

// ShouldApplyMod reports whether a mod type may touch m_modStat.
// LSB: type != Mod::NONE.
inline auto ShouldApplyMod(const std::uint16_t modType) -> bool
{
    return modType != ModNone;
}

// ApplyAdd mirrors CBattleEntity::addModifier:
//   if type != NONE { m_modStat[type] += amount }
// Returns the new stored value; NONE leaves current unchanged.
inline auto ApplyAdd(const std::int16_t current, const std::int16_t amount, const std::uint16_t modType) -> std::int16_t
{
    if (!ShouldApplyMod(modType))
    {
        return current;
    }
    return static_cast<std::int16_t>(current + amount);
}

// ApplySet mirrors CBattleEntity::setModifier:
//   if type != NONE { m_modStat[type] = amount }
// Returns the new stored value; NONE leaves current unchanged.
inline auto ApplySet(const std::int16_t current, const std::int16_t amount, const std::uint16_t modType) -> std::int16_t
{
    if (!ShouldApplyMod(modType))
    {
        return current;
    }
    return amount;
}

// ApplyDel mirrors CBattleEntity::delModifier:
//   if type != NONE { m_modStat[type] -= amount }
// Returns the new stored value; NONE leaves current unchanged.
inline auto ApplyDel(const std::int16_t current, const std::int16_t amount, const std::uint16_t modType) -> std::int16_t
{
    if (!ShouldApplyMod(modType))
    {
        return current;
    }
    return static_cast<std::int16_t>(current - amount);
}

} // namespace modstathelpers
