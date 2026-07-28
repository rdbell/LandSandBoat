#include "test_treasure_terminal_resolution_dispatch_6970.h"

#include "map/entities/char_entity.h"
#include "map/enums/packet_s2c.h"
#include "map/item_container.h"
#include "map/packets/s2c/0x0d3_trophy_solution.h"
#include "map/treasure_pool.h"

#include <cstddef>
#include <iostream>

namespace
{

constexpr auto lootUniqueNoOffset = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_SOLUTION::PacketData, LootUniqueNo);
constexpr auto lootPointOffset    = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_SOLUTION::PacketData, LootPoint);
constexpr auto judgeOffset        = sizeof(GP_SERV_HEADER) + offsetof(GP_SERV_COMMAND_TROPHY_SOLUTION::PacketData, JudgeFlg);

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure terminal resolution dispatch 6970 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Direct CTreasurePool::lotItem/checkTreasureItem characterization (slice
// 6970). The all-voted lot broadcasts feedback before the terminal Win packet
// naming the highest lotter. The winner also receives two inventory packets.
auto runTreasureTerminalResolutionDispatch6970SelfTests() -> bool
{
    CTreasurePool pool(TreasurePoolType::Party);
    CCharEntity   first;
    CCharEntity   second;
    first.id             = 6970001;
    second.id            = 6970002;
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
    first.clearPacketList();
    second.clearPacketList();
    pool.lotItem(&second, 0, 400);

    bool ok = expect(pool.itemCount() == 0, "last vote resolves item") &&
              expect(first.getPacketCount() == 4 && second.getPacketCount() == 2, "winner receives inventory updates and both members receive terminal packet");
    for (CCharEntity* member : { &first, &second })
    {
        if (member->getPacketCount() == 2)
        {
            const auto& terminal = member->getPacketList().back();
            ok                   = expect(terminal->getType() == static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_TROPHY_SOLUTION), "terminal packet is TROPHY_SOLUTION") && ok;
            ok                   = expect(terminal->ref<uint32>(lootUniqueNoOffset) == first.id, "terminal packet names winner") && ok;
            ok                   = expect(terminal->ref<int16>(lootPointOffset) == 0, "terminal Win has zero lot point") && ok;
            ok                   = expect(terminal->ref<uint8>(judgeOffset) == static_cast<uint8>(GP_TROPHY_SOLUTION_STATE::Win), "terminal packet is Win") && ok;
        }
    }

    first.PTreasurePool  = nullptr;
    second.PTreasurePool = nullptr;
    return ok;
}
