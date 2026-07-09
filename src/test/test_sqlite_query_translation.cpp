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

#include "test_sqlite_query_translation.h"

#include "common/database/sqlite/sqlite_connection.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{

struct QueryCase
{
    std::string label;
    std::string query;
    std::string expected;
    std::string secondPass;
};

auto expectEqual(const std::string& actual, const std::string& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "sqlite query translation self-test failed: " << label << "\n"
                  << "  expected: " << expected << "\n"
                  << "  actual:   " << actual << '\n';
        return false;
    }

    return true;
}

} // namespace

auto runSQLiteQueryTranslationSelfTests() -> bool
{
    const std::string unityMembersQuery =
        "UPDATE unity_system JOIN (SELECT unity_leader, COUNT(*) AS members FROM char_profile GROUP BY unity_leader) TMP "
        "ON unity_system.leader = unity_leader SET unity_system.members_current = members";
    const std::string unityMembersExpected =
        "UPDATE unity_system SET members_current = "
        "(SELECT COUNT(*) FROM char_profile WHERE char_profile.unity_leader = unity_system.leader)";
    const std::string titleUpdateQuery =
        "UPDATE chars LEFT JOIN char_stats USING(charid) SET titles = ?, title = ? WHERE charid = ? LIMIT 1";

    const std::vector<QueryCase> cases{
        { "empty", "", "" },
        { "ordinary query passthrough", "SELECT * FROM chars WHERE charid = ?", "SELECT * FROM chars WHERE charid = ?" },
        { "insert set", "INSERT INTO chars SET charid = ?, charname = ?", "INSERT INTO chars (charid, charname) VALUES (?, ?)" },
        { "insert set mixed case", "insert into chars SeT name = ?, nation = 1", "INSERT INTO chars (name, nation) VALUES (?, 1)" },
        { "insert set quoted commas and functions",
          "INSERT INTO audit_log SET message = 'alpha,beta', payload = COALESCE(?, JSON_OBJECT('x', 1)), attempts = 2",
          "INSERT INTO audit_log (message, payload, attempts) VALUES ('alpha,beta', COALESCE(?, JSON_OBJECT('x', 1)), 2)" },
        { "insert set upsert values",
          "INSERT INTO chars SET charid = ?, charname = ? ON DUPLICATE KEY UPDATE charname = VALUES(`charname`)",
          "INSERT INTO chars (charid, charname) VALUES (?, ?) ON CONFLICT DO UPDATE SET charname = excluded.charname",
          "INSERT INTO chars (charid, charname) VALUES (?, ?) ON CONFLICT DO UPDATE (charname) VALUES (excluded.charname)" },
        { "upsert flexible whitespace",
          "INSERT INTO records (id, value) VALUES (?, ?)\n on   duplicate key   update value = values ( value )",
          "INSERT INTO records (id, value) VALUES (?, ?) ON CONFLICT DO UPDATE SET value = excluded.value",
          "INSERT INTO records (id, value) VALUES (?, ?) ON CONFLICT DO UPDATE (value) VALUES (excluded.value)" },
        { "date add thirty days",
          "SELECT date_add ( now ( ), interval 30 day )",
          "SELECT datetime(CURRENT_TIMESTAMP, '+30 day')" },
        { "now spellings",
          "SELECT NOW(), now(), CURRENT_TIMESTAMP(), current_timestamp(), NoW()",
          "SELECT CURRENT_TIMESTAMP, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP, CURRENT_TIMESTAMP, NoW()" },
        { "datediff current timestamp",
          "SELECT DaTeDiFf ( CURRENT_TIMESTAMP, chars.updated_at )",
          "SELECT CAST(julianday(CURRENT_TIMESTAMP) - julianday(chars.updated_at) AS INTEGER)" },
        { "unix timestamp now",
          "SELECT UNIX_TIMESTAMP()",
          "SELECT CAST(strftime('%s', 'now') AS INTEGER)" },
        { "unix timestamp backtick alias",
          "SELECT unix_timestamp ( `lastupdate` ) AS stamp",
          "SELECT CAST(strftime('%s', `lastupdate`) AS INTEGER) AS stamp" },
        { "unix timestamp identifier alias",
          "SELECT UNIX_TIMESTAMP(chars.lastupdate) as stamp",
          "SELECT CAST(strftime('%s', chars.lastupdate) AS INTEGER) AS stamp" },
        { "unix timestamp backtick default alias",
          "SELECT UNIX_TIMESTAMP(`lastupdate`)",
          "SELECT CAST(strftime('%s', `lastupdate`) AS INTEGER) AS \"UNIX_TIMESTAMP(`lastupdate`)\"",
          "SELECT CAST(strftime('%s', `lastupdate`) AS INTEGER) AS \"CAST(strftime('%s', `lastupdate`) AS INTEGER) AS \"UNIX_TIMESTAMP(`lastupdate`)\"\"" },
        { "unix timestamp identifier default alias",
          "SELECT UNIX_TIMESTAMP(chars.lastupdate)",
          "SELECT CAST(strftime('%s', chars.lastupdate) AS INTEGER) AS \"UNIX_TIMESTAMP(chars.lastupdate)\"",
          "SELECT CAST(strftime('%s', chars.lastupdate) AS INTEGER) AS \"CAST(strftime('%s', chars.lastupdate) AS INTEGER) AS \"UNIX_TIMESTAMP(chars.lastupdate)\"\"" },
        { "if function",
          "SELECT IF (active, 1, 0), DIFF(value)",
          "SELECT IIF(active, 1, 0), DIFF(value)" },
        { "insert ignore",
          "\n  InSeRt   IgNoRe InTo records (id) VALUES (?)",
          "\n  INSERT OR IGNORE INTO records (id) VALUES (?)" },
        { "insert trailing limit",
          "INSERT INTO audit SELECT * FROM source limit 1   ",
          "INSERT INTO audit SELECT * FROM source" },
        { "location field ordering",
          "SELECT itemid FROM inventory ORDER BY FIELD(location,0,1,9,2,3,4,5,6,7,8,10,11,12)",
          "SELECT itemid FROM inventory ORDER BY CASE location WHEN 0 THEN 0 WHEN 1 THEN 1 WHEN 9 THEN 2 WHEN 2 THEN 3 WHEN 3 THEN 4 WHEN 4 THEN 5 WHEN 5 THEN 6 WHEN 6 THEN 7 WHEN 7 THEN 8 WHEN 8 THEN 9 WHEN 10 THEN 10 WHEN 11 THEN 11 WHEN 12 THEN 12 ELSE 13 END" },
        { "unity members exact rewrite", unityMembersQuery, unityMembersExpected },
        { "title exact rewrite", titleUpdateQuery, "UPDATE chars SET titles = ?, title = ? WHERE charid = ?" },
        { "exact rewrite is case sensitive",
          "update chars left join char_stats using(charid) set titles = ?, title = ? where charid = ? limit 1",
          "update chars left join char_stats using(charid) set titles = ?, title = ? where charid = ? limit 1" },
        { "malformed insert set passthrough",
          "INSERT INTO chars SET charid, charname = ?",
          "INSERT INTO chars SET charid, charname = ?" },
        { "empty insert set passthrough", "INSERT INTO chars SET ", "INSERT INTO chars SET " },
        { "leading whitespace insert set passthrough",
          " INSERT INTO chars SET charid = ?",
          " INSERT INTO chars SET charid = ?" },
        { "field replacement is exact",
          "SELECT FIELD(location, 0, 1, 9, 2)",
          "SELECT FIELD(location, 0, 1, 9, 2)" },
    };

    bool ok = true;
    for (const auto& test : cases)
    {
        const auto translated = db::detail::sqlite::translateQuery(test.query);
        ok                    = expectEqual(translated, test.expected, test.label) && ok;
        const auto expectedSecondPass = test.secondPass.empty() ? translated : test.secondPass;
        ok = expectEqual(db::detail::sqlite::translateQuery(translated), expectedSecondPass, test.label + " second pass") && ok;
    }

    return ok;
}
