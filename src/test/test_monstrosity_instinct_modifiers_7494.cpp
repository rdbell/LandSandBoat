#include "test_monstrosity_instinct_modifiers_7494.h"

#include "map/monstrosity.h"

#include <iostream>
#include <vector>

auto runMonstrosityInstinctModifiers7494SelfTests() -> bool
{
    using monstrosity::ApplyInstinctModifierRows;
    using monstrosity::InstinctCatalog;
    using monstrosity::InstinctCatalogRow;
    using monstrosity::InstinctModifierRow;

    auto catalog = InstinctCatalog{
        { 40, { .monstrosityInstinctId = 40, .mods = { CModifier{ Mod::HP, 1 } } } },
        { 41, { .monstrosityInstinctId = 41 } },
    };
    ApplyInstinctModifierRows(catalog, std::vector<InstinctModifierRow>{
                                           { .monstrosityInstinctId = 40, .mod = Mod::STR, .value = 2 },
                                           { .monstrosityInstinctId = 40, .mod = Mod::ATT, .value = -3 },
                                           { .monstrosityInstinctId = 99, .mod = Mod::HP, .value = 42 },
                                           { .monstrosityInstinctId = 41, .mod = Mod::HPP, .value = 4 },
                                       });

    const auto& existing = catalog.at(40).mods;
    if (existing.size() != 3 || existing[0].getModID() != Mod::HP || existing[0].getModAmount() != 1 || existing[1].getModID() != Mod::STR || existing[1].getModAmount() != 2 || existing[2].getModID() != Mod::ATT || existing[2].getModAmount() != -3)
    {
        std::cerr << "monstrosity instinct modifiers: rows should append in order\n";
        return false;
    }
    const auto& second = catalog.at(41).mods;
    if (second.size() != 1 || second[0].getModID() != Mod::HPP || second[0].getModAmount() != 4 || catalog.contains(99))
    {
        std::cerr << "monstrosity instinct modifiers: absent instincts should remain absent\n";
        return false;
    }

    return true;
}
