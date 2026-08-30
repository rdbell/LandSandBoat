/*
===========================================================================

  Copyright (c) 2025 LandSandBoat Dev Teams

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

#include "test_application.h"
#include "test_engine.h"

#include "common/database.h"
#include "common/database/sqlite/sqlite_database.h"
#include "common/settings.h"

#include <nlohmann/json.hpp>
#include <spdlog/async.h>
#include <sqlite3.h>

#include <algorithm>
#include <charconv>
#include <chrono>
#include <cctype>
#include <filesystem>
#include <future>
#include <format>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

namespace
{

constexpr auto TestDaemonPrefix = "XI_TESTD ";

auto jsonStringArray(const nlohmann::json& object, const char* key) -> std::vector<std::string>
{
    std::vector<std::string> values;
    if (!object.contains(key))
    {
        return values;
    }

    const auto& node = object.at(key);
    if (node.is_string())
    {
        values.push_back(node.get<std::string>());
        return values;
    }

    if (!node.is_array())
    {
        return values;
    }

    for (const auto& value : node)
    {
        if (value.is_string())
        {
            values.push_back(value.get<std::string>());
        }
    }

    return values;
}

void writeDaemonResponse(nlohmann::json response)
{
    std::cout << TestDaemonPrefix << response.dump() << std::endl;
}

auto daemonReportSummary(const std::string& outputPath) -> nlohmann::json
{
    if (outputPath.empty())
    {
        return nullptr;
    }

    std::ifstream file(outputPath);
    if (!file.is_open())
    {
        return nullptr;
    }

    try
    {
        nlohmann::json report;
        file >> report;
        if (report.contains("results") && report["results"].contains("summary"))
        {
            return report["results"]["summary"];
        }
    }
    catch (...)
    {
    }

    return nullptr;
}

auto trimWhitespace(std::string_view text) -> std::string_view
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

auto startsWith(std::string_view text, std::string_view prefix) -> bool
{
    return text.size() >= prefix.size() && text.substr(0, prefix.size()) == prefix;
}

void replaceAll(std::string& text, std::string_view from, std::string_view to)
{
    std::size_t pos = 0;
    while ((pos = text.find(from, pos)) != std::string::npos)
    {
        text.replace(pos, from.size(), to);
        pos += to.size();
    }
}

auto stripTrailingSqlComment(std::string line) -> std::string
{
    bool inString = false;
    for (std::size_t i = 0; i + 1 < line.size(); ++i)
    {
        if (line[i] == '\'' && (i == 0 || line[i - 1] != '\\'))
        {
            inString = !inString;
            continue;
        }

        if (!inString && line[i] == '-' && line[i + 1] == '-' &&
            (i == 0 || std::isspace(static_cast<unsigned char>(line[i - 1])) || line[i - 1] == ';'))
        {
            line.resize(i);
            break;
        }
    }

    return line;
}

auto stripTrailingComma(std::string line) -> std::string
{
    while (!line.empty() && std::isspace(static_cast<unsigned char>(line.back())))
    {
        line.pop_back();
    }

    if (!line.empty() && line.back() == ',')
    {
        line.pop_back();
    }

    return line;
}

auto convertSQLiteSchemaLine(std::string line) -> std::string
{
    line = stripTrailingSqlComment(std::move(line));

    replaceAll(line, " unsigned", "");
    replaceAll(line, " UNSIGNED", "");
    replaceAll(line, " signed", "");
    replaceAll(line, " SIGNED", "");
    replaceAll(line, " zerofill", "");
    replaceAll(line, " ZEROFILL", "");
    replaceAll(line, " AUTO_INCREMENT", "");
    replaceAll(line, " auto_increment", "");
    replaceAll(line, " CURRENT_TIMESTAMP()", " CURRENT_TIMESTAMP");
    replaceAll(line, " current_timestamp()", " CURRENT_TIMESTAMP");

    static const std::regex collatePattern(R"(\s+COLLATE\s+[A-Za-z0-9_]+)", std::regex_constants::icase);
    static const std::regex charsetPattern(R"(\s+CHARACTER\s+SET\s+[A-Za-z0-9_]+)", std::regex_constants::icase);
    static const std::regex commentPattern(R"(\s+COMMENT\s+'([^'\\]|\\.)*')", std::regex_constants::icase);
    static const std::regex onUpdatePattern(R"(\s+ON\s+UPDATE\s+CURRENT_TIMESTAMP(?:\(\))?)", std::regex_constants::icase);
    static const std::regex enumPattern(R"(\b(enum|set)\s*\([^)]*\))", std::regex_constants::icase);
    static const std::regex namedPrimaryKeyPattern(R"(PRIMARY\s+KEY\s+`[^`]+`\s*)", std::regex_constants::icase);
    static const std::regex defaultVariablePattern(R"(DEFAULT\s+@[A-Za-z0-9_]+)", std::regex_constants::icase);
    static const std::regex usingIndexPattern(R"(\s+USING\s+(BTREE|HASH))", std::regex_constants::icase);

    line = std::regex_replace(line, collatePattern, "");
    line = std::regex_replace(line, charsetPattern, "");
    line = std::regex_replace(line, commentPattern, "");
    line = std::regex_replace(line, onUpdatePattern, "");
    line = std::regex_replace(line, enumPattern, "TEXT");
    line = std::regex_replace(line, namedPrimaryKeyPattern, "PRIMARY KEY ");
    line = std::regex_replace(line, defaultVariablePattern, "DEFAULT 0");
    line = std::regex_replace(line, usingIndexPattern, "");

    return line;
}

class SQLiteSeedHandle
{
public:
    explicit SQLiteSeedHandle(const std::filesystem::path& path)
    {
        const auto rc = sqlite3_open_v2(path.string().c_str(), &db_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_URI | SQLITE_OPEN_FULLMUTEX, nullptr);
        if (rc != SQLITE_OK)
        {
            const std::string message = db_ != nullptr ? sqlite3_errmsg(db_) : "failed to allocate SQLite handle";
            throw std::runtime_error(fmt::format("Failed to open SQLite seed cache {}: {}", path.string(), message));
        }
    }

    ~SQLiteSeedHandle()
    {
        for (auto& [_, stmt] : statements_)
        {
            sqlite3_finalize(stmt);
        }

        if (db_ != nullptr)
        {
            sqlite3_close(db_);
        }
    }

    SQLiteSeedHandle(const SQLiteSeedHandle&)            = delete;
    SQLiteSeedHandle& operator=(const SQLiteSeedHandle&) = delete;

    auto get() -> sqlite3*
    {
        return db_;
    }

    void exec(std::string_view sql)
    {
        char*      error = nullptr;
        const auto rc    = sqlite3_exec(db_, std::string(sql).c_str(), nullptr, nullptr, &error);
        if (rc != SQLITE_OK)
        {
            const std::string message = error != nullptr ? error : sqlite3_errmsg(db_);
            sqlite3_free(error);
            throw std::runtime_error(fmt::format("SQLite seed exec failed: {}\nSQL: {}", message, sql));
        }
    }

    auto statement(const std::string& key, const std::string& sql) -> sqlite3_stmt*
    {
        auto it = statements_.find(key);
        if (it != statements_.end())
        {
            return it->second;
        }

        sqlite3_stmt* stmt = nullptr;
        const auto    rc   = sqlite3_prepare_v2(db_, sql.c_str(), static_cast<int>(sql.size()), &stmt, nullptr);
        if (rc != SQLITE_OK)
        {
            throw std::runtime_error(fmt::format("SQLite seed prepare failed: {}\nSQL: {}", sqlite3_errmsg(db_), sql));
        }

        return statements_.emplace(key, stmt).first->second;
    }

private:
    sqlite3*                                    db_{ nullptr };
    std::unordered_map<std::string, sqlite3_stmt*> statements_;
};

struct SQLiteSeedBlob
{
    std::vector<std::byte> data;
};

using SQLiteSeedValue = std::variant<std::monostate, int64_t, double, std::string, SQLiteSeedBlob>;
using SQLiteSeedVariables = std::unordered_map<std::string, SQLiteSeedValue>;

auto quoteSQLiteIdentifier(std::string_view identifier) -> std::string
{
    std::string quoted;
    quoted.reserve(identifier.size() + 2);
    quoted.push_back('"');
    for (const auto ch : identifier)
    {
        if (ch == '"')
        {
            quoted.push_back('"');
        }
        quoted.push_back(ch);
    }
    quoted.push_back('"');
    return quoted;
}

auto normalizeSQLIdentifier(std::string_view identifier) -> std::string
{
    identifier = trimWhitespace(identifier);
    if (identifier.size() >= 2 && ((identifier.front() == '`' && identifier.back() == '`') || (identifier.front() == '"' && identifier.back() == '"')))
    {
        identifier.remove_prefix(1);
        identifier.remove_suffix(1);
    }
    return std::string(identifier);
}

auto extractInsertTable(std::string_view statement) -> std::string
{
    constexpr std::string_view prefix = "INSERT INTO ";
    auto                       rest   = trimWhitespace(statement.substr(prefix.size()));

    std::size_t end = 0;
    if (!rest.empty() && rest.front() == '`')
    {
        end = rest.find('`', 1);
        if (end != std::string_view::npos)
        {
            return std::string(rest.substr(1, end - 1));
        }
    }

    while (end < rest.size() && !std::isspace(static_cast<unsigned char>(rest[end])) && rest[end] != '(')
    {
        ++end;
    }

    return normalizeSQLIdentifier(rest.substr(0, end));
}

auto extractInsertColumns(std::string_view statement) -> std::vector<std::string>
{
    auto values = statement.find(" VALUES");
    if (values == std::string_view::npos)
    {
        values = statement.find(" values");
    }
    if (values == std::string_view::npos)
    {
        return {};
    }

    auto head = trimWhitespace(statement.substr(0, values));
    auto open = head.find('(');
    if (open == std::string_view::npos)
    {
        return {};
    }

    auto close = head.rfind(')');
    if (close == std::string_view::npos || close <= open)
    {
        return {};
    }

    std::vector<std::string> columns;
    auto                     list = head.substr(open + 1, close - open - 1);
    while (!list.empty())
    {
        const auto comma = list.find(',');
        const auto part  = comma == std::string_view::npos ? list : list.substr(0, comma);
        columns.emplace_back(normalizeSQLIdentifier(part));

        if (comma == std::string_view::npos)
        {
            break;
        }
        list.remove_prefix(comma + 1);
    }

    return columns;
}

auto hexDigitValue(char ch) -> uint8_t
{
    if (ch >= '0' && ch <= '9')
    {
        return static_cast<uint8_t>(ch - '0');
    }
    if (ch >= 'a' && ch <= 'f')
    {
        return static_cast<uint8_t>(10 + ch - 'a');
    }
    if (ch >= 'A' && ch <= 'F')
    {
        return static_cast<uint8_t>(10 + ch - 'A');
    }
    return 0;
}

auto parseHexBlob(std::string_view value) -> SQLiteSeedBlob
{
    if (startsWith(value, "0x") || startsWith(value, "0X"))
    {
        value.remove_prefix(2);
    }

    SQLiteSeedBlob blob;
    blob.data.reserve((value.size() + 1) / 2);

    if (value.size() % 2 == 1)
    {
        blob.data.push_back(static_cast<std::byte>(hexDigitValue(value.front())));
        value.remove_prefix(1);
    }

    for (std::size_t i = 0; i + 1 < value.size(); i += 2)
    {
        blob.data.push_back(static_cast<std::byte>((hexDigitValue(value[i]) << 4) | hexDigitValue(value[i + 1])));
    }

    return blob;
}

auto parseSeedNumber(std::string_view value) -> SQLiteSeedValue
{
    if (value.find_first_of(".eE") != std::string_view::npos)
    {
        double parsed = 0.0;
        auto   result = std::from_chars(value.data(), value.data() + value.size(), parsed);
        if (result.ec == std::errc{})
        {
            return parsed;
        }
    }
    else
    {
        int64_t parsed = 0;
        auto    result = std::from_chars(value.data(), value.data() + value.size(), parsed);
        if (result.ec == std::errc{})
        {
            return parsed;
        }
    }

    return std::string(value);
}

auto seedValueAsInteger(const SQLiteSeedValue& value) -> std::pair<bool, int64_t>
{
    if (const auto* integer = std::get_if<int64_t>(&value))
    {
        return { true, *integer };
    }

    return { false, 0 };
}

auto parseSeedIntegerTerm(std::string_view term, const SQLiteSeedVariables& variables) -> std::pair<bool, int64_t>
{
    term = trimWhitespace(term);

    while (term.size() >= 2 && term.front() == '(' && term.back() == ')')
    {
        term.remove_prefix(1);
        term.remove_suffix(1);
        term = trimWhitespace(term);
    }

    if (term.empty())
    {
        return { false, 0 };
    }

    if (term.front() == '@')
    {
        const auto it = variables.find(std::string(term.substr(1)));
        if (it == variables.end())
        {
            return { false, 0 };
        }

        return seedValueAsInteger(it->second);
    }

    if (startsWith(term, "0x") || startsWith(term, "0X"))
    {
        int64_t parsed = 0;
        auto    digits = term.substr(2);
        auto    result = std::from_chars(digits.data(), digits.data() + digits.size(), parsed, 16);
        if (result.ec == std::errc{} && result.ptr == digits.data() + digits.size())
        {
            return { true, parsed };
        }

        return { false, 0 };
    }

    int64_t parsed = 0;
    auto    result = std::from_chars(term.data(), term.data() + term.size(), parsed);
    if (result.ec == std::errc{} && result.ptr == term.data() + term.size())
    {
        return { true, parsed };
    }

    return { false, 0 };
}

auto parseSeedIntegerExpression(std::string_view expression, const SQLiteSeedVariables& variables) -> std::pair<bool, int64_t>
{
    int64_t value = 0;
    bool    sawTerm = false;

    while (true)
    {
        const auto separator = expression.find('|');
        const auto term      = separator == std::string_view::npos ? expression : expression.substr(0, separator);
        auto [ok, termValue] = parseSeedIntegerTerm(term, variables);
        if (!ok)
        {
            return { false, 0 };
        }

        value |= termValue;
        sawTerm = true;

        if (separator == std::string_view::npos)
        {
            break;
        }

        expression.remove_prefix(separator + 1);
    }

    return { sawTerm, value };
}

auto parseSQLString(std::string_view& text) -> std::string
{
    std::string value;
    text.remove_prefix(1);

    while (!text.empty())
    {
        const auto ch = text.front();
        text.remove_prefix(1);

        if (ch == '\'')
        {
            if (!text.empty() && text.front() == '\'')
            {
                value.push_back('\'');
                text.remove_prefix(1);
                continue;
            }

            break;
        }

        if (ch == '\\' && !text.empty())
        {
            const auto escaped = text.front();
            text.remove_prefix(1);
            switch (escaped)
            {
                case '0':
                    value.push_back('\0');
                    break;
                case 'n':
                    value.push_back('\n');
                    break;
                case 'r':
                    value.push_back('\r');
                    break;
                case 't':
                    value.push_back('\t');
                    break;
                case 'b':
                    value.push_back('\b');
                    break;
                case 'Z':
                    value.push_back('\x1A');
                    break;
                default:
                    value.push_back(escaped);
                    break;
            }
            continue;
        }

        value.push_back(ch);
    }

    return value;
}

auto parseSQLValue(std::string_view& text, const SQLiteSeedVariables& variables) -> SQLiteSeedValue
{
    text = trimWhitespace(text);
    if (text.empty())
    {
        return std::monostate{};
    }

    if (text.front() == '\'')
    {
        return parseSQLString(text);
    }

    std::size_t end = 0;
    while (end < text.size() && text[end] != ',')
    {
        ++end;
    }

    auto token = trimWhitespace(text.substr(0, end));
    text.remove_prefix(end);

    if (token == "NULL" || token == "null")
    {
        return std::monostate{};
    }

    if (startsWith(token, "0x") || startsWith(token, "0X"))
    {
        return parseHexBlob(token);
    }

    if (!token.empty() && (token.front() == '@' || token.find('|') != std::string_view::npos))
    {
        auto [ok, value] = parseSeedIntegerExpression(token, variables);
        if (ok)
        {
            return value;
        }
        return int64_t{ 0 };
    }

    return parseSeedNumber(token);
}

auto parseSQLValues(std::string_view valuesText, const SQLiteSeedVariables& variables) -> std::vector<SQLiteSeedValue>
{
    std::vector<SQLiteSeedValue> values;
    while (!trimWhitespace(valuesText).empty())
    {
        values.emplace_back(parseSQLValue(valuesText, variables));
        valuesText = trimWhitespace(valuesText);
        if (!valuesText.empty() && valuesText.front() == ',')
        {
            valuesText.remove_prefix(1);
        }
    }

    return values;
}

auto extractInsertRows(std::string_view statement) -> std::vector<std::string_view>
{
    auto values = statement.find("VALUES");
    if (values == std::string_view::npos)
    {
        values = statement.find("values");
    }
    if (values == std::string_view::npos)
    {
        return {};
    }

    std::vector<std::string_view> rows;
    auto                          rest     = statement.substr(values + 6);
    bool                          inString = false;
    std::size_t                   depth    = 0;
    std::size_t                   rowStart = std::string_view::npos;

    for (std::size_t i = 0; i < rest.size(); ++i)
    {
        const auto ch = rest[i];
        if (ch == '\'' && (i == 0 || rest[i - 1] != '\\'))
        {
            if (inString && i + 1 < rest.size() && rest[i + 1] == '\'')
            {
                ++i;
                continue;
            }
            inString = !inString;
            continue;
        }

        if (inString)
        {
            continue;
        }

        if (ch == '(')
        {
            if (depth == 0)
            {
                rowStart = i + 1;
            }
            ++depth;
        }
        else if (ch == ')' && depth > 0)
        {
            --depth;
            if (depth == 0 && rowStart != std::string_view::npos)
            {
                rows.emplace_back(rest.substr(rowStart, i - rowStart));
                rowStart = std::string_view::npos;
            }
        }
    }

    return rows;
}

auto parseSeedVariableAssignment(std::string_view line) -> std::pair<std::string, SQLiteSeedValue>
{
    static const std::regex variablePattern(R"(^SET\s+@([A-Za-z0-9_]+)\s*=\s*([^;]+))", std::regex_constants::icase);

    std::cmatch match;
    const auto  begin = line.data();
    const auto  end   = line.data() + line.size();
    if (!std::regex_search(begin, end, match, variablePattern))
    {
        return {};
    }

    const auto name  = match[1].str();
    const auto value = trimWhitespace(std::string_view(match[2].first, static_cast<std::size_t>(match[2].length())));
    if (value.empty() || value.find('@') != std::string_view::npos)
    {
        return {};
    }

    return { name, parseSeedNumber(value) };
}

auto insertStatementSQL(const std::string& table, const std::vector<std::string>& columns, std::size_t valueCount) -> std::string
{
    std::string sql = "INSERT INTO " + quoteSQLiteIdentifier(table);
    if (!columns.empty())
    {
        sql += " (";
        for (std::size_t i = 0; i < columns.size(); ++i)
        {
            if (i > 0)
            {
                sql += ", ";
            }
            sql += quoteSQLiteIdentifier(columns[i]);
        }
        sql += ")";
    }

    sql += " VALUES (";
    for (std::size_t i = 0; i < valueCount; ++i)
    {
        if (i > 0)
        {
            sql += ", ";
        }
        sql += "?";
    }
    sql += ")";
    return sql;
}

void bindSeedValue(sqlite3_stmt* stmt, int index, const SQLiteSeedValue& value)
{
    const auto rc = std::visit(
        [&](const auto& v) -> int
        {
            using U = std::remove_cvref_t<decltype(v)>;
            if constexpr (std::is_same_v<U, std::monostate>)
            {
                return sqlite3_bind_null(stmt, index);
            }
            else if constexpr (std::is_same_v<U, int64_t>)
            {
                return sqlite3_bind_int64(stmt, index, static_cast<sqlite3_int64>(v));
            }
            else if constexpr (std::is_same_v<U, double>)
            {
                return sqlite3_bind_double(stmt, index, v);
            }
            else if constexpr (std::is_same_v<U, std::string>)
            {
                return sqlite3_bind_text64(stmt, index, v.data(), static_cast<sqlite3_uint64>(v.size()), SQLITE_TRANSIENT, SQLITE_UTF8);
            }
            else
            {
                return sqlite3_bind_blob64(stmt, index, v.data.data(), static_cast<sqlite3_uint64>(v.data.size()), SQLITE_TRANSIENT);
            }
        },
        value);

    if (rc != SQLITE_OK)
    {
        throw std::runtime_error(fmt::format("SQLite seed bind failed: {}", sqlite3_errstr(rc)));
    }
}

void executeSeedInsert(SQLiteSeedHandle& database, std::string_view statement, const SQLiteSeedVariables& variables)
{
    const auto table   = extractInsertTable(statement);
    const auto columns = extractInsertColumns(statement);
    const auto rows    = extractInsertRows(statement);

    for (const auto row : rows)
    {
        const auto values = parseSQLValues(row, variables);
        const auto key    = fmt::format("{}:{}:{}", table, fmt::join(columns, ","), values.size());
        const auto sql    = insertStatementSQL(table, columns, values.size());
        sqlite3_stmt* stmt = nullptr;
        try
        {
            stmt = database.statement(key, sql);
        }
        catch (const std::exception& e)
        {
            throw std::runtime_error(fmt::format("{}\nRow: {}\nStatement: {}", e.what(), row, statement));
        }

        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);

        for (std::size_t i = 0; i < values.size(); ++i)
        {
            bindSeedValue(stmt, static_cast<int>(i + 1), values[i]);
        }

        const auto rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE)
        {
            throw std::runtime_error(fmt::format("SQLite seed insert failed: {}\nSQL: {}", sqlite3_errmsg(database.get()), sql));
        }
    }
}

auto sqliteSeedFiles() -> std::vector<std::filesystem::path>
{
    std::vector<std::filesystem::path> sqlFiles;
    for (const auto& entry : std::filesystem::directory_iterator("sql"))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".sql" && entry.path().filename() != "triggers.sql")
        {
            sqlFiles.emplace_back(entry.path());
        }
    }

    std::sort(sqlFiles.begin(), sqlFiles.end());
    return sqlFiles;
}

auto sqliteSeedCachePath() -> std::filesystem::path
{
    const auto buildPath = std::filesystem::path("build-mage-lsb-test") / "xi_test_sqlite_seed.db";
    if (std::filesystem::exists(buildPath.parent_path()))
    {
        return buildPath;
    }

    return std::filesystem::temp_directory_path() / "xi_test_sqlite_seed.db";
}

auto sqliteSeedCacheIsFresh(const std::filesystem::path& cachePath) -> bool
{
    if (!std::filesystem::exists(cachePath))
    {
        return false;
    }

    const auto cacheWriteTime = std::filesystem::last_write_time(cachePath);
    for (const auto& entry : std::filesystem::directory_iterator("sql"))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".sql" && std::filesystem::last_write_time(entry.path()) > cacheWriteTime)
        {
            return false;
        }
    }

    return true;
}

void buildSQLiteSeedCache(const std::filesystem::path& cachePath)
{
    std::filesystem::create_directories(cachePath.parent_path());

    const auto tempPath = cachePath.string() + ".tmp";
    std::filesystem::remove(tempPath);

    const auto start = std::chrono::steady_clock::now();

    ShowInfoFmt("Building SQLite seed cache at {}", cachePath.string());
    SQLiteSeedHandle database(tempPath);
    ShowInfoFmt("Opened SQLite seed cache temp file at {}", tempPath);
    database.exec("PRAGMA foreign_keys = OFF");
    database.exec("PRAGMA synchronous = OFF");
    database.exec("PRAGMA journal_mode = MEMORY");
    database.exec("PRAGMA temp_store = MEMORY");
    ShowInfo("Configured SQLite seed cache pragmas");
    database.exec("BEGIN IMMEDIATE");
    ShowInfo("Started SQLite seed cache transaction");

    std::vector<std::string> createLines;
    std::string              pendingInsert;
    SQLiteSeedVariables      variables;
    bool                     inCreate        = false;
    bool                     skippingTrigger = false;
    std::size_t              insertCount     = 0;
    std::size_t              tableCount      = 0;

    const auto executePendingInsert = [&]()
    {
        if (pendingInsert.empty())
        {
            return;
        }

        const auto rows = extractInsertRows(pendingInsert);
        executeSeedInsert(database, pendingInsert, variables);
        insertCount += rows.size();
        pendingInsert.clear();
    };

    auto lastProgress = start;
    for (const auto& path : sqliteSeedFiles())
    {
        const auto fileStart = std::chrono::steady_clock::now();
        ShowInfoFmt("Loading SQLite seed file {}", path.filename().string());

        std::ifstream file(path);
        if (!file.is_open())
        {
            ShowCriticalFmt("Unable to open {} for SQLite test database seed", path.string());
            continue;
        }

        std::string line;
        while (std::getline(file, line))
        {
            if (startsWith(line, "\xEF\xBB\xBF"))
            {
                line.erase(0, 3);
            }

            line         = stripTrailingSqlComment(std::move(line));
            auto trimmed = trimWhitespace(line);

            if (!pendingInsert.empty())
            {
                if (!trimmed.empty())
                {
                    pendingInsert.push_back('\n');
                    pendingInsert.append(trimmed);
                }

                if (trimmed.find(';') != std::string_view::npos)
                {
                    executePendingInsert();
                }
                continue;
            }

            if (trimmed.empty() || startsWith(trimmed, "--") || startsWith(trimmed, "/*!") || startsWith(trimmed, "/*") || startsWith(trimmed, "*/"))
            {
                continue;
            }

            if (startsWith(trimmed, "DELIMITER $$"))
            {
                skippingTrigger = true;
                continue;
            }

            if (skippingTrigger)
            {
                if (startsWith(trimmed, "DELIMITER ;"))
                {
                    skippingTrigger = false;
                }
                continue;
            }

            if (startsWith(trimmed, "SET @"))
            {
                auto [name, value] = parseSeedVariableAssignment(trimmed);
                if (!name.empty())
                {
                    variables[std::move(name)] = std::move(value);
                }
                continue;
            }

            if (startsWith(trimmed, "SET ") || startsWith(trimmed, "LOCK TABLES") || startsWith(trimmed, "UNLOCK TABLES") ||
                startsWith(trimmed, "DROP TABLE") || startsWith(trimmed, "ALTER TABLE"))
            {
                continue;
            }

            if (startsWith(trimmed, "CREATE TABLE"))
            {
                inCreate = true;
                createLines.clear();
                createLines.emplace_back(convertSQLiteSchemaLine(std::string(trimmed)));
                continue;
            }

            if (inCreate)
            {
                if (trimmed.starts_with("KEY ") || trimmed.starts_with("UNIQUE KEY") || trimmed.starts_with("FULLTEXT KEY") ||
                    trimmed.starts_with("INDEX ") || trimmed.starts_with("UNIQUE INDEX") || trimmed.starts_with("FOREIGN KEY") ||
                    trimmed.starts_with("ON DELETE") || trimmed.starts_with("ON UPDATE") || trimmed.starts_with("REFERENCES") ||
                    trimmed.starts_with("CONSTRAINT"))
                {
                    continue;
                }

                if (trimmed.starts_with(") ENGINE=") || trimmed.starts_with(") DEFAULT") || trimmed == ")" || trimmed == ");")
                {
                    if (!createLines.empty())
                    {
                        createLines.back() = stripTrailingComma(std::move(createLines.back()));
                    }

                    std::string createStatement;
                    for (const auto& createLine : createLines)
                    {
                        createStatement.append(createLine);
                        createStatement.push_back('\n');
                    }
                    createStatement.append(")");
                    database.exec(createStatement);

                    ++tableCount;
                    inCreate = false;
                    createLines.clear();
                    continue;
                }

                createLines.emplace_back(convertSQLiteSchemaLine(std::string(trimmed)));
                continue;
            }

            if (startsWith(trimmed, "INSERT INTO"))
            {
                pendingInsert.assign(trimmed);
                if (trimmed.find(';') != std::string_view::npos)
                {
                    executePendingInsert();
                }
            }

            const auto now = std::chrono::steady_clock::now();
            if (now - lastProgress > std::chrono::seconds(5))
            {
                ShowInfoFmt("SQLite seed progress: {} tables, {} rows, currently {}",
                            tableCount,
                            insertCount,
                            path.filename().string());
                lastProgress = now;
            }
        }

        const auto fileEnd = std::chrono::steady_clock::now();
        ShowInfoFmt("Loaded SQLite seed file {} in {} ms ({} tables, {} rows total)",
                    path.filename().string(),
                    std::chrono::duration_cast<std::chrono::milliseconds>(fileEnd - fileStart).count(),
                    tableCount,
                    insertCount);
    }

    executePendingInsert();
    ShowInfo("Committing SQLite seed cache transaction");
    database.exec("COMMIT");

    const auto executed = std::chrono::steady_clock::now();
    ShowInfoFmt("Built SQLite seed cache at {} with {} tables and {} rows in {} ms",
                cachePath.string(),
                tableCount,
                insertCount,
                std::chrono::duration_cast<std::chrono::milliseconds>(executed - start).count());

    std::filesystem::remove(cachePath);
    std::filesystem::rename(tempPath, cachePath);
}

void seedSQLiteTestDatabase(db::SQLiteDatabase& database)
{
    const auto start = std::chrono::steady_clock::now();
    const auto cachePath = sqliteSeedCachePath();

    if (!sqliteSeedCacheIsFresh(cachePath))
    {
        ShowInfo("SQLite seed cache is missing or stale");
        buildSQLiteSeedCache(cachePath);
    }
    else
    {
        ShowInfoFmt("Using SQLite seed cache at {}", cachePath.string());
    }

    const auto cacheReady = std::chrono::steady_clock::now();
    database.backupFromFile(cachePath.string());

    const auto restored = std::chrono::steady_clock::now();
    ShowInfoFmt("Restored SQLite seed cache into memory in {} ms ({} ms total)",
                std::chrono::duration_cast<std::chrono::milliseconds>(restored - cacheReady).count(),
                std::chrono::duration_cast<std::chrono::milliseconds>(restored - start).count());
}

auto appConfig() -> ApplicationConfig
{
    return ApplicationConfig{
        .serverName = "test",
        .arguments  = {
            ArgumentDefinition{
                .name        = "--keep-going",
                .description = "Continue as much as possible after an error or failure.",
                .type        = ArgumentType::Flag,
            },
            ArgumentDefinition{
                .name        = "--verbose",
                .description = "Verbose output of errors.",
                .type        = ArgumentType::Flag,
            },
            ArgumentDefinition{
                .name        = "--watch",
                .description = "Watch files for changes and re-run tests. (Not yet implemented)",
                .type        = ArgumentType::Flag,
            },
            ArgumentDefinition{
                .name        = "--no-lua-smoke",
                .description = "Skip loading every non-test Lua file during test-server startup.",
                .type        = ArgumentType::Flag,
            },
            ArgumentDefinition{
                .name        = "--daemon-stdio",
                .description = "Keep the warmed test server running and accept JSON-lines commands on stdin.",
                .type        = ArgumentType::Flag,
            },
            ArgumentDefinition{
                .name        = "--sqlite-memory",
                .description = "Use the experimental in-memory SQLite test database backend.",
                .type        = ArgumentType::Flag,
            },
            ArgumentDefinition{
                .name        = "--native-only",
                .description = "Initialize the test server without running Lua tests.",
                .type        = ArgumentType::Flag,
            },
            ArgumentDefinition{
                .name        = "--skip-omega-self-tests",
                .description = "Skip Omega native self-tests after the test server exits.",
                .type        = ArgumentType::Flag,
            },
            ArgumentDefinition{
                .name        = "--omega-self-test",
                .description = "Run registered Omega native self-tests whose names match the regex.",
                .type        = ArgumentType::Multiple,
            },
            ArgumentDefinition{
                .name        = "--tag",
                .description = "Only run tests with these #tags.",
                .type        = ArgumentType::Multiple,
            },
            ArgumentDefinition{
                .name        = "--no-tag",
                .description = "Do not run tests with these #tags, takes precedence over tags.",
                .type        = ArgumentType::Multiple,
            },
            ArgumentDefinition{
                .name        = "--file",
                .description = "Only run test files matching the regex pattern.",
                .type        = ArgumentType::Multiple,
            },
            ArgumentDefinition{
                .name        = "--no-file",
                .description = "Do not run test files matching the regex pattern, takes precedence over file.",
                .type        = ArgumentType::Multiple,
            },
            ArgumentDefinition{
                .name        = "--filter",
                .description = "Only run test names matching the regex pattern.",
                .type        = ArgumentType::Multiple,
            },
            ArgumentDefinition{
                .name        = "--no-filter",
                .description = "Do not run test names matching the regex pattern, takes precedence over filter.",
                .type        = ArgumentType::Multiple,
            },
            ArgumentDefinition{
                .name        = "--output",
                .description = "Output file for test results. Use .json extension for CTRF format.",
            },
        },
    };
}

} // namespace

TestApplication::TestApplication(const int argc, char** argv)
: Application(appConfig(), argc, argv)
, sink_(std::make_shared<InMemorySink>())
{
}

TestApplication::~TestApplication() = default;

auto TestApplication::createEngine() -> std::unique_ptr<Engine>
{
    // Required by Application, but initialized manually in run()
    return nullptr;
}

auto TestApplication::run() -> bool
{
    TracyZoneScoped;
    const bool daemonMode = args().get<bool>("--daemon-stdio");
    const bool nativeOnly = args().get<bool>("--native-only");
    const bool sqliteMemory = args().get<bool>("--sqlite-memory") || args().hasRawArgument("--sqlite-memory");
    ShowInfoFmt("sqliteMemory flag: {}", sqliteMemory);
    static std::unique_ptr<db::SQLiteDatabase> sqliteDatabase;

    scheduler_.postToMainThread(
        [&, daemonMode, sqliteMemory]() -> Task<void>
        {
            try
            {
                // The test harness embeds both the world and map servers in this one process. Route their
                // IPC over inproc:// (a shared, in-process transport) instead of a TCP port.
                settings::set("network.ZMQ_TRANSPORT", std::string("inproc"));

                if (sqliteMemory)
                {
                    sqliteDatabase = std::make_unique<db::SQLiteDatabase>("file:xi_test?mode=memory&cache=shared");
                    db::setDatabase(sqliteDatabase.get());
                    ShowInfo("Using experimental in-memory SQLite test database");
                    seedSQLiteTestDatabase(*sqliteDatabase);
                }

                //
                // Prepare WorldEngine
                //

                auto worldEngine = std::make_unique<WorldEngine>(scheduler_, zmqService_, WorldEngine::EnableHTTPServer::No);

                worldEngine->onInitialize();

                //
                // Prepare MapEngine
                //

                MapConfig mapConfig{
                    .inCI              = true,
                    .isTestServer      = true,
                    .lazyZones         = true,
                    .controlledWeather = true,
                    .smokeLuaFiles     = !args().get<bool>("--no-lua-smoke"),
                };
                auto mapEngine = std::make_unique<MapEngine>(*this, mapConfig);

                // We must ensure that mapEngine->init() is complete before we
                // try and construct TestEngine or run the tests
                scheduler_.blockOnMainThread(mapEngine->init());

                mapEngine->onInitialize();

                //
                // Prepare TestEngine with MapEngine and WorldEngine
                //

                auto testConfig = baseTestConfig();

                engine_ = std::make_unique<TestEngine>(*this, std::move(testConfig), std::move(mapEngine), std::move(worldEngine));

                // From this point, every logging statements end up in the in-memory sink
                // Print to stderr directly if needed
                captureLogger();

                if (nativeOnly)
                {
                    success_ = true;
                    this->requestExit();
                    co_return;
                }

                if (daemonMode)
                {
                    daemonThread_ = std::thread(&TestApplication::runDaemonStdio, this);
                    co_return;
                }

                // Record the result and exit through the normal path so main() can run
                // lua_cleanup() before the process tears down.
                success_ = co_await static_cast<TestEngine*>(engine_.get())->executeTests();
            }
            catch (const std::exception& e)
            {
                ShowCriticalFmt("Fatal test startup exception: {}", e.what());
                success_ = false;
            }
            catch (...)
            {
                ShowCritical("Fatal unknown test startup exception");
                success_ = false;
            }

            this->requestExit();
        });

    try
    {
        scheduler_.run(); // blocks
    }
    catch (const std::exception& e)
    {
        ShowCriticalFmt("Fatal Exception: {}", e.what());
        success_ = false;
    }

    if (daemonThread_.joinable())
    {
        daemonThread_.join();
    }

    return success_;
}

auto TestApplication::baseTestConfig() const -> TestConfig
{
    return TestConfig{
        .loggerSink = sink_,
        .verbose    = args().get<bool>("--verbose"),
        .output     = args().present<std::string>("--output").value_or(""),
        .keepGoing  = args().get<bool>("--keep-going"),
        .watch      = args().get<bool>("--watch"),
        .console    = true,
        .filters    = {
            .includePatterns = args().get<std::vector<std::string>>("--file"),
            .excludePatterns = args().get<std::vector<std::string>>("--no-file"),
            .includeFilters  = args().get<std::vector<std::string>>("--filter"),
            .excludeFilters  = args().get<std::vector<std::string>>("--no-filter"),
            .includeTags     = args().get<std::vector<std::string>>("--tag"),
            .excludeTags     = args().get<std::vector<std::string>>("--no-tag"),
        },
    };
}

void TestApplication::runDaemonStdio()
{
    success_ = true;
    writeDaemonResponse({
        { "event", "ready" },
        { "protocol", 1 },
    });

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line.empty())
        {
            continue;
        }

        nlohmann::json command;
        try
        {
            command = nlohmann::json::parse(line);
        }
        catch (const std::exception& e)
        {
            writeDaemonResponse({
                { "status", "error" },
                { "error", std::format("invalid JSON command: {}", e.what()) },
            });
            continue;
        }

        const auto id = command.value("id", std::string{});
        const auto op = command.value("op", std::string{});

        if (op == "shutdown")
        {
            success_ = true;
            writeDaemonResponse({
                { "id", id },
                { "status", "ok" },
            });
            requestExit();
            break;
        }

        if (op != "run")
        {
            writeDaemonResponse({
                { "id", id },
                { "status", "error" },
                { "error", std::format("unknown daemon op '{}'", op) },
            });
            continue;
        }

        auto testConfig                  = baseTestConfig();
        testConfig.output                = command.value("output", testConfig.output);
        testConfig.keepGoing             = command.value("keepGoing", testConfig.keepGoing);
        testConfig.verbose               = command.value("verbose", testConfig.verbose);
        testConfig.console               = command.value("console", false);
        testConfig.watch                 = false;
        if (command.contains("files"))
        {
            testConfig.filters.includePatterns = jsonStringArray(command, "files");
        }
        if (command.contains("excludeFiles"))
        {
            testConfig.filters.excludePatterns = jsonStringArray(command, "excludeFiles");
        }
        if (command.contains("filters"))
        {
            testConfig.filters.includeFilters = jsonStringArray(command, "filters");
        }
        if (command.contains("excludeFilters"))
        {
            testConfig.filters.excludeFilters = jsonStringArray(command, "excludeFilters");
        }
        if (command.contains("tags"))
        {
            testConfig.filters.includeTags = jsonStringArray(command, "tags");
        }
        if (command.contains("excludeTags"))
        {
            testConfig.filters.excludeTags = jsonStringArray(command, "excludeTags");
        }
        const auto outputPath = testConfig.output;

        auto promise = std::make_shared<std::promise<bool>>();
        auto future  = promise->get_future();

        scheduler_.postToMainThread(
            [this, promise, testConfig = std::move(testConfig)]() mutable -> Task<void>
            {
                try
                {
                    const auto ok = co_await static_cast<TestEngine*>(engine_.get())->executeTests(std::move(testConfig));
                    promise->set_value(ok);
                }
                catch (...)
                {
                    promise->set_exception(std::current_exception());
                }
            });

        try
        {
            const auto ok = future.get();
            auto response = nlohmann::json{
                { "id", id },
                { "status", ok ? "passed" : "failed" },
                { "ok", ok },
            };
            if (auto summary = daemonReportSummary(outputPath); !summary.is_null())
            {
                response["summary"] = std::move(summary);
            }
            writeDaemonResponse(std::move(response));
            success_ = success_ && ok;
        }
        catch (const std::exception& e)
        {
            writeDaemonResponse({
                { "id", id },
                { "status", "error" },
                { "error", e.what() },
            });
            success_ = false;
        }
    }

    requestExit();
}

// Replace all loggers sinks with the in-memory sink
void TestApplication::captureLogger() const
{
    const auto loggerNames = std::vector<std::string>{
        "critical", "error", "lua", "warn", "info", "debug", "trace"
    };

    // spdlog blows up if we don't!
    spdlog::shutdown();

    // Re-register all loggers as SYNCHRONOUS loggers with only our in-memory sink
    // This ensures all logs are immediately written to the sink
    for (const auto& name : loggerNames)
    {
        const auto logger = std::make_shared<spdlog::logger>(name, sink_);
        logger->set_level(spdlog::level::trace);
        spdlog::register_logger(logger);
    }

    // The loggers were just dropped and re-created, so the lock-free loggerFor cache now holds
    // dangling pointers. Re-resolve it before anything logs again.
    logging::RefreshLoggerCache();

    logging::SetPattern(settings::get<std::string>("test.PATTERN"));
}
