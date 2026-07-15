#pragma once

#include <cstdint>

// Pure CBattleEntity::IsDualWielding with fully injected inputs.
// Parity: internal/dualwield (slice 1662).
//
// Reference: src/map/entities/battle_entity.cpp
//   CBattleEntity::IsDualWielding (~146–154)
//
// Host retains objtype, m_dualWield, and mobmod storage; helpers take injected
// values only. isMob is the projected (objtype == TYPE_MOB) gate.

namespace dualwieldhelpers
{

// IsDualWielding mirrors CBattleEntity::IsDualWielding:
//   if objtype == TYPE_MOB:
//     return getMobMod(MOBMOD_DUAL_WIELD) != 0
//   return m_dualWield
//
// isMob is (objtype == TYPE_MOB).
// dualWieldFlag is m_dualWield (used only when !isMob).
// mobDualWieldMod is getMobMod(MOBMOD_DUAL_WIELD) (used only when isMob).
// Non-zero mob mod is true (negative values count).
inline auto IsDualWielding(const bool          isMob,
                           const bool          dualWieldFlag,
                           const std::int16_t  mobDualWieldMod) -> bool
{
    if (isMob)
    {
        return mobDualWieldMod != 0;
    }
    return dualWieldFlag;
}

} // namespace dualwieldhelpers
