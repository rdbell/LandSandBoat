#pragma once

namespace automatoncontrollerspellpermission
{
inline auto CanCastSpells(bool silenced, bool muted, bool spellAvailable, bool ignoreRecastsAndCosts, bool canChangeState) -> bool
{
    return !silenced && !muted && (ignoreRecastsAndCosts || spellAvailable) && canChangeState;
}
} // namespace automatoncontrollerspellpermission
