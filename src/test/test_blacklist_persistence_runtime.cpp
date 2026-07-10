#include "test_blacklist_persistence_runtime.h"

#include "common/database/database.h"
#include "common/database/libmariadb/libmariadb_result_set.h"
#include "map/utils/blacklistutils.h"

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace
{
class FakeDatabase final : public db::Database
{
public:
    auto execute(const std::string& query, const std::vector<db::BoundValue>& params) -> std::unique_ptr<db::ResultSet> override
    {
        queries.push_back(query);
        bindings.push_back(params);
        if (results.empty())
        {
            return nullptr;
        }
        auto result = std::move(results.front());
        results.erase(results.begin());
        return result;
    }

    auto getSchema() -> std::string override { return "xidb"; }
    auto getVersion() -> std::string override { return "fake"; }
    auto getDriverVersion() -> std::string override { return "fake"; }

    std::vector<std::string>                    queries;
    std::vector<std::vector<db::BoundValue>>    bindings;
    std::vector<std::unique_ptr<db::ResultSet>> results;
};

struct DatabaseGuard
{
    ~DatabaseGuard() { db::setDatabase(nullptr); }
};

auto updateResult(std::size_t affected) -> std::unique_ptr<db::ResultSet>
{
    return std::make_unique<db::LibMariaDBResultSet>(affected, "mutation");
}

auto membershipResult(bool found) -> std::unique_ptr<db::ResultSet>
{
    auto schema          = std::make_shared<db::ColumnSchema>();
    schema->names        = { "charid_owner", "charid_target" };
    schema->index["charid_owner"]  = 0;
    schema->index["charid_target"] = 1;
    auto rows = std::vector<db::LibMariaDBResultSet::Row>{};
    if (found)
    {
        rows.push_back({ static_cast<uint64>(10), static_cast<uint64>(20) });
    }
    return std::make_unique<db::LibMariaDBResultSet>("membership", std::move(schema), std::move(rows));
}

auto expect(bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "blacklist persistence runtime self-test failed: " << label << '\n';
    }
    return condition;
}

auto boundID(const db::BoundValue& value, uint32 expected) -> bool
{
    return std::holds_alternative<uint32>(value) && std::get<uint32>(value) == expected;
}

auto testMembershipAndNullFailure() -> bool
{
    DatabaseGuard guard;
    FakeDatabase  fake;
    fake.results.push_back(membershipResult(true));
    fake.results.push_back(membershipResult(false));
    fake.results.push_back(nullptr);
    db::setDatabase(&fake);

    bool ok = true;
    ok = expect(blacklistutils::IsBlacklisted(10, 20), "existing membership") && ok;
    ok = expect(!blacklistutils::IsBlacklisted(10, 21), "missing membership") && ok;
    ok = expect(!blacklistutils::IsBlacklisted(10, 22), "null result is false") && ok;
    ok = expect(fake.queries.size() == 3 && fake.queries[0] ==
                    "SELECT * FROM char_blacklist WHERE charid_owner = ? AND charid_target = ? LIMIT 1",
                "membership query") && ok;
    ok = expect(boundID(fake.bindings[0][0], 10) && boundID(fake.bindings[0][1], 20), "membership bindings") && ok;
    return ok;
}

auto testAddPersistenceSemantics() -> bool
{
    DatabaseGuard guard;
    FakeDatabase  fake;
    fake.results.push_back(membershipResult(true));
    fake.results.push_back(membershipResult(false));
    fake.results.push_back(updateResult(1));
    fake.results.push_back(membershipResult(false));
    fake.results.push_back(updateResult(0));
    fake.results.push_back(membershipResult(false));
    fake.results.push_back(nullptr);
    db::setDatabase(&fake);

    bool ok = true;
    ok = expect(!blacklistutils::AddBlacklisted(10, 20), "duplicate skips insert") && ok;
    ok = expect(blacklistutils::AddBlacklisted(10, 21), "affected insert succeeds") && ok;
    ok = expect(!blacklistutils::AddBlacklisted(10, 22), "zero affected insert fails") && ok;
    ok = expect(!blacklistutils::AddBlacklisted(10, 23), "null insert fails") && ok;
    ok = expect(fake.queries.size() == 7, "duplicate performed only existence query") && ok;
    ok = expect(fake.queries[2] == "INSERT INTO char_blacklist (charid_owner, charid_target) VALUES (?, ?)", "insert query") && ok;
    ok = expect(boundID(fake.bindings[2][0], 10) && boundID(fake.bindings[2][1], 21), "insert bindings") && ok;
    return ok;
}

auto testDeletePersistenceSemantics() -> bool
{
    DatabaseGuard guard;
    FakeDatabase  fake;
    fake.results.push_back(membershipResult(false));
    fake.results.push_back(membershipResult(true));
    fake.results.push_back(updateResult(1));
    fake.results.push_back(membershipResult(true));
    fake.results.push_back(updateResult(0));
    fake.results.push_back(membershipResult(true));
    fake.results.push_back(nullptr);
    db::setDatabase(&fake);

    bool ok = true;
    ok = expect(!blacklistutils::DeleteBlacklisted(10, 20), "missing skips delete") && ok;
    ok = expect(blacklistutils::DeleteBlacklisted(10, 21), "affected delete succeeds") && ok;
    ok = expect(!blacklistutils::DeleteBlacklisted(10, 22), "zero affected delete fails") && ok;
    ok = expect(!blacklistutils::DeleteBlacklisted(10, 23), "null delete fails") && ok;
    ok = expect(fake.queries.size() == 7, "missing performed only existence query") && ok;
    ok = expect(fake.queries[2] == "DELETE FROM char_blacklist WHERE charid_owner = ? AND charid_target = ? LIMIT 1", "delete query") && ok;
    ok = expect(boundID(fake.bindings[2][0], 10) && boundID(fake.bindings[2][1], 21), "delete bindings") && ok;
    return ok;
}
} // namespace

auto runBlacklistPersistenceRuntimeSelfTests() -> bool
{
    bool ok = true;
    ok = testMembershipAndNullFailure() && ok;
    ok = testAddPersistenceSemantics() && ok;
    ok = testDeletePersistenceSemantics() && ok;
    ok = expect(blacklistutils::detail::IsNameCharactersOnly("Alpha"), "ASCII name accepted") && ok;
    ok = expect(!blacklistutils::detail::IsNameCharactersOnly("Alpha-1"), "punctuated name rejected") && ok;
    ok = expect(!blacklistutils::detail::IsNameCharactersOnly(std::string{ "a\0b", 3 }), "embedded NUL rejected") && ok;
    const auto complete = blacklistutils::detail::FullChunkFlags(12, 12);
    const auto filtered = blacklistutils::detail::FullChunkFlags(12, 13);
    ok = expect(complete.first && complete.second, "complete first chunk resets and finishes") && ok;
    ok = expect(filtered.first && !filtered.second, "filtered row suppresses last flag") && ok;
    return ok;
}
