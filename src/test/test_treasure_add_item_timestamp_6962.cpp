#include "test_treasure_add_item_timestamp_6962.h"

#include "common/timer.h"
#include "map/treasure_pool.h"

#include <iostream>

namespace
{

using namespace std::chrono_literals;

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure addItem timestamp 6962 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Direct CTreasurePool::addItem characterization (slice 6962). Newly added
// items are backdated by treasure_checktime, so their later timeout is based
// on the three-second grace interval used by the source scheduler.
auto runTreasureAddItemTimestamp6962SelfTests() -> bool
{
    CTreasurePool pool(TreasurePoolType::Party);
    const auto    before = timer::now();
    const auto    count  = pool.addItem(1, nullptr);
    const auto    after  = timer::now();
    const auto    stamp  = pool.getItems().front().TimeStamp;

    bool ok = true;
    ok      = expect(count == 1, "item inserts") && ok;
    ok      = expect(stamp >= before - 3s, "timestamp is not earlier than source grace interval") && ok;
    ok      = expect(stamp <= after - 3s, "timestamp is backdated by source grace interval") && ok;
    return ok;
}
