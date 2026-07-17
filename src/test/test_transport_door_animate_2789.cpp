#include "test_transport_door_animate_2789.h"

#include "map/entities/base_entity.h"
#include "map/transport.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "transport door animate 2789 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runTransportDoorAnimate2789SelfTests() -> bool
{
    using namespace transporthelpers;

    bool ok = true;

    // animateSetup: non-zero animation IDs set animation; timestamp always stamps.
    {
        const auto plan = PlanShipAnimateSetup(1);
        ok = expect(plan.setAnimation && plan.stampTransportTimestamp, "animate non-zero") && ok;
    }
    {
        const auto plan = PlanShipAnimateSetup(0);
        ok = expect(!plan.setAnimation && plan.stampTransportTimestamp, "animate zero skips animation") && ok;
    }
    {
        const auto plan = PlanShipAnimateSetup(255);
        ok = expect(plan.setAnimation && plan.stampTransportTimestamp, "animate high id") && ok;
    }

    // openDoor/closeDoor with a door present.
    {
        const auto plan = PlanTransportDoor(true, true, true);
        ok = expect(plan.applyAnimation && plan.animation == static_cast<uint8>(ANIMATION_OPEN_DOOR) && plan.sendEntityUpdate,
                    "open door + packet") &&
             ok;
    }
    {
        const auto plan = PlanTransportDoor(true, false, true);
        ok = expect(plan.applyAnimation && plan.animation == static_cast<uint8>(ANIMATION_OPEN_DOOR) && !plan.sendEntityUpdate,
                    "open door no packet") &&
             ok;
    }
    {
        const auto plan = PlanTransportDoor(true, true, false);
        ok = expect(plan.applyAnimation && plan.animation == static_cast<uint8>(ANIMATION_CLOSE_DOOR) && plan.sendEntityUpdate,
                    "close door + packet") &&
             ok;
    }
    {
        const auto plan = PlanTransportDoor(true, false, false);
        ok = expect(plan.applyAnimation && plan.animation == static_cast<uint8>(ANIMATION_CLOSE_DOOR) && !plan.sendEntityUpdate,
                    "close door no packet") &&
             ok;
    }

    // Missing door: every field is a no-op regardless of sendPacket/open.
    {
        const auto planOpen  = PlanTransportDoor(false, true, true);
        const auto planClose = PlanTransportDoor(false, false, false);
        ok = expect(!planOpen.applyAnimation && planOpen.animation == 0 && !planOpen.sendEntityUpdate, "no door open no-op") && ok;
        ok = expect(!planClose.applyAnimation && planClose.animation == 0 && !planClose.sendEntityUpdate, "no door close no-op") && ok;
    }

    // Animation ordinals match base_entity.h.
    ok = expect(static_cast<uint8>(ANIMATION_OPEN_DOOR) == 8, "OPEN_DOOR == 8") && ok;
    ok = expect(static_cast<uint8>(ANIMATION_CLOSE_DOOR) == 9, "CLOSE_DOOR == 9") && ok;

    return ok;
}
