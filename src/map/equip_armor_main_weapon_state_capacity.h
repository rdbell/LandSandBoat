#pragma once

namespace equiparmormainweaponstatehelpers
{
struct Plan
{
    bool setMainWeapon{};
};

constexpr Plan PlanFor(const bool incomingIsWeapon)
{
    return { .setMainWeapon = incomingIsWeapon };
}
} // namespace equiparmormainweaponstatehelpers
