/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_data_enum_traits.h"

#include "map/data/enums/enum_traits.h"

#include <array>
#include <cstdint>
#include <exception>
#include <iostream>
#include <span>
#include <string>
#include <string_view>
#include <utility>

namespace
{

enum class TestCreature : std::uint8_t
{
    Rabbit = 1,
    Mandragora,
    Dhalmel,
};

enum class TestCreatureFlags : std::uint8_t
{
    None  = 0,
    Small = 1,
};

constexpr auto creatureEntries = std::array<std::pair<std::string_view, TestCreature>, 5>{
    std::pair<std::string_view, TestCreature>{ "rabbit", TestCreature::Rabbit },
    { "mandragora", TestCreature::Mandragora },
    { "dhalmel", TestCreature::Dhalmel },
    { "rabbit-alias", TestCreature::Rabbit },
    { "mandragora", TestCreature::Dhalmel },
};

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "data enum traits self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto expectFalse(bool actual, const std::string& label) -> bool
{
    if (actual)
    {
        std::cerr << "data enum traits self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

template <typename T>
auto expectEqual(const T& actual, const T& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "data enum traits self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto expectString(std::string_view actual, std::string_view expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "data enum traits self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }

    return true;
}

auto testFromName() -> bool
{
    bool ok = true;

    ok = expectEqual(xi::data::enum_detail::fromName<TestCreature>("rabbit", creatureEntries, "creature"), TestCreature::Rabbit, "fromName rabbit") && ok;
    ok = expectEqual(xi::data::enum_detail::fromName<TestCreature>("mandragora", creatureEntries, "creature"), TestCreature::Mandragora, "fromName mandragora") && ok;
    ok = expectEqual(xi::data::enum_detail::fromName<TestCreature>("rabbit-alias", creatureEntries, "creature"), TestCreature::Rabbit, "fromName alias") && ok;

    try
    {
        (void)xi::data::enum_detail::fromName<TestCreature>("griffin", creatureEntries, "creature");
        ok = expectTrue(false, "fromName missing value throws") && ok;
    }
    catch (const std::exception& e)
    {
        ok = expectString(e.what(), "'griffin' is not a valid creature enum value", "fromName missing error") && ok;
    }

    return ok;
}

auto testToName() -> bool
{
    bool ok = true;

    ok = expectString(xi::data::enum_detail::toName<TestCreature>(TestCreature::Rabbit, creatureEntries), "rabbit", "toName first matching value") && ok;
    ok = expectString(xi::data::enum_detail::toName<TestCreature>(TestCreature::Mandragora, creatureEntries), "mandragora", "toName mandragora") && ok;
    ok = expectString(xi::data::enum_detail::toName<TestCreature>(TestCreature::Dhalmel, creatureEntries), "dhalmel", "toName dhalmel") && ok;
    ok = expectString(xi::data::enum_detail::toName<TestCreature>(static_cast<TestCreature>(99), creatureEntries), "<unknown>", "toName unknown") && ok;

    return ok;
}

} // namespace

namespace xi::data
{

template <>
struct EnumTraits<TestCreature>
{
    static constexpr auto kTypeName = std::string_view{ "creature" };

    static auto fromName(const std::string_view name) -> TestCreature
    {
        return enum_detail::fromName<TestCreature>(name, creatureEntries, kTypeName);
    }

    static auto toName(const TestCreature value) -> std::string_view
    {
        return enum_detail::toName<TestCreature>(value, creatureEntries);
    }
};

template <>
inline constexpr bool isFlagEnum<TestCreatureFlags> = true;

} // namespace xi::data

auto runDataEnumTraitsSelfTests() -> bool
{
    bool ok = true;

    ok = testFromName() && ok;
    ok = testToName() && ok;
    ok = expectTrue(xi::data::Nameable<TestCreature>, "Nameable concept accepts specialized enum") && ok;
    ok = expectFalse(xi::data::isFlagEnum<TestCreature>, "isFlagEnum defaults false") && ok;
    ok = expectTrue(xi::data::isFlagEnum<TestCreatureFlags>, "isFlagEnum specialization") && ok;

    return ok;
}
