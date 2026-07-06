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

#pragma once

#include <common/database/bound_value.h>
#include <common/database/prepared_statement.h>
#include <common/database/result_set.h>
#include <common/database/sqlite/sqlite_result_set.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

struct sqlite3;
struct sqlite3_stmt;

namespace db::detail::sqlite
{

struct Error final : std::runtime_error
{
    Error(int code, const std::string& message);

    int code;
};

} // namespace db::detail::sqlite

namespace db
{

class SQLitePreparedStatement final : public PreparedStatement
{
public:
    SQLitePreparedStatement(sqlite3* db, sqlite3_stmt* stmt);
    ~SQLitePreparedStatement() override;

    SQLitePreparedStatement(const SQLitePreparedStatement&)            = delete;
    SQLitePreparedStatement& operator=(const SQLitePreparedStatement&) = delete;

    auto bind(int index, const BoundValue& value) -> void override;
    auto executeQuery(const std::string& query) -> std::unique_ptr<ResultSet> override;
    auto executeUpdate(const std::string& query) -> std::unique_ptr<ResultSet> override;

private:
    auto resetStatement() -> void;
    auto ensureSchema() -> void;
    auto fetchRows() -> std::vector<SQLiteResultSet::Row>;
    auto throwLastError() const -> void;

    sqlite3*      db_{ nullptr };
    sqlite3_stmt* stmt_{ nullptr };

    std::shared_ptr<const SQLiteColumnSchema> schema_;
    bool                                      schemaInitialized_{ false };
};

} // namespace db
