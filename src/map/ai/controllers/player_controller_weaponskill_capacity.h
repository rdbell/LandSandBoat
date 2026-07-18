#pragma once

#include <cstdint>

namespace playercontrollerweaponskill
{
enum class Error { None, Unable, CannotUse, CannotUseAny, NotEnoughTP, NoRangedWeapon, CannotSee };
struct Decision { bool dispatch; Error error; };

constexpr auto Evaluate(
    const bool canAct, const bool canChangeState, const bool exists, const bool known, const bool usable,
    const bool amnesia, const uint8_t impairmentPower, const uint16_t tp, const bool rangedSkill,
    const bool rangedGearReady, const bool targetValid, const bool targetUntargetable,
    const bool facingTarget, const bool selfTarget) -> Decision
{
    if (!canAct || !canChangeState) return { false, Error::Unable };
    if (!exists || !known || !usable) return { false, Error::CannotUse };
    if (amnesia || impairmentPower == 2 || impairmentPower == 3) return { false, Error::CannotUseAny };
    if (tp < 1000) return { false, Error::NotEnoughTP };
    if (rangedSkill && !rangedGearReady) return { false, Error::NoRangedWeapon };
    if (!targetValid || targetUntargetable) return { false, Error::None };
    if (!selfTarget && !facingTarget) return { false, Error::CannotSee };
    return { true, Error::None };
}
} // namespace playercontrollerweaponskill
