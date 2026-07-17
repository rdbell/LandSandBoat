#include "test_transport_ship_spawn_2797.h"

#include "map/transport.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "transport ship spawn 2797 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runTransportShipSpawn2797SelfTests() -> bool
{
    using namespace transporthelpers;

    bool ok = true;

    // spawn always copies dock into npc location and makes the ship visible.
    {
        const auto plan = PlanShipSpawn();
        ok = expect(plan.copyDockToNpcLoc, "copyDockToNpcLoc always true") && ok;
        ok = expect(plan.setVisible, "setVisible always true") && ok;
    }

    // setVisible(true) applies the visible status/movement pair via VisibilityFor.
    {
        const auto visible = VisibilityFor(true);
        ok = expect(visible.status == static_cast<uint8>(STATUS_TYPE::NORMAL) && visible.moving == 0x8007,
                    "spawn visibility uses VisibilityFor(true)") &&
             ok;
    }

    return ok;
}
