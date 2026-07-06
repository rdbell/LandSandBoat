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

#include <common/database/caching_database.h>
#include <common/database/connection.h>
#include <common/database/sqlite/sqlite_connection.h>

#include <memory>
#include <string>

namespace db
{

class SQLiteDatabase final : public CachingDatabase
{
public:
    explicit SQLiteDatabase(std::string uri);

    auto executeScript(const std::string& script) -> void;
    auto backupFromFile(const std::string& path) -> void;

protected:
    auto createConnection() -> std::unique_ptr<Connection> override;

private:
    std::string                       uri_;
    std::unique_ptr<SQLiteConnection> keeperConnection_;
};

} // namespace db
