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

#include "test_mariadb_result_set_metadata.h"

#include "common/database/libmariadb/libmariadb_result_set.h"

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
        std::cerr << "MariaDB result-set metadata self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto makeSchema() -> std::shared_ptr<db::ColumnSchema>
{
    auto schema = std::make_shared<db::ColumnSchema>();
    schema->names = {
        "id",
        "blob_col",
        "null_col",
        "dup",
        "dup",
    };

    for (std::size_t i = 0; i < schema->names.size(); ++i)
    {
        schema->index[schema->names[i]] = i;
    }
    return schema;
}

auto makeSelectResultSet() -> db::LibMariaDBResultSet
{
    auto first = db::LibMariaDBResultSet::Row{};
    first.emplace_back(static_cast<int64>(7));
    first.emplace_back(std::string("abc\0def", 7));
    first.emplace_back(db::LibMariaDBResultSet::Cell{});
    first.emplace_back(static_cast<int64>(1));
    first.emplace_back(static_cast<int64>(2));

    auto second = db::LibMariaDBResultSet::Row{};
    second.emplace_back(static_cast<uint64>(9));
    second.emplace_back(std::string("z"));
    second.emplace_back(std::string("filled"));
    second.emplace_back(static_cast<int64>(3));
    second.emplace_back(db::LibMariaDBResultSet::Cell{});

    auto rows = std::vector<db::LibMariaDBResultSet::Row>{};
    rows.emplace_back(std::move(first));
    rows.emplace_back(std::move(second));

    return db::LibMariaDBResultSet("SELECT result set metadata", makeSchema(), std::move(rows));
}

auto testSelectMetadata() -> bool
{
    auto rset = makeSelectResultSet();
    bool ok = true;

    ok = expectEqual(rset.type(), db::ResultSetType::Select, "select type") && ok;
    ok = expectEqual(rset.query(), std::string("SELECT result set metadata"), "select query") && ok;
    ok = expectEqual(rset.rowsCount(), static_cast<uint32>(2), "select row count") && ok;
    ok = expectEqual(rset.rowsAffected(), static_cast<uint32>(0), "select affected rows default") && ok;
    ok = expectEqual(rset.columnCount(), static_cast<uint32>(5), "select column count") && ok;
    ok = expectEqual(rset.columnName(0), std::string("id"), "first column name") && ok;
    ok = expectEqual(rset.columnName(1), std::string("blob_col"), "second column name") && ok;
    ok = expectEqual(rset.columnName(5), std::string(""), "out-of-range column name") && ok;
    ok = expectEqual(rset.columnName(std::numeric_limits<uint32>::max()), std::string(""), "wrapped column name") && ok;

    ok = expectEqual(rset.isNull("id"), true, "pre-next cell is null") && ok;

    ok = expectEqual(rset.next(), true, "first row available") && ok;
    ok = expectEqual(rset.isNull("id"), false, "id cell is populated") && ok;
    ok = expectEqual(rset.isNull("null_col"), true, "null cell is null") && ok;
    ok = expectEqual(rset.isNull("missing_col"), true, "missing column is null") && ok;
    ok = expectEqual(rset.getBlobBytes("blob_col"), std::string("abc\0def", 7), "blob bytes preserve embedded null") && ok;
    ok = expectEqual(rset.getBlobBytes("dup"), std::string("2"), "duplicate column resolves to last index") && ok;

    ok = expectEqual(rset.next(), true, "second row available") && ok;
    ok = expectEqual(rset.isNull("dup"), true, "second-row duplicate cell is null") && ok;

    ok = expectEqual(rset.next(), false, "no third row") && ok;
    ok = expectEqual(rset.isNull("id"), true, "post-exhaustion cell is null") && ok;
    ok = expectEqual(rset.getBlobBytes("id"), std::string(""), "post-exhaustion blob bytes are empty") && ok;

    return ok;
}

auto testUpdateMetadata() -> bool
{
    auto rset = db::LibMariaDBResultSet(static_cast<std::size_t>(std::numeric_limits<uint32>::max()) + 4U, "UPDATE result set metadata");
    bool ok = true;

    ok = expectEqual(rset.type(), db::ResultSetType::Update, "update type") && ok;
    ok = expectEqual(rset.query(), std::string("UPDATE result set metadata"), "update query") && ok;
    ok = expectEqual(rset.rowsAffected(), static_cast<uint32>(3), "update affected rows truncation") && ok;
    ok = expectEqual(rset.next(), false, "update next default") && ok;
    ok = expectEqual(rset.rowsCount(), static_cast<uint32>(0), "update row count default") && ok;
    ok = expectEqual(rset.columnCount(), static_cast<uint32>(0), "update column count default") && ok;
    ok = expectEqual(rset.columnName(0), std::string(""), "update column name default") && ok;
    ok = expectEqual(rset.isNull("id"), false, "update isNull default") && ok;
    ok = expectEqual(rset.getBlobBytes("id"), std::string(""), "update blob bytes default") && ok;

    return ok;
}

} // namespace

auto runMariaDBResultSetMetadataSelfTests() -> bool
{
    return testSelectMetadata() && testUpdateMetadata();
}
