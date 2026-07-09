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

#include <common/database/connection.h>

#include <exception>
#include <memory>
#include <string>

struct sqlite3;

namespace db::detail::sqlite
{

// Rewrites the MariaDB-flavored query forms used by LSB into the SQLite
// equivalents accepted by SQLiteConnection.
auto translateQuery(std::string query) -> std::string;

} // namespace db::detail::sqlite

namespace db
{

class SQLiteConnection final : public Connection
{
public:
    explicit SQLiteConnection(sqlite3* db);
    ~SQLiteConnection() override;

    SQLiteConnection(const SQLiteConnection&)            = delete;
    SQLiteConnection& operator=(const SQLiteConnection&) = delete;

    auto prepare(const std::string& query) -> std::unique_ptr<PreparedStatement> override;
    auto schema() -> std::string override;
    auto version() -> std::string override;
    auto driverVersion() -> std::string override;
    auto isConnectionError(const std::exception& e) const -> bool override;

    auto exec(const std::string& query) -> void;
    auto backupFromFile(const std::string& path) -> void;

    static auto connect(const std::string& uri) -> std::unique_ptr<SQLiteConnection>;

private:
    sqlite3* db_{ nullptr };
};

} // namespace db
