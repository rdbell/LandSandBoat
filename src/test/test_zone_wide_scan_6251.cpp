#include "test_zone_wide_scan_6251.h"

#include "map/wide_scan_policy.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone wide scan 6251 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Pins CZoneEntities::WideScan list inclusion between ListStart and ListEnd.
auto runZoneWideScan6251SelfTests() -> bool
{
    using widescanhelpers::ShouldIncludeWideScanEntity;
    bool ok = true;
    for (const bool scannable : { false, true })
    {
        for (const bool inRange : { false, true })
        {
            for (const bool sameFloor : { false, true })
            {
                const bool want = scannable && inRange && sameFloor;
                ok              = expect(ShouldIncludeWideScanEntity(scannable, inRange, sameFloor) == want,
                             "wide scan requires scannable, in-range, same-floor entity") &&
                     ok;
            }
        }
    }
    return ok;
}
