#include "test_treasure_add_member_update_6966.h"

#include "map/entities/char_entity.h"
#include "map/enums/packet_s2c.h"
#include "map/treasure_pool.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure addMember update 6966 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Direct CTreasurePool::addMember characterization (slice 6966). A valid
// member is stored before addMember calls updatePool, which queues one
// historical TROPHY_LIST packet for every fixed pool slot.
auto runTreasureAddMemberUpdate6966SelfTests() -> bool
{
    CTreasurePool pool(TreasurePoolType::Solo);
    CCharEntity   member;
    member.status        = STATUS_TYPE::NORMAL;
    member.PTreasurePool = &pool;

    pool.addMember(&member);

    bool ok = true;
    ok      = expect(pool.memberCount() == 1 && pool.isMember(&member), "member is stored") && ok;
    ok      = expect(member.getPacketCount() == TREASUREPOOL_SIZE, "member receives updated pool") && ok;
    for (const auto& packet : member.getPacketList())
    {
        ok = expect(packet->getType() == static_cast<uint16>(PacketS2C::GP_SERV_COMMAND_TROPHY_LIST), "update queues trophy list") && ok;
    }

    member.PTreasurePool = nullptr;
    return ok;
}
