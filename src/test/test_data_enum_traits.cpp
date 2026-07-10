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
#include "map/data/backends/yaml.h"
#include "map/data/load.h"

#include <array>
#include <cstdint>
#include <exception>
#include <iostream>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

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

struct TestDataRecord
{
    std::uint16_t Id{};
    std::string   Name{};
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

auto testYAMLNodeAndMerge() -> bool
{
    using Backend = xi::data::backends::YAMLBackend;
    using Node    = xi::data::Node<Backend>;

    Backend::Tree core{ R"(
items:
  1:
    name: original
    nested:
      keep: 17
      list: [core-a, core-b]
)" };
    Backend::Tree module{ R"(
items:
  1:
    name: patched
    nested:
      added: true
      list: [module-only]
  2: {name: added}
)" };
    Backend::mergeInto(core.root(), module.root());

    const Node root{ core.root() };
    const auto one    = root.child("items").child("1");
    const auto nested = one.child("nested");
    bool       ok     = true;
    ok = expectString(one.read<std::string>("name"), "patched", "YAML scalar read") && ok;
    ok = expectEqual(nested.read<int>("keep"), 17, "recursive map merge retains core") && ok;
    ok = expectTrue(nested.read<bool>("added"), "recursive map merge adds module value") && ok;
    ok = expectEqual(nested.child("list").children().size(), std::size_t{ 1 }, "nested sequence replaces core sequence") && ok;
    ok = expectString(nested.child("list").children().front().as<std::string>(), "module-only", "replacement sequence value") && ok;
    ok = expectTrue(root.child("items").has("2"), "recursive map merge adds record") && ok;
    ok = expectEqual(one.read<int>("missing"), 0, "missing field returns zero") && ok;
    return ok;
}

auto testYAMLPopulationFailures() -> bool
{
    using Backend = xi::data::backends::YAMLBackend;
    using Node    = xi::data::Node<Backend>;

    const auto populate = [](const Node child, const std::uint16_t id) {
        return TestDataRecord{ .Id = id, .Name = child.read<std::string>("name") };
    };

    bool ok = true;
    {
        Backend::Tree tree{ "records: {16555: {name: ridill}}" };
        const auto result = xi::data::populateMapDriver<TestDataRecord>(Node{ tree.root() }, "records", xi::data::IdSource::YAMLKey, "numeric.yaml", populate);
        ok = expectEqual(result.at(16555).Id, std::uint16_t{ 16555 }, "numeric YAML key ID") && ok;
        ok = expectString(result.at(16555).Name, "ridill", "numeric YAML key record") && ok;
    }

    try
    {
        Backend::Tree tree{ "records: {rabbit: {name: rabbit}}" };
        (void)xi::data::populateMapDriver<TestDataRecord>(Node{ tree.root() }, "records", xi::data::IdSource::YAMLKey, "bad-key.yaml", populate);
        ok = expectTrue(false, "malformed YAML key throws") && ok;
    }
    catch (const std::exception&)
    {
    }

    try
    {
        Backend::Tree tree{ "records: {first: {id: 4}, second: {id: 4}}" };
        (void)xi::data::populateMapDriver<TestDataRecord>(Node{ tree.root() }, "records", xi::data::IdSource::YAMLField, "duplicate.yaml", populate);
        ok = expectTrue(false, "duplicate YAML ID throws") && ok;
    }
    catch (const std::exception&)
    {
    }

    try
    {
        Backend::Tree tree{ "items: [one, two" };
        (void)tree;
        ok = expectTrue(false, "malformed YAML throws") && ok;
    }
    catch (const std::exception&)
    {
    }

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
    ok = testYAMLNodeAndMerge() && ok;
    ok = testYAMLPopulationFailures() && ok;
    ok = expectTrue(xi::data::Nameable<TestCreature>, "Nameable concept accepts specialized enum") && ok;
    ok = expectFalse(xi::data::isFlagEnum<TestCreature>, "isFlagEnum defaults false") && ok;
    ok = expectTrue(xi::data::isFlagEnum<TestCreatureFlags>, "isFlagEnum specialization") && ok;

    return ok;
}
