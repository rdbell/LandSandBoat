#include "test_treasure_pass_all_voted_resolve_6965.h"

#include "map/entities/char_entity.h"
#include "map/treasure_pool.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure pass all voted resolve 6965 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Direct CTreasurePool::passItem characterization (slice 6965). The first
// party pass leaves the slot pending; the pass completing member-count entries
// immediately resolves the item, and with no eligible candidates it is lost.
auto runTreasurePassAllVotedResolve6965SelfTests() -> bool
{
    CTreasurePool pool(TreasurePoolType::Party);
    CCharEntity   first;
    CCharEntity   second;
    first.PTreasurePool  = &pool;
    second.PTreasurePool = &pool;
    pool.addMember(&first);
    pool.addMember(&second);
    first.clearPacketList();
    second.clearPacketList();

    bool ok = true;
    ok      = expect(pool.addItem(1, nullptr) == 1, "item inserts") && ok;
    pool.passItem(&first, 0);
    ok = expect(pool.itemCount() == 1, "first pass leaves item pending") && ok;
    pool.passItem(&second, 0);
    ok = expect(pool.itemCount() == 0, "last pass resolves lost item") && ok;

    first.PTreasurePool  = nullptr;
    second.PTreasurePool = nullptr;
    return ok;
}
