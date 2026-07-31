#include "test_map_dynamic_targid_percent_2769.h"

#include "map/map_networking_capacity.h"

#include <cstdint>
#include <iostream>
#include <limits>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map dynamic targid percent 2769 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runMapDynamicTargIdPercent2769SelfTests() -> bool
{
    using mapnetworkinghelpers::AccumulateDynamicTargIdCapacity;
    using mapnetworkinghelpers::DynamicTargIdCapacityPerZone;
    using mapnetworkinghelpers::DynamicTargIdUsagePercent;

    bool ok = true;

    ok = expect(DynamicTargIdCapacityPerZone == 511, "capacity per zone is 511") && ok;

    ok = expect(AccumulateDynamicTargIdCapacity(0) == 0, "accumulate 0 zones") && ok;
    ok = expect(AccumulateDynamicTargIdCapacity(1) == 511, "accumulate 1 zone") && ok;
    ok = expect(AccumulateDynamicTargIdCapacity(2) == 1022, "accumulate 2 zones") && ok;
    ok = expect(AccumulateDynamicTargIdCapacity(3) == 1533, "accumulate 3 zones") && ok;

    // capacity == 0 → 0 (including both zero)
    ok = expect(DynamicTargIdUsagePercent(0, 0) == 0, "0/0 → 0") && ok;
    ok = expect(DynamicTargIdUsagePercent(1, 0) == 0, "1/0 → 0") && ok;
    ok = expect(DynamicTargIdUsagePercent(100, 0) == 0, "100/0 → 0") && ok;

    // empty usage
    ok = expect(DynamicTargIdUsagePercent(0, 511) == 0, "0/511 → 0") && ok;
    ok = expect(DynamicTargIdUsagePercent(0, 1022) == 0, "0/1022 → 0") && ok;

    // truncation toward zero of (double)count/(double)capacity*100.0
    // 1/511*100 ≈ 0.195... → 0
    ok = expect(DynamicTargIdUsagePercent(1, 511) == 0, "1/511 → 0 (trunc)") && ok;
    // 255/511*100 ≈ 49.902... → 49
    ok = expect(DynamicTargIdUsagePercent(255, 511) == 49, "255/511 → 49") && ok;
    // full single zone
    ok = expect(DynamicTargIdUsagePercent(511, 511) == 100, "511/511 → 100") && ok;
    // slightly over one zone's capacity against one-zone denominator
    ok = expect(DynamicTargIdUsagePercent(512, 511) == 100, "512/511 → 100 (trunc)") && ok;

    // multi-zone capacity (2 active zones → 1022)
    ok = expect(DynamicTargIdUsagePercent(511, 1022) == 50, "511/1022 → 50") && ok;
    ok = expect(DynamicTargIdUsagePercent(1022, 1022) == 100, "1022/1022 → 100") && ok;
    // 1/1022*100 ≈ 0.097... → 0
    ok = expect(DynamicTargIdUsagePercent(1, 1022) == 0, "1/1022 → 0 (trunc)") && ok;

    // composition: accumulate then percent matches flushStatistics wiring
    const auto capacity2 = AccumulateDynamicTargIdCapacity(2);
    ok                   = expect(capacity2 == 1022, "compose capacity") && ok;
    ok                   = expect(DynamicTargIdUsagePercent(200, capacity2) == DynamicTargIdUsagePercent(200, 1022), "compose percent") && ok;

    // Both inputs are std::size_t in LSB. Pin the native-width conversion for
    // a synthetic negative count without requiring a fixed host word size.
    const auto maxCapacity = std::numeric_limits<std::int64_t>::max();
    const auto expectedNegativeCount = static_cast<std::int64_t>(
        static_cast<double>(static_cast<std::size_t>(-1)) /
        static_cast<double>(static_cast<std::size_t>(maxCapacity)) * 100.0);
    ok = expect(DynamicTargIdUsagePercent(-1, maxCapacity) == expectedNegativeCount,
               "negative count uses native size_t conversion") &&
         ok;
    const auto expectedNegativeCapacity = static_cast<std::int64_t>(
        static_cast<double>(static_cast<std::size_t>(maxCapacity)) /
        static_cast<double>(static_cast<std::size_t>(-1)) * 100.0);
    ok = expect(DynamicTargIdUsagePercent(maxCapacity, -1) == expectedNegativeCapacity,
               "negative capacity uses native size_t conversion") &&
         ok;

    return ok;
}
