#include "test_trait_mutation_6848.h"

#include "map/entities/char_entity.h"
#include "map/utils/charutils.h"

#include <cstdint>
#include <iostream>

namespace
{
auto Check() -> bool
{
    auto character = CCharEntity{};

    constexpr auto firstID      = std::uint16_t{ 0 };
    constexpr auto lastID       = std::uint16_t{ sizeof(character.m_TraitList) * 8 - 1 };
    constexpr auto outOfRangeID = std::uint16_t{ sizeof(character.m_TraitList) * 8 };

    if (charutils::hasTrait(&character, firstID) != 0 ||
        charutils::addTrait(&character, firstID) != 1 ||
        charutils::hasTrait(&character, firstID) == 0 ||
        charutils::addTrait(&character, firstID) != 0 ||
        charutils::delTrait(&character, firstID) != 1 ||
        charutils::hasTrait(&character, firstID) != 0 ||
        charutils::delTrait(&character, firstID) != 0)
    {
        return false;
    }

    if (charutils::addTrait(&character, lastID) != 1 ||
        charutils::hasTrait(&character, lastID) == 0 ||
        charutils::delTrait(&character, lastID) != 1 ||
        charutils::addTrait(&character, outOfRangeID) != 0 ||
        charutils::delTrait(&character, outOfRangeID) != 0)
    {
        return false;
    }

    character.objtype = TYPE_MOB;
    return charutils::hasTrait(&character, firstID) == 0 &&
           charutils::addTrait(&character, firstID) == 0 &&
           charutils::delTrait(&character, firstID) == 0;
}
} // namespace

auto runTraitMutation6848SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "trait mutation 6848 self-test failed\n";
    }
    return ok;
}
