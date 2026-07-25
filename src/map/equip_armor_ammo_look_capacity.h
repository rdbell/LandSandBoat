#pragma once

#include <cstdint>

// Pure ammo fallback appearance rule from charutils::EquipArmor.

namespace equiparmorammolookhelpers
{

struct Facts
{
    bool incomingIsWeapon{};
    bool hasRangedAfterCompatibility{};
    std::uint16_t modelID{};
};

// ShouldSetRangedLook mirrors weapon ammo supplying the ranged appearance.
constexpr auto ShouldSetRangedLook(const Facts& facts) -> bool
{
    return facts.incomingIsWeapon && !facts.hasRangedAfterCompatibility;
}

struct Plan
{
    bool          setRangedLook = false;
    std::uint16_t modelID       = 0;
};

// PlanFor mirrors weapon ammo supplying the ranged appearance model.
constexpr auto PlanFor(const Facts& facts) -> Plan
{
    if (!ShouldSetRangedLook(facts))
    {
        return {};
    }
    return {
        .setRangedLook = true,
        .modelID       = facts.modelID,
    };
}

} // namespace equiparmorammolookhelpers
