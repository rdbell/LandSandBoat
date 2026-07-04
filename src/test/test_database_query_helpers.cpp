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

#include "test_database_query_helpers.h"

#include "common/database/database.h"
#include "common/database/query_validation.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{

template <typename T, typename U>
auto expectEqual(const T actual, const U expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "database query helpers self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

struct KeywordCase
{
    std::string       query;
    db::ResultSetType expected;
};

struct BoolCase
{
    std::string query;
    bool        expected;
};

struct EscapeCase
{
    std::string input;
    std::string expected;
};

auto testKeywordClassification() -> bool
{
    bool ok = true;

    const std::vector<KeywordCase> cases{
        { "select * from chars", db::ResultSetType::Select },
        { " SHOW TABLES", db::ResultSetType::Select },
        { "\nselect 1", db::ResultSetType::Select },
        { "INSERT INTO chars VALUES (?)", db::ResultSetType::Update },
        { "update chars set name = ?", db::ResultSetType::Update },
        { "DELETE FROM chars", db::ResultSetType::Update },
        { "replace into chars values (?)", db::ResultSetType::Update },
        { "CREATE TABLE tmp (id int)", db::ResultSetType::Update },
        { "alter table chars add column tmp int", db::ResultSetType::Update },
        { "DROP TABLE tmp", db::ResultSetType::Update },
        { "TRUNCATE TABLE tmp", db::ResultSetType::Update },
        { "SET autocommit = 0", db::ResultSetType::Update },
        { "START TRANSACTION", db::ResultSetType::Update },
        { "COMMIT", db::ResultSetType::Update },
        { "ROLLBACK", db::ResultSetType::Update },
        { "", db::ResultSetType::Invalid },
        { "   ", db::ResultSetType::Invalid },
        { "\n", db::ResultSetType::Invalid },
        { "WITH cte AS (SELECT 1) SELECT * FROM cte", db::ResultSetType::Invalid },
    };

    for (const auto& test : cases)
    {
        ok = expectEqual(db::detail::validateQueryLeadingKeyword(test.query), test.expected, "keyword " + test.query) && ok;
    }

    return ok;
}

auto testQueryContentValidation() -> bool
{
    bool ok = true;

    const std::vector<BoolCase> cases{
        { "SELECT * FROM chars WHERE name LIKE '%Omega%'", true },
        { "SELECT {}", false },
        { "SELECT * FROM chars; DROP TABLE chars", false },
    };

    for (const auto& test : cases)
    {
        ok = expectEqual(db::detail::validateQueryContent(test.query), test.expected, "content " + test.query) && ok;
    }

    return ok;
}

auto testEscapeString() -> bool
{
    bool ok = true;

    const std::vector<EscapeCase> cases{
        { "", "" },
        { "plain", "plain" },
        { "back\\slash", "back\\\\slash" },
        { "line\nreturn\r", "line\\nreturn\\r" },
        { "'\"", "\\'\\\"" },
        { std::string{ 'a', '\x1a', '\b', '%', '|', ';' }, "a\\Z\\b\\%\\|\\;" },
        { std::string("abc\0def\n", 8), "abc" },
    };

    for (const auto& test : cases)
    {
        ok = expectEqual(db::escapeString(test.input), test.expected, "escape string") && ok;
    }

    ok = expectEqual(db::escapeString(static_cast<const char*>(nullptr)), std::string(""), "escape null c-string") && ok;

    return ok;
}

} // namespace

auto runDatabaseQueryHelpersSelfTests() -> bool
{
    bool ok = true;

    ok = testKeywordClassification() && ok;
    ok = testQueryContentValidation() && ok;
    ok = testEscapeString() && ok;

    return ok;
}
