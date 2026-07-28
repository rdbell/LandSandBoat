#include "test_treasure_add_item_solo_resolve_6963.h"

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
        std::cerr << "treasure addItem solo resolve 6963 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Direct CTreasurePool::addItem characterization (slice 6963). The source
// immediately resolves a newly inserted slot for exactly one member when that
// member has inventory space; a two-member pool leaves it for normal lot/pass.
auto runTreasureAddItemSoloResolve6963SelfTests() -> bool
{
    bool ok = true;

    {
        CTreasurePool pool(TreasurePoolType::Solo);
        CCharEntity   member;
        member.id            = 6963001;
        member.PTreasurePool = &pool;
        member.getStorage(LOC_INVENTORY)->SetSize(1);
        pool.addMember(&member);
        member.clearPacketList();

        ok                   = expect(pool.addItem(1, nullptr) == 0, "solo returns post-resolution count") && ok;
        ok                   = expect(pool.itemCount() == 0, "solo member with space resolves immediately") && ok;
        member.PTreasurePool = nullptr;
    }

    {
        CTreasurePool pool(TreasurePoolType::Party);
        CCharEntity   first;
        CCharEntity   second;
        first.id             = 6963002;
        second.id            = 6963003;
        first.PTreasurePool  = &pool;
        second.PTreasurePool = &pool;
        first.getStorage(LOC_INVENTORY)->SetSize(1);
        second.getStorage(LOC_INVENTORY)->SetSize(1);
        pool.addMember(&first);
        pool.addMember(&second);
        first.clearPacketList();
        second.clearPacketList();

        ok                   = expect(pool.addItem(1, nullptr) == 1, "party inserts") && ok;
        ok                   = expect(pool.itemCount() == 1, "multiple members defer resolution") && ok;
        first.PTreasurePool  = nullptr;
        second.PTreasurePool = nullptr;
    }

    return ok;
}
