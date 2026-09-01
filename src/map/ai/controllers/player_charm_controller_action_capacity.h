#pragma once

#include <cstdint>

namespace playercharmcontrolleraction
{
// CPlayerCharmController rejects every public controller action.  Keep the
// action identity in the capacity seam so the exact constant-false policy is
// independently characterizable without constructing the entity host.
enum class Action : std::uint8_t
{
    Cast,
    ChangeTarget,
    WeaponSkill,
    Ability,
    RangedAttack,
};

constexpr auto CanDispatch(Action) -> bool
{
    return false;
}
} // namespace playercharmcontrolleraction
