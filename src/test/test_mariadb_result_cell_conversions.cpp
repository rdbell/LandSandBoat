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

#include "test_mariadb_result_cell_conversions.h"

#include "common/database/libmariadb/libmariadb_result_set.h"

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace
{

template <typename T, typename U>
auto expectEqual(const T& actual, const U& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "MariaDB result cell conversion self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto makeSchema() -> std::shared_ptr<db::ColumnSchema>
{
    auto schema = std::make_shared<db::ColumnSchema>();
    schema->names = {
        "null_col",
        "int_col",
        "uint_col",
        "double_col",
        "text_int",
        "text_float",
        "text_invalid",
        "text_neg_one",
        "text_hex",
        "text_inf",
        "text_overflow",
        "text_nan",
    };

    for (std::size_t i = 0; i < schema->names.size(); ++i)
    {
        schema->index[schema->names[i]] = i;
    }
    return schema;
}

auto makeResultSet() -> db::LibMariaDBResultSet
{
    auto row = db::LibMariaDBResultSet::Row{};
    row.emplace_back(db::LibMariaDBResultSet::Cell{});
    row.emplace_back(static_cast<int64>(-42));
    row.emplace_back(static_cast<uint64>(42));
    row.emplace_back(3.25);
    row.emplace_back(std::string("  -123xyz"));
    row.emplace_back(std::string("12.75xyz"));
    row.emplace_back(std::string("abc123"));
    row.emplace_back(std::string("-1"));
    row.emplace_back(std::string("0x1.8p2tail"));
    row.emplace_back(std::string("infinitetail"));
    row.emplace_back(std::string("1e9999tail"));
    row.emplace_back(std::string("nan(payload)tail"));

    auto rows = std::vector<db::LibMariaDBResultSet::Row>{};
    rows.emplace_back(std::move(row));

    return db::LibMariaDBResultSet("SELECT cell conversions", makeSchema(), std::move(rows));
}

auto testCellConversions() -> bool
{
    auto rset = makeResultSet();
    bool ok = true;

    ok = expectEqual(rset.next(), true, "first row available") && ok;

    ok = expectEqual(rset.isNull("null_col"), true, "null cell is null") && ok;
    ok = expectEqual(rset.get<int64>("null_col"), static_cast<int64>(0), "null to int64") && ok;
    ok = expectEqual(rset.get<uint64>("null_col"), static_cast<uint64>(0), "null to uint64") && ok;
    ok = expectEqual(rset.get<double>("null_col"), 0.0, "null to double") && ok;
    ok = expectEqual(rset.get<std::string>("null_col"), std::string(""), "null to string") && ok;

    ok = expectEqual(rset.get<int64>("int_col"), static_cast<int64>(-42), "int64 to int64") && ok;
    ok = expectEqual(rset.get<uint64>("int_col"), std::numeric_limits<uint64>::max() - 41, "int64 to uint64") && ok;
    ok = expectEqual(rset.get<double>("int_col"), -42.0, "int64 to double") && ok;
    ok = expectEqual(rset.get<std::string>("int_col"), std::string("-42"), "int64 to string") && ok;
    ok = expectEqual(rset.get<bool>("int_col"), true, "int64 to bool") && ok;

    ok = expectEqual(rset.get<int64>("uint_col"), static_cast<int64>(42), "uint64 to int64") && ok;
    ok = expectEqual(rset.get<uint64>("uint_col"), static_cast<uint64>(42), "uint64 to uint64") && ok;
    ok = expectEqual(rset.get<double>("uint_col"), 42.0, "uint64 to double") && ok;
    ok = expectEqual(rset.get<std::string>("uint_col"), std::string("42"), "uint64 to string") && ok;

    ok = expectEqual(rset.get<int64>("double_col"), static_cast<int64>(3), "double to int64") && ok;
    ok = expectEqual(rset.get<uint64>("double_col"), static_cast<uint64>(3), "double to uint64") && ok;
    ok = expectEqual(rset.get<double>("double_col"), 3.25, "double to double") && ok;
    ok = expectEqual(rset.get<std::string>("double_col"), std::string("3.250000"), "double to string") && ok;

    ok = expectEqual(rset.get<int64>("text_int"), static_cast<int64>(-123), "text integer prefix to int64") && ok;
    ok = expectEqual(rset.get<uint64>("text_int"), std::numeric_limits<uint64>::max() - 122, "text integer prefix to uint64") && ok;
    ok = expectEqual(rset.get<double>("text_int"), -123.0, "text integer prefix to double") && ok;
    ok = expectEqual(rset.get<std::string>("text_int"), std::string("  -123xyz"), "text integer to string") && ok;

    ok = expectEqual(rset.get<int64>("text_float"), static_cast<int64>(12), "text float prefix to int64") && ok;
    ok = expectEqual(rset.get<uint64>("text_float"), static_cast<uint64>(12), "text float prefix to uint64") && ok;
    ok = expectEqual(rset.get<double>("text_float"), 12.75, "text float prefix to double") && ok;

    ok = expectEqual(rset.get<double>("text_hex"), 6.0, "text hex float prefix to double") && ok;
    ok = expectEqual(std::isinf(rset.get<double>("text_inf")) && rset.get<double>("text_inf") > 0.0, true, "text infinity prefix to double") && ok;
    ok = expectEqual(std::isinf(rset.get<double>("text_overflow")) && rset.get<double>("text_overflow") > 0.0, true, "text overflow to double") && ok;
    ok = expectEqual(std::isnan(rset.get<double>("text_nan")), true, "text NaN prefix to double") && ok;

    ok = expectEqual(rset.get<int64>("text_invalid"), static_cast<int64>(0), "invalid text to int64") && ok;
    ok = expectEqual(rset.get<uint64>("text_invalid"), static_cast<uint64>(0), "invalid text to uint64") && ok;
    ok = expectEqual(rset.get<double>("text_invalid"), 0.0, "invalid text to double") && ok;
    ok = expectEqual(rset.get<bool>("text_invalid"), false, "invalid text to bool") && ok;

    ok = expectEqual(rset.get<int64>("text_neg_one"), static_cast<int64>(-1), "negative text to int64") && ok;
    ok = expectEqual(rset.get<uint64>("text_neg_one"), std::numeric_limits<uint64>::max(), "negative text to uint64") && ok;
    ok = expectEqual(rset.get<double>("text_neg_one"), -1.0, "negative text to double") && ok;

    ok = expectEqual(rset.get<int64>("missing_col"), static_cast<int64>(0), "missing column to int64") && ok;
    ok = expectEqual(rset.next(), false, "no second row") && ok;

    return ok;
}

} // namespace

auto runMariaDBResultCellConversionsSelfTests() -> bool
{
    return testCellConversions();
}
