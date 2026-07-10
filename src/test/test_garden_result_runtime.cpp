#include "test_garden_result_runtime.h"

#include "map/utils/gardenutils.h"

#include <cstdint>
#include <iostream>
#include <vector>

namespace
{
struct ResultRow
{
    uint16 resultId;
    uint8  seed;
    uint8  element1;
    uint8  element2;
    uint16 itemId;
    uint8  minQuantity;
    uint8  maxQuantity;
    uint8  weight;
};

void loadRows(gardenutils::detail::ResultMap& results, const std::vector<ResultRow>& rows)
{
    for (const auto& row : rows)
    {
        gardenutils::detail::AppendResult(results, row.seed, row.element1, row.element2,
                                          row.itemId, row.minQuantity, row.maxQuantity, row.weight);
    }
}

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "garden result runtime self-test failed: " << label << '\n';
    }
    return condition;
}

auto testGroupingAndIterationOrder() -> bool
{
    gardenutils::detail::ResultMap results;
    loadRows(results, { { 91, 2, 1, 3, 100, 1, 2, 10 },
                        { 7, 1, 0, 0, 300, 3, 5, 30 },
                        { 12, 2, 1, 3, 200, 2, 4, 20 } });
    const auto& grouped = results[gardenutils::detail::ResultKey(2, 1, 3)];
    bool ok = true;
    ok = expect(grouped.size() == 2 && grouped[0].ItemID == 100 && grouped[1].ItemID == 200,
                "rows grouped in result-set order") && ok;
    ok = expect(results[gardenutils::detail::ResultKey(1, 0, 0)].front().ItemID == 300, "independent feed key") && ok;
    return ok;
}

auto testRepeatedLoadAndMissingLookup() -> bool
{
    gardenutils::detail::ResultMap results;
    const std::vector<ResultRow> rows{ { 1, 2, 0, 0, 100, 1, 1, 10 } };
    loadRows(results, rows);
    loadRows(results, rows);
    bool ok = true;
    ok = expect(results[gardenutils::detail::ResultKey(2, 0, 0)].size() == 2, "repeated load appends duplicates") && ok;
    const auto missingKey = gardenutils::detail::ResultKey(9, 8, 7);
    ok = expect(results.find(missingKey) == results.end(), "missing key absent before operator lookup") && ok;
    (void)results[missingKey];
    ok = expect(results.find(missingKey) != results.end() && results[missingKey].empty(),
                "operator lookup materializes empty list") && ok;
    return ok;
}
} // namespace

auto runGardenResultRuntimeSelfTests() -> bool
{
    bool ok = true;
    ok = testGroupingAndIterationOrder() && ok;
    ok = testRepeatedLoadAndMissingLookup() && ok;
    return ok;
}
