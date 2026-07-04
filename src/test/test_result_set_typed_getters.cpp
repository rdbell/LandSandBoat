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

#include "test_result_set_typed_getters.h"

#include "common/database/libmariadb/libmariadb_result_set.h"

#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace
{

enum class TestEnum : uint16
{
    One = 1,
};

template <typename T, typename U>
auto expectEqual(const T& actual, const U& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "Result-set typed getter self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto makeTypedSchema() -> std::shared_ptr<db::ColumnSchema>
{
    auto schema = std::make_shared<db::ColumnSchema>();
    schema->names = {
        "i64",
        "u64",
        "i32",
        "u32",
        "i16",
        "u16",
        "i8",
        "u8",
        "bool_true",
        "bool_false",
        "float_col",
        "string_col",
        "size_col",
        "null_col",
    };

    for (std::size_t i = 0; i < schema->names.size(); ++i)
    {
        schema->index[schema->names[i]] = i;
    }
    return schema;
}

auto makeTypedResultSet() -> db::LibMariaDBResultSet
{
    auto row = db::LibMariaDBResultSet::Row{};
    row.emplace_back(static_cast<int64>(-42));
    row.emplace_back(static_cast<uint64>(42));
    row.emplace_back(std::string("-123"));
    row.emplace_back(std::string("4294967297"));
    row.emplace_back(std::string("-12"));
    row.emplace_back(std::string("65537"));
    row.emplace_back(std::string("-5"));
    row.emplace_back(std::string("257"));
    row.emplace_back(std::string("12"));
    row.emplace_back(std::string("0"));
    row.emplace_back(std::string("12.5x"));
    row.emplace_back(3.25);
    row.emplace_back(std::string("3"));
    row.emplace_back(db::LibMariaDBResultSet::Cell{});

    auto rows = std::vector<db::LibMariaDBResultSet::Row>{};
    rows.emplace_back(std::move(row));

    return db::LibMariaDBResultSet("SELECT typed getters", makeTypedSchema(), std::move(rows));
}

auto testTypedGetters() -> bool
{
    auto rset = makeTypedResultSet();
    bool ok = true;

    ok = expectEqual(rset.get<int64>("i64"), static_cast<int64>(0), "pre-next int64 default") && ok;
    ok = expectEqual(rset.getOrDefault<int64>("i64", static_cast<int64>(77)), static_cast<int64>(77), "pre-next default") && ok;
    ok = expectEqual(rset.next(), true, "first row available") && ok;

    ok = expectEqual(rset.get<int64>("i64"), static_cast<int64>(-42), "int64 getter") && ok;
    ok = expectEqual(rset.get<uint64>("u64"), static_cast<uint64>(42), "uint64 getter") && ok;
    ok = expectEqual(rset.get<int32>("i32"), static_cast<int32>(-123), "int32 getter") && ok;
    ok = expectEqual(rset.get<int>("u32"), static_cast<int>(1), "int getter uses int32") && ok;
    ok = expectEqual(rset.get<uint32>("u32"), static_cast<uint32>(1), "uint32 narrowing getter") && ok;
    ok = expectEqual(rset.get<int16>("i16"), static_cast<int16>(-12), "int16 getter") && ok;
    ok = expectEqual(rset.get<uint16>("u16"), static_cast<uint16>(1), "uint16 narrowing getter") && ok;
    ok = expectEqual(rset.get<int8>("i8"), static_cast<int8>(-5), "int8 getter") && ok;
    ok = expectEqual(rset.get<uint8>("u8"), static_cast<uint8>(1), "uint8 narrowing getter") && ok;
    ok = expectEqual(rset.get<bool>("bool_true"), true, "bool true getter") && ok;
    ok = expectEqual(rset.get<bool>("bool_false"), false, "bool false getter") && ok;
    ok = expectEqual(rset.get<float>("float_col"), 12.5F, "float getter") && ok;
    ok = expectEqual(rset.get<double>("float_col"), 12.5, "double getter") && ok;
    ok = expectEqual(rset.get<std::string>("string_col"), std::string("3.250000"), "string getter") && ok;
    ok = expectEqual(rset.get<size_t>("size_col"), static_cast<size_t>(3), "size_t getter uses uint32") && ok;
    ok = expectEqual(rset.get<TestEnum>("u16"), TestEnum::One, "enum getter") && ok;
    ok = expectEqual(rset.get<int64>("null_col"), static_cast<int64>(0), "null int64 default") && ok;
    ok = expectEqual(rset.get<int64>("missing_col"), static_cast<int64>(0), "missing int64 default") && ok;

    ok = expectEqual(rset.get<int64>(0), static_cast<int64>(-42), "indexed int64 getter") && ok;
    ok = expectEqual(rset.get<std::string>(11), std::string("3.250000"), "indexed string getter") && ok;
    ok = expectEqual(rset.get<int64>(99), static_cast<int64>(0), "out-of-range indexed getter") && ok;
    ok = expectEqual(rset.get<int64>(std::numeric_limits<uint32>::max()), static_cast<int64>(0), "wrapped indexed getter") && ok;

    ok = expectEqual(rset.getOrDefault<int64>("i64", static_cast<int64>(44)), static_cast<int64>(-42), "populated getOrDefault") && ok;
    ok = expectEqual(rset.getOrDefault<int64>("null_col", static_cast<int64>(44)), static_cast<int64>(44), "null getOrDefault") && ok;
    ok = expectEqual(rset.getOrDefault<std::string>("missing_col", std::string("fallback")), std::string("fallback"), "missing getOrDefault") && ok;
    ok = expectEqual(rset.getOrDefault<std::string>(11, std::string("fallback")), std::string("3.250000"), "indexed getOrDefault") && ok;
    ok = expectEqual(rset.getOrDefault<int64>(99, static_cast<int64>(44)), static_cast<int64>(44), "out-of-range indexed getOrDefault") && ok;

    return ok;
}

auto testUpdateDefaults() -> bool
{
    auto rset = db::LibMariaDBResultSet(static_cast<std::size_t>(1), "UPDATE typed getters");
    bool ok = true;

    ok = expectEqual(rset.get<int64>("id"), static_cast<int64>(0), "update get default") && ok;
    ok = expectEqual(rset.get<std::string>(0), std::string(""), "update indexed get default") && ok;
    ok = expectEqual(rset.getOrDefault<int64>("id", static_cast<int64>(55)), static_cast<int64>(55), "update getOrDefault default") && ok;
    ok = expectEqual(rset.getOrDefault<std::string>(0, std::string("fallback")), std::string("fallback"), "update indexed getOrDefault default") && ok;

    return ok;
}

} // namespace

auto runResultSetTypedGettersSelfTests() -> bool
{
    return testTypedGetters() && testUpdateDefaults();
}
