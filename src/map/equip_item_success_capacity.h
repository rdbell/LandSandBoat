#pragma once

// Pure successful-equipment side-effect selection from charutils::EquipItem.

namespace equipitemsuccesshelpers
{

struct Facts
{
    bool hasEquipScript{};
    bool hasUsableCharges{};
    bool isSubSlot{};
    bool mainNeedsUnarmed{};
};

struct Plan
{
    bool setScriptEquipFlag{};
    bool assignChargeTime{};
    bool addItemRecast{};
    bool pushItemAttr{};
    bool lockItem{};
    bool checkUnarmedWeapon{};
    bool addEquipModifiers{};
    bool addLatentEffects{};
    bool checkLatentsEquip{};
    bool addPetModifiers{};
    bool onItemEquip{};
    bool queueEquipChange{};
};

// PlanFor mirrors the path after EquipArmor accepts an unlocked equipment item.
constexpr auto PlanFor(const Facts& facts) -> Plan
{
    return {
        .setScriptEquipFlag = facts.hasEquipScript,
        .assignChargeTime   = facts.hasUsableCharges,
        .addItemRecast      = facts.hasUsableCharges,
        .pushItemAttr       = facts.hasUsableCharges,
        .lockItem           = true,
        .checkUnarmedWeapon = facts.isSubSlot && facts.mainNeedsUnarmed,
        .addEquipModifiers  = true,
        .addLatentEffects   = true,
        .checkLatentsEquip  = true,
        .addPetModifiers    = true,
        .onItemEquip        = true,
        .queueEquipChange   = true,
    };
}

} // namespace equipitemsuccesshelpers
