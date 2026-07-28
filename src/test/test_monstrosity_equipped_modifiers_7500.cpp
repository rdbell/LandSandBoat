#include "test_monstrosity_equipped_modifiers_7500.h"

#include "map/monstrosity.h"

#include <iostream>

auto runMonstrosityEquippedModifiers7500SelfTests() -> bool
{
    using monstrosity::InstinctCatalog;
    using monstrosity::InstinctCatalogRow;
    using monstrosity::ResolveEquippedInstinctModifiers;

    auto catalog = InstinctCatalog{
        { 10, { .monstrosityInstinctId = 10, .mods = { CModifier{ Mod::HP, 1 }, CModifier{ Mod::STR, 2 } } } },
        { 11, { .monstrosityInstinctId = 11, .mods = { CModifier{ Mod::ATT, -3 } } } },
    };
    std::array<uint16, 12> equipped{};
    equipped[0] = 10;
    equipped[1] = 99;
    equipped[2] = 11;
    equipped[3] = 10;

    const auto modifiers = ResolveEquippedInstinctModifiers(catalog, equipped);
    if (modifiers.size() != 5 || modifiers[0].getModID() != Mod::HP || modifiers[0].getModAmount() != 1 || modifiers[1].getModID() != Mod::STR || modifiers[1].getModAmount() != 2 || modifiers[2].getModID() != Mod::ATT || modifiers[2].getModAmount() != -3 || modifiers[3].getModID() != Mod::HP || modifiers[3].getModAmount() != 1 || modifiers[4].getModID() != Mod::STR || modifiers[4].getModAmount() != 2)
    {
        std::cerr << "monstrosity equipped modifiers: slot and modifier order failed\n";
        return false;
    }
    if (catalog.contains(0) || catalog.contains(99))
    {
        std::cerr << "monstrosity equipped modifiers: missing IDs should not mutate the catalog\n";
        return false;
    }
    return true;
}
