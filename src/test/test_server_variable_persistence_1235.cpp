#include "test_server_variable_persistence_1235.h"

#include "map/utils/serverutils.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "server variable persistence 1235 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runServerVariablePersistence1235SelfTests() -> bool
{
    using serverutils::detail::MakePersistencePlan;
    using serverutils::detail::PersistenceOperation;
    using serverutils::detail::ShouldRetry;

    const auto deletion = MakePersistencePlan(0, 77);
    const auto upsert   = MakePersistencePlan(-4, 99);

    bool ok = true;
    ok      = expect(deletion.operation == PersistenceOperation::Delete, "zero selects delete") && ok;
    ok      = expect(deletion.value == 0 && deletion.expiry == 77, "delete preserves fields") && ok;
    ok      = expect(upsert.operation == PersistenceOperation::Upsert, "nonzero selects upsert") && ok;
    ok      = expect(upsert.value == -4 && upsert.expiry == 99, "upsert preserves fields") && ok;
    ok      = expect(!ShouldRetry(INT_MIN, 5, 1, 0), "retry max zero stops after first write") && ok;
    ok      = expect(!ShouldRetry(INT_MIN, 5, 1, 1), "retry max one is one total write") && ok;
    ok      = expect(ShouldRetry(INT_MIN, 5, 1, 3), "mismatch retries below maximum") && ok;
    ok      = expect(!ShouldRetry(5, 5, 1, 3), "matching verification stops") && ok;
    ok      = expect(!ShouldRetry(INT_MIN, 5, 3, 3), "maximum bounds total writes") && ok;
    return ok;
}
