#pragma once

// Pure accepted sub-slot mutation plan from charutils::EquipArmor.

namespace equiparmorsubhelpers
{

enum class MainKind
{
    HandToHand,
    OneHanded,
    Other,
};

struct Facts
{
    MainKind mainKind{};
    bool     incomingIsWeapon{};
};

struct Plan
{
    bool unequipMain{};
    bool setSubWeapon{};
    bool setDualWield{};
};

// PlanFor mirrors the post-validation mutations in the SLOT_SUB branch.
constexpr auto PlanFor(const Facts& facts) -> Plan
{
    switch (facts.mainKind)
    {
        case MainKind::OneHanded:
            return { .setSubWeapon = facts.incomingIsWeapon, .setDualWield = facts.incomingIsWeapon };
        case MainKind::HandToHand:
        case MainKind::Other:
            return { .unequipMain = !facts.incomingIsWeapon };
    }
    return {};
}

} // namespace equiparmorsubhelpers
