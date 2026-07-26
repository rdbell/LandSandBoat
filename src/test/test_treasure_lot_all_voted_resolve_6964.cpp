#include "test_treasure_lot_all_voted_resolve_6964.h"

#include "map/entities/char_entity.h"
#include "map/item_container.h"
#include "map/treasure_pool.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure lot all voted resolve 6964 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Direct CTreasurePool::lotItem characterization (slice 6964). The first
// party vote leaves a new slot pending; the vote that completes the member
// count immediately checks and resolves it.
auto runTreasureLotAllVotedResolve6964SelfTests() -> bool
{
    CTreasurePool pool(TreasurePoolType::Party);
    CCharEntity   first;
    CCharEntity   second;
    first.PTreasurePool  = &pool;
    second.PTreasurePool = &pool;
    first.getStorage(LOC_INVENTORY)->SetSize(1);
    second.getStorage(LOC_INVENTORY)->SetSize(1);
    pool.addMember(&first);
    pool.addMember(&second);
    first.clearPacketList();
    second.clearPacketList();

    bool ok = true;
    ok      = expect(pool.addItem(1, nullptr) == 1, "item inserts") && ok;
    pool.lotItem(&first, 0, 500);
    ok = expect(pool.itemCount() == 1, "first vote leaves item pending") && ok;
    pool.lotItem(&second, 0, 400);
    ok = expect(pool.itemCount() == 0, "last vote resolves item") && ok;

    first.PTreasurePool  = nullptr;
    second.PTreasurePool = nullptr;
    return ok;
}
