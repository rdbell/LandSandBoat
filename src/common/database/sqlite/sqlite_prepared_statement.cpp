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

#include <common/database/sqlite/sqlite_prepared_statement.h>

#include <sqlite3.h>

#include <cctype>
#include <memory>
#include <string>
#include <type_traits>
#include <variant>

namespace
{

auto lowercase(std::string value) -> std::string
{
    for (auto& ch : value)
    {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return value;
}

} // namespace

db::detail::sqlite::Error::Error(int code, const std::string& message)
: std::runtime_error(message)
, code(code)
{
}

db::SQLitePreparedStatement::SQLitePreparedStatement(sqlite3* db, sqlite3_stmt* stmt)
: db_(db)
, stmt_(stmt)
{
}

db::SQLitePreparedStatement::~SQLitePreparedStatement()
{
    if (stmt_ != nullptr)
    {
        sqlite3_finalize(stmt_);
    }
}

auto db::SQLitePreparedStatement::bind(int index, const BoundValue& value) -> void
{
    const auto rc = std::visit(
        [&](const auto& v) -> int
        {
            using U = std::remove_cvref_t<decltype(v)>;
            if constexpr (std::is_same_v<U, std::shared_ptr<BlobWrapper>>)
            {
                return sqlite3_bind_blob64(stmt_, index, v->data.get(), static_cast<sqlite3_uint64>(v->size), SQLITE_TRANSIENT);
            }
            else if constexpr (std::is_same_v<U, std::string>)
            {
                return sqlite3_bind_text64(stmt_, index, v.data(), static_cast<sqlite3_uint64>(v.size()), SQLITE_TRANSIENT, SQLITE_UTF8);
            }
            else if constexpr (std::is_same_v<U, float> || std::is_same_v<U, double>)
            {
                return sqlite3_bind_double(stmt_, index, static_cast<double>(v));
            }
            else
            {
                return sqlite3_bind_int64(stmt_, index, static_cast<sqlite3_int64>(v));
            }
        },
        value);

    if (rc != SQLITE_OK)
    {
        throwLastError();
    }
}

auto db::SQLitePreparedStatement::executeQuery(const std::string& query) -> std::unique_ptr<ResultSet>
{
    try
    {
        ensureSchema();
        auto rows = fetchRows();
        resetStatement();
        return std::make_unique<SQLiteResultSet>(query, schema_, std::move(rows));
    }
    catch (...)
    {
        resetStatement();
        throw;
    }
}

auto db::SQLitePreparedStatement::executeUpdate(const std::string& query) -> std::unique_ptr<ResultSet>
{
    try
    {
        const auto rc = sqlite3_step(stmt_);
        if (rc != SQLITE_DONE && rc != SQLITE_ROW)
        {
            throwLastError();
        }
        const auto affected = sqlite3_changes64(db_);
        resetStatement();
        return std::make_unique<SQLiteResultSet>(static_cast<std::size_t>(affected), query);
    }
    catch (...)
    {
        resetStatement();
        throw;
    }
}

auto db::SQLitePreparedStatement::resetStatement() -> void
{
    sqlite3_reset(stmt_);
    sqlite3_clear_bindings(stmt_);
}

auto db::SQLitePreparedStatement::ensureSchema() -> void
{
    if (schemaInitialized_)
    {
        return;
    }

    auto schema = std::make_shared<SQLiteColumnSchema>();
    const auto ncol = sqlite3_column_count(stmt_);
    schema->names.reserve(static_cast<std::size_t>(ncol));

    for (int i = 0; i < ncol; ++i)
    {
        auto name = std::string(sqlite3_column_name(stmt_, i));
        const auto index = static_cast<std::size_t>(i);
        schema->index[name] = index;
        schema->index[lowercase(name)] = index;
        schema->names.push_back(std::move(name));
    }

    schema_            = std::move(schema);
    schemaInitialized_ = true;
}

auto db::SQLitePreparedStatement::fetchRows() -> std::vector<SQLiteResultSet::Row>
{
    std::vector<SQLiteResultSet::Row> rows;
    const auto ncol = sqlite3_column_count(stmt_);

    for (;;)
    {
        const auto rc = sqlite3_step(stmt_);
        if (rc == SQLITE_DONE)
        {
            break;
        }
        if (rc != SQLITE_ROW)
        {
            throwLastError();
        }

        SQLiteResultSet::Row row;
        row.reserve(static_cast<std::size_t>(ncol));
        for (int i = 0; i < ncol; ++i)
        {
            switch (sqlite3_column_type(stmt_, i))
            {
                case SQLITE_INTEGER:
                    row.emplace_back(static_cast<int64>(sqlite3_column_int64(stmt_, i)));
                    break;
                case SQLITE_FLOAT:
                    row.emplace_back(sqlite3_column_double(stmt_, i));
                    break;
                case SQLITE_TEXT:
                {
                    const auto* text = reinterpret_cast<const char*>(sqlite3_column_text(stmt_, i));
                    const auto  size = sqlite3_column_bytes(stmt_, i);
                    row.emplace_back(std::string(text, static_cast<std::size_t>(size)));
                    break;
                }
                case SQLITE_BLOB:
                {
                    const auto* blob = static_cast<const char*>(sqlite3_column_blob(stmt_, i));
                    const auto  size = sqlite3_column_bytes(stmt_, i);
                    row.emplace_back(std::string(blob, static_cast<std::size_t>(size)));
                    break;
                }
                case SQLITE_NULL:
                default:
                    row.emplace_back(std::monostate{});
                    break;
            }
        }
        rows.push_back(std::move(row));
    }

    return rows;
}

auto db::SQLitePreparedStatement::throwLastError() const -> void
{
    throw detail::sqlite::Error(sqlite3_errcode(db_), sqlite3_errmsg(db_));
}
