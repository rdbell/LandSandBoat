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

#include <common/database/sqlite/sqlite_connection.h>

#include <common/database/sqlite/sqlite_prepared_statement.h>

#include <sqlite3.h>

#include <memory>
#include <regex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace
{

auto replaceAll(std::string& text, std::string_view from, std::string_view to) -> void
{
    std::size_t pos = 0;
    while ((pos = text.find(from, pos)) != std::string::npos)
    {
        text.replace(pos, from.size(), to);
        pos += to.size();
    }
}

auto findCaseInsensitive(std::string_view text, std::string_view needle, std::size_t pos = 0) -> std::size_t
{
    const auto lower = [](const unsigned char ch)
    {
        return static_cast<char>(std::tolower(ch));
    };

    if (needle.empty() || text.size() < needle.size() || pos > text.size() - needle.size())
    {
        return std::string_view::npos;
    }

    for (std::size_t i = pos; i + needle.size() <= text.size(); ++i)
    {
        bool matched = true;
        for (std::size_t j = 0; j < needle.size(); ++j)
        {
            if (lower(static_cast<unsigned char>(text[i + j])) != lower(static_cast<unsigned char>(needle[j])))
            {
                matched = false;
                break;
            }
        }
        if (matched)
        {
            return i;
        }
    }

    return std::string_view::npos;
}

auto trim(std::string_view text) -> std::string_view
{
    while (!text.empty() && std::isspace(static_cast<unsigned char>(text.front())))
    {
        text.remove_prefix(1);
    }
    while (!text.empty() && std::isspace(static_cast<unsigned char>(text.back())))
    {
        text.remove_suffix(1);
    }
    return text;
}

auto splitTopLevelCommaList(std::string_view text) -> std::vector<std::string_view>
{
    std::vector<std::string_view> values;
    bool                          inString = false;
    std::size_t                   depth    = 0;
    std::size_t                   start    = 0;

    for (std::size_t i = 0; i < text.size(); ++i)
    {
        const auto ch = text[i];
        if (ch == '\'' && (i == 0 || text[i - 1] != '\\'))
        {
            inString = !inString;
            continue;
        }
        if (inString)
        {
            continue;
        }
        if (ch == '(')
        {
            ++depth;
            continue;
        }
        if (ch == ')' && depth > 0)
        {
            --depth;
            continue;
        }
        if (ch == ',' && depth == 0)
        {
            values.emplace_back(trim(text.substr(start, i - start)));
            start = i + 1;
        }
    }

    values.emplace_back(trim(text.substr(start)));
    return values;
}

auto translateInsertSet(std::string query) -> std::string
{
    constexpr std::string_view insertPrefix = "INSERT INTO ";

    if (findCaseInsensitive(query, insertPrefix) != 0)
    {
        return query;
    }

    const auto setPos = findCaseInsensitive(query, " SET ", insertPrefix.size());
    if (setPos == std::string::npos)
    {
        return query;
    }

    const auto duplicatePos = findCaseInsensitive(query, " ON DUPLICATE KEY UPDATE ", setPos + 5);
    const auto table        = trim(std::string_view(query).substr(insertPrefix.size(), setPos - insertPrefix.size()));
    const auto assignments  = trim(std::string_view(query).substr(setPos + 5, (duplicatePos == std::string::npos ? query.size() : duplicatePos) - (setPos + 5)));

    std::vector<std::string_view> columns;
    std::vector<std::string_view> values;
    for (const auto assignment : splitTopLevelCommaList(assignments))
    {
        const auto equals = assignment.find('=');
        if (equals == std::string_view::npos)
        {
            return query;
        }
        columns.emplace_back(trim(assignment.substr(0, equals)));
        values.emplace_back(trim(assignment.substr(equals + 1)));
    }

    std::string translated = "INSERT INTO ";
    translated.append(table);
    translated.append(" (");
    for (std::size_t i = 0; i < columns.size(); ++i)
    {
        if (i > 0)
        {
            translated.append(", ");
        }
        translated.append(columns[i]);
    }
    translated.append(") VALUES (");
    for (std::size_t i = 0; i < values.size(); ++i)
    {
        if (i > 0)
        {
            translated.append(", ");
        }
        translated.append(values[i]);
    }
    translated.push_back(')');

    if (duplicatePos != std::string::npos)
    {
        translated.append(std::string_view(query).substr(duplicatePos));
    }

    return translated;
}

} // namespace

auto db::detail::sqlite::translateQuery(std::string query) -> std::string
{
    static const std::regex onDuplicatePattern(R"(\s+ON\s+DUPLICATE\s+KEY\s+UPDATE\s+)", std::regex_constants::icase);
    static const std::regex valuesPattern(R"(\bvalues\s*\(\s*`?([A-Za-z0-9_]+)`?\s*\))", std::regex_constants::icase);
    static const std::regex dateAddThirtyDayPattern(R"(DATE_ADD\s*\(\s*NOW\s*\(\s*\)\s*,\s*INTERVAL\s+30\s+DAY\s*\))", std::regex_constants::icase);
    static const std::regex datediffNowPattern(R"(DATEDIFF\s*\(\s*CURRENT_TIMESTAMP\s*,\s*([A-Za-z0-9_`.]+)\s*\))", std::regex_constants::icase);
    static const std::regex unixTimestampEmptyPattern(R"(UNIX_TIMESTAMP\s*\(\s*\))", std::regex_constants::icase);
    static const std::regex unixTimestampBacktickAliasPattern(R"(UNIX_TIMESTAMP\s*\(\s*`([^`]+)`\s*\)\s+AS\s+([A-Za-z0-9_]+))", std::regex_constants::icase);
    static const std::regex unixTimestampIdentifierAliasPattern(R"(UNIX_TIMESTAMP\s*\(\s*([A-Za-z0-9_.]+)\s*\)\s+AS\s+([A-Za-z0-9_]+))", std::regex_constants::icase);
    static const std::regex unixTimestampBacktickPattern(R"(UNIX_TIMESTAMP\s*\(\s*`([^`]+)`\s*\))", std::regex_constants::icase);
    static const std::regex unixTimestampIdentifierPattern(R"(UNIX_TIMESTAMP\s*\(\s*([A-Za-z0-9_.]+)\s*\))", std::regex_constants::icase);
    static const std::regex ifFunctionPattern(R"(\bIF\s*\()", std::regex_constants::icase);
    static const std::regex insertIgnorePattern(R"(^(\s*)INSERT\s+IGNORE\s+INTO\b)", std::regex_constants::icase);
    static const std::regex insertLimitPattern(R"(^(\s*INSERT\b.+?)\s+LIMIT\s+\d+\s*$)", std::regex_constants::icase);
    static const std::regex mutationLimitPattern(R"(^(\s*(?:UPDATE|DELETE)\b.+?)\s+LIMIT\s+1\s*$)", std::regex_constants::icase);
    static const std::regex deleteAliasPattern(R"(^(\s*)DELETE\s+([A-Za-z0-9_`]+)\s+FROM\s+\2\b)", std::regex_constants::icase);

    query = translateInsertSet(std::move(query));

    const std::string unityMembersQuery =
        "UPDATE unity_system JOIN (SELECT unity_leader, COUNT(*) AS members FROM char_profile GROUP BY unity_leader) TMP "
        "ON unity_system.leader = unity_leader SET unity_system.members_current = members";
    const std::string titleUpdateQuery =
        "UPDATE chars LEFT JOIN char_stats USING(charid) SET titles = ?, title = ? WHERE charid = ? LIMIT 1";

    if (query == unityMembersQuery)
    {
        return "UPDATE unity_system SET members_current = "
               "(SELECT COUNT(*) FROM char_profile WHERE char_profile.unity_leader = unity_system.leader)";
    }
    if (query == titleUpdateQuery)
    {
        return "UPDATE chars SET titles = ?, title = ? WHERE charid = ?";
    }

    query = std::regex_replace(query, dateAddThirtyDayPattern, "datetime(CURRENT_TIMESTAMP, '+30 day')");
    query = std::regex_replace(query, datediffNowPattern, "CAST(julianday(CURRENT_TIMESTAMP) - julianday($1) AS INTEGER)");
    query = std::regex_replace(query, unixTimestampEmptyPattern, "CAST(strftime('%s', 'now') AS INTEGER)");
    query = std::regex_replace(query, unixTimestampBacktickAliasPattern, "CAST(strftime('%s', `$1`) AS INTEGER) AS $2");
    query = std::regex_replace(query, unixTimestampIdentifierAliasPattern, "CAST(strftime('%s', $1) AS INTEGER) AS $2");
    query = std::regex_replace(query, unixTimestampBacktickPattern, "CAST(strftime('%s', `$1`) AS INTEGER) AS \"UNIX_TIMESTAMP(`$1`)\"");
    query = std::regex_replace(query, unixTimestampIdentifierPattern, "CAST(strftime('%s', $1) AS INTEGER) AS \"UNIX_TIMESTAMP($1)\"");
    query = std::regex_replace(query, ifFunctionPattern, "IIF(");
    query = std::regex_replace(query, insertIgnorePattern, "$1INSERT OR IGNORE INTO");
    query = std::regex_replace(query, deleteAliasPattern, "$1DELETE FROM $2");

    replaceAll(query, "NOW()", "CURRENT_TIMESTAMP");
    replaceAll(query, "now()", "CURRENT_TIMESTAMP");
    replaceAll(query, "CURRENT_TIMESTAMP()", "CURRENT_TIMESTAMP");
    replaceAll(query, "current_timestamp()", "CURRENT_TIMESTAMP");
    replaceAll(query,
               "FIELD(location,0,1,9,2,3,4,5,6,7,8,10,11,12)",
               "CASE location WHEN 0 THEN 0 WHEN 1 THEN 1 WHEN 9 THEN 2 WHEN 2 THEN 3 WHEN 3 THEN 4 WHEN 4 THEN 5 WHEN 5 THEN 6 WHEN 6 THEN 7 WHEN 7 THEN 8 WHEN 8 THEN 9 WHEN 10 THEN 10 WHEN 11 THEN 11 WHEN 12 THEN 12 ELSE 13 END");

    query = std::regex_replace(query, onDuplicatePattern, " ON CONFLICT DO UPDATE SET ");
    query = std::regex_replace(query, valuesPattern, "excluded.$1");
    query = std::regex_replace(query, insertLimitPattern, "$1");
    query = std::regex_replace(query, mutationLimitPattern, "$1");

    return query;
}

db::SQLiteConnection::SQLiteConnection(sqlite3* db)
: db_(db)
{
}

db::SQLiteConnection::~SQLiteConnection()
{
    if (db_ != nullptr)
    {
        sqlite3_close(db_);
    }
}

auto db::SQLiteConnection::prepare(const std::string& query) -> std::unique_ptr<PreparedStatement>
{
    const auto translatedQuery = detail::sqlite::translateQuery(query);
    sqlite3_stmt* stmt = nullptr;
    const auto rc = sqlite3_prepare_v2(db_, translatedQuery.c_str(), static_cast<int>(translatedQuery.size()), &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        throw detail::sqlite::Error(sqlite3_errcode(db_), sqlite3_errmsg(db_));
    }

    return std::make_unique<SQLitePreparedStatement>(db_, stmt);
}

auto db::SQLiteConnection::schema() -> std::string
{
    return "xidb";
}

auto db::SQLiteConnection::version() -> std::string
{
    return std::string("SQLite ") + sqlite3_libversion();
}

auto db::SQLiteConnection::driverVersion() -> std::string
{
    return std::string("SQLite ") + sqlite3_libversion();
}

auto db::SQLiteConnection::isConnectionError(const std::exception& e) const -> bool
{
    if (const auto* error = dynamic_cast<const detail::sqlite::Error*>(&e))
    {
        return error->code == SQLITE_BUSY || error->code == SQLITE_LOCKED;
    }

    return false;
}

auto db::SQLiteConnection::connect(const std::string& uri) -> std::unique_ptr<SQLiteConnection>
{
    sqlite3* db = nullptr;
    const auto flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI | SQLITE_OPEN_FULLMUTEX;
    const auto rc = sqlite3_open_v2(uri.c_str(), &db, flags, nullptr);
    if (rc != SQLITE_OK)
    {
        const std::string message = db != nullptr ? sqlite3_errmsg(db) : "failed to allocate SQLite handle";
        if (db != nullptr)
        {
            sqlite3_close(db);
        }
        throw detail::sqlite::Error(rc, message);
    }

    auto connection = std::make_unique<SQLiteConnection>(db);
    connection->exec("PRAGMA foreign_keys = OFF");
    connection->exec("PRAGMA journal_mode = MEMORY");
    connection->exec("PRAGMA synchronous = OFF");
    connection->exec("PRAGMA temp_store = MEMORY");
    connection->exec("PRAGMA busy_timeout = 5000");
    return connection;
}

auto db::SQLiteConnection::exec(const std::string& query) -> void
{
    char* error = nullptr;
    const auto rc = sqlite3_exec(db_, query.c_str(), nullptr, nullptr, &error);
    if (rc != SQLITE_OK)
    {
        const std::string message = error != nullptr ? error : sqlite3_errmsg(db_);
        sqlite3_free(error);
        throw detail::sqlite::Error(rc, message);
    }
}

auto db::SQLiteConnection::backupFromFile(const std::string& path) -> void
{
    sqlite3* source = nullptr;
    const auto openRc = sqlite3_open_v2(path.c_str(), &source, SQLITE_OPEN_READONLY | SQLITE_OPEN_URI | SQLITE_OPEN_FULLMUTEX, nullptr);
    if (openRc != SQLITE_OK)
    {
        const std::string message = source != nullptr ? sqlite3_errmsg(source) : "failed to allocate SQLite source handle";
        if (source != nullptr)
        {
            sqlite3_close(source);
        }
        throw detail::sqlite::Error(openRc, message);
    }

    sqlite3_backup* backup = sqlite3_backup_init(db_, "main", source, "main");
    if (backup == nullptr)
    {
        const auto        rc      = sqlite3_errcode(db_);
        const std::string message = sqlite3_errmsg(db_);
        sqlite3_close(source);
        throw detail::sqlite::Error(rc, message);
    }

    const auto stepRc   = sqlite3_backup_step(backup, -1);
    const auto finishRc = sqlite3_backup_finish(backup);
    sqlite3_close(source);

    if (stepRc != SQLITE_DONE)
    {
        throw detail::sqlite::Error(stepRc, sqlite3_errmsg(db_));
    }

    if (finishRc != SQLITE_OK)
    {
        throw detail::sqlite::Error(finishRc, sqlite3_errmsg(db_));
    }
}
