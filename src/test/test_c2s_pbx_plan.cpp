#include "test_c2s_pbx_plan.h"
#include "map/packets/c2s/0x04d_pbx.h"
#include <iostream>

namespace
{

enum class Operation : uint8_t
{
    Denied    = 0,
    Work      = 1,
    Set       = 2,
    Send      = 3,
    Cancel    = 4,
    Check     = 5,
    Recv      = 6,
    Confirm   = 7,
    Accept    = 8,
    Reject    = 9,
    Get       = 10,
    Clear     = 11,
    Query     = 12,
    DeliOpen  = 13,
    PostOpen  = 14,
    PostClose = 15
};

struct Access
{
    bool residential{}, gm{}, auctionHouse{}, mogMenu{};
};

auto plan(Access a, uint8_t command) -> Operation
{
    if (!(a.residential || a.gm || a.auctionHouse || a.mogMenu))
        return Operation::Denied;
    return command >= 1 && command <= 15 ? static_cast<Operation>(command) : Operation::Denied;
}

} // namespace

auto runC2SPBXPlanSelfTests() -> bool
{
    bool ok = true;
    for (uint8_t c = 1; c <= 15; c++)
    {
        if (plan({ .residential = true }, c) != static_cast<Operation>(c))
        {
            std::cerr << "PBX plan self-test failed\n";
            ok = false;
        }
    }
    if (plan({}, 1) != Operation::Denied)
    {
        std::cerr << "PBX denied self-test failed\n";
        ok = false;
    }
    for (const auto access : { Access{ .gm = true }, Access{ .auctionHouse = true }, Access{ .mogMenu = true } })
    {
        if (plan(access, 1) != Operation::Work)
        {
            std::cerr << "PBX authorization self-test failed\n";
            ok = false;
        }
    }
    if (plan({ .residential = true }, 0) != Operation::Denied)
    {
        std::cerr << "PBX unknown command self-test failed\n";
        ok = false;
    }
    return ok;
}
