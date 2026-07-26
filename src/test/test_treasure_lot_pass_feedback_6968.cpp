#include "test_treasure_lot_pass_feedback_6968.h"

#include "map/enums/packet_s2c.h"
#include "map/entities/char_entity.h"
#include "map/item_container.h"
#include "map/packets/s2c/0x0d3_trophy_solution.h"
#include "map/treasure_pool.h"

#include <cstddef>
#include <iostream>

namespace
{

constexpr auto lootUniqueNoOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_SOLUTION::PacketData, LootUniqueNo);
constexpr auto lootPointOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_SOLUTION::PacketData, LootPoint);
constexpr auto entryPointOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_SOLUTION::PacketData, EntryPoint);
constexpr auto slotOffset       = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_SOLUTION::PacketData, TrophyItemIndex);

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure lot/pass feedback 6968 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectFeedback(CCharEntity& member, int16 highestLot, int16 actionLot) -> bool
{
    bool ok = expect(member.getPacketCount() == 1, "each member receives one feedback packet");
    if (!ok)
    {
        return false;
    }
    const auto& packet = member.getPacketList().front();
    return expect(packet->getType() == static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_TROPHY_SOLUTION), "packet is TROPHY_SOLUTION") &&
           expect(packet->ref<int16>(lootPointOffset) == highestLot, "packet records current highest lot") &&
           expect(packet->ref<int16>(entryPointOffset) == actionLot, "packet records action lot or pass mask") &&
           expect(packet->ref<uint8>(slotOffset) == 0, "packet records slot");
}

} // namespace

// Direct CTreasurePool::lotItem / passItem characterization (slice 6968).
// Each action broadcasts a TROPHY_SOLUTION update. A pass is encoded as the
// 0xFFFF mask and removes the former highest lot from the feedback packet.
auto runTreasureLotPassFeedback6968SelfTests() -> bool
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
    pool.addItem(1, nullptr);
    first.clearPacketList();
    second.clearPacketList();

    pool.lotItem(&first, 0, 500);
    bool ok = expectFeedback(first, 500, 500) && expectFeedback(second, 500, 500);

    first.clearPacketList();
    second.clearPacketList();
    pool.passItem(&first, 0);
    ok = expectFeedback(first, 0, -1) && ok;
    ok = expectFeedback(second, 0, -1) && ok;

    first.PTreasurePool  = nullptr;
    second.PTreasurePool = nullptr;

    // Equal lots retain the first recorded lotter as the displayed highest.
    {
        CTreasurePool tiePool(TreasurePoolType::Party);
        CCharEntity   tieFirst;
        CCharEntity   tieSecond;
        CCharEntity   tieThird;
        tieFirst.id  = 101;
        tieSecond.id = 202;
        tieFirst.PTreasurePool  = &tiePool;
        tieSecond.PTreasurePool = &tiePool;
        tieThird.PTreasurePool  = &tiePool;
        tieFirst.getStorage(LOC_INVENTORY)->SetSize(1);
        tieSecond.getStorage(LOC_INVENTORY)->SetSize(1);
        tieThird.getStorage(LOC_INVENTORY)->SetSize(1);
        tiePool.addMember(&tieFirst);
        tiePool.addMember(&tieSecond);
        tiePool.addMember(&tieThird);
        tieFirst.clearPacketList();
        tieSecond.clearPacketList();
        tieThird.clearPacketList();
        tiePool.addItem(1, nullptr);
        tieFirst.clearPacketList();
        tieSecond.clearPacketList();
        tieThird.clearPacketList();
        tiePool.lotItem(&tieFirst, 0, 500);
        tieFirst.clearPacketList();
        tieSecond.clearPacketList();
        tieThird.clearPacketList();
        tiePool.lotItem(&tieSecond, 0, 500);

        for (CCharEntity* member : { &tieFirst, &tieSecond, &tieThird })
        {
            ok = expect(member->getPacketCount() == 1, "tied lot broadcasts one feedback packet") && ok;
            if (member->getPacketCount() == 1)
            {
                ok = expect(member->getPacketList().front()->ref<uint32>(lootUniqueNoOffset) == tieFirst.id, "first tied lotter remains highest") && ok;
            }
        }

        tieFirst.PTreasurePool  = nullptr;
        tieSecond.PTreasurePool = nullptr;
        tieThird.PTreasurePool  = nullptr;
    }
    return ok;
}
