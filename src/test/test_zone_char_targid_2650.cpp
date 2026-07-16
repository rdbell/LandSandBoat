#include "test_zone_char_targid_2650.h"

#include "map/zone_capacity.h"

#include <iostream>
#include <set>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone char targid 2650 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runZoneCharTargid2650SelfTests() -> bool
{
    bool ok = true;

    auto allocation = zonehelpers::AllocateCharTargid({});
    ok              = expect(allocation.targid == 0x400 && !allocation.high, "empty starts at 0x400") && ok;

    allocation = zonehelpers::AllocateCharTargid({ 0x400, 0x401, 0x403 });
    ok         = expect(allocation.targid == 0x402 && !allocation.high, "contiguous ids stop at first gap") && ok;

    allocation = zonehelpers::AllocateCharTargid({ 0x3FF, 0x400, 0x401 });
    ok         = expect(allocation.targid == 0x400 && !allocation.high, "lower id stops source scan") && ok;

    std::set<uint16> untilHigh;
    for (uint16 targid = 0x400; targid < 0x700; ++targid)
    {
        untilHigh.insert(targid);
    }
    allocation = zonehelpers::AllocateCharTargid(untilHigh);
    ok         = expect(allocation.targid == 0x700 && allocation.high, "0x6ff advances to high 0x700") && ok;

    return ok;
}
