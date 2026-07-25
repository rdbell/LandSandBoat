#pragma once

// Pure ammo fallback appearance rule from charutils::EquipArmor.

namespace equiparmorammolookhelpers
{

struct Facts
{
    bool incomingIsWeapon{};
    bool hasRangedAfterCompatibility{};
};

// ShouldSetRangedLook mirrors weapon ammo supplying the ranged appearance.
constexpr auto ShouldSetRangedLook(const Facts& facts) -> bool
{
    return facts.incomingIsWeapon && !facts.hasRangedAfterCompatibility;
}

} // namespace equiparmorammolookhelpers
