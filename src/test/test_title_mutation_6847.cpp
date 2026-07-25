#include "test_title_mutation_6847.h"

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
    constexpr auto lastID       = std::uint16_t{ sizeof(character.m_TitleList) * 8 - 1 };
    constexpr auto outOfRangeID = std::uint16_t{ sizeof(character.m_TitleList) * 8 };

    if (charutils::hasTitle(&character, firstID) != 0 ||
        charutils::addTitle(&character, firstID) != 1 ||
        charutils::hasTitle(&character, firstID) == 0 ||
        charutils::addTitle(&character, firstID) != 0 ||
        charutils::delTitle(&character, firstID) != 1 ||
        charutils::hasTitle(&character, firstID) != 0 ||
        charutils::delTitle(&character, firstID) != 0)
    {
        return false;
    }

    if (charutils::addTitle(&character, lastID) != 1 ||
        charutils::hasTitle(&character, lastID) == 0 ||
        charutils::delTitle(&character, lastID) != 1 ||
        charutils::addTitle(&character, outOfRangeID) != 0 ||
        charutils::delTitle(&character, outOfRangeID) != 0)
    {
        return false;
    }

    return true;
}
} // namespace

auto runTitleMutation6847SelfTests() -> bool
{
    const bool ok = Check();
    if (!ok)
    {
        std::cerr << "title mutation 6847 self-test failed\n";
    }
    return ok;
}
