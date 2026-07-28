#include "test_treasure_add_item_rare_admission_6971.h"

#include "map/entities/char_entity.h"
#include "map/item_container.h"
#include "map/treasure_pool.h"
#include "map/utils/charutils.h"

#include <iostream>

namespace
{

constexpr uint16 RareItemID = 28; // drawing desk: @FLAG_RARE in item_basic.sql

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure addItem rare admission 6971 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Direct CTreasurePool::addItem characterization (slice 6971). A rare item is
// rejected only when every pool member already owns it; one missing owner lets
// the pool add the item normally.
auto runTreasureAddItemRareAdmission6971SelfTests() -> bool
{
    bool ok = true;
    {
        CTreasurePool pool(TreasurePoolType::Party);
        CCharEntity   first;
        CCharEntity   second;
        first.id             = 6971001;
        second.id            = 6971002;
        first.PTreasurePool  = &pool;
        second.PTreasurePool = &pool;
        first.getStorage(LOC_INVENTORY)->SetSize(2);
        second.getStorage(LOC_INVENTORY)->SetSize(2);
        charutils::AddItem(&first, LOC_INVENTORY, RareItemID, 1, true);
        charutils::AddItem(&second, LOC_INVENTORY, RareItemID, 1, true);
        pool.addMember(&first);
        pool.addMember(&second);
        first.clearPacketList();
        second.clearPacketList();

        ok                   = expect(pool.addItem(RareItemID, nullptr) == 0 && pool.itemCount() == 0, "all owners reject rare item") && ok;
        first.PTreasurePool  = nullptr;
        second.PTreasurePool = nullptr;
    }
    {
        CTreasurePool pool(TreasurePoolType::Party);
        CCharEntity   first;
        CCharEntity   second;
        first.id             = 6971003;
        second.id            = 6971004;
        first.PTreasurePool  = &pool;
        second.PTreasurePool = &pool;
        first.getStorage(LOC_INVENTORY)->SetSize(2);
        second.getStorage(LOC_INVENTORY)->SetSize(2);
        charutils::AddItem(&first, LOC_INVENTORY, RareItemID, 1, true);
        pool.addMember(&first);
        pool.addMember(&second);
        first.clearPacketList();
        second.clearPacketList();

        ok                   = expect(pool.addItem(RareItemID, nullptr) == 1 && pool.itemCount() == 1, "missing owner admits rare item") && ok;
        first.PTreasurePool  = nullptr;
        second.PTreasurePool = nullptr;
    }
    return ok;
}
