#include "test_salvage_open_door_2892.h"

#include "map/salvage_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "salvage open door 2892 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua onDoorOpen pure gate for dual-wire cross-check:
//   npc:getAnimation() == xi.animation.CLOSE_DOOR and
//   npc:getLocalVar('unSealed') == 1
auto inlineCanOpenDoor(const uint8 animation, const int32 unSealed) -> bool
{
    return animation == salvagehelpers::kAnimCloseDoor && unSealed == 1;
}

} // namespace

// Pure dual-wire expansion for salvagehelpers::CanOpenDoor
// (Lua onDoorOpen CLOSE_DOOR + unSealed gate; slice 2892).
auto runSalvageOpenDoor2892SelfTests() -> bool
{
    using salvagehelpers::CanOpenDoor;
    using salvagehelpers::kAnimCloseDoor;
    using salvagehelpers::kAnimOpenDoor;
    using salvagehelpers::kDoorSealedValue;
    using salvagehelpers::kDoorUnsealedValue;

    bool ok = true;

    // Anim / seal catalog pins (match Go AnimCloseDoor / DoorUnsealedValue).
    ok = expect(kAnimOpenDoor == 8, "AnimOpenDoor pin") && ok;
    ok = expect(kAnimCloseDoor == 9, "AnimCloseDoor pin") && ok;
    ok = expect(kDoorSealedValue == 0, "DoorSealedValue pin") && ok;
    ok = expect(kDoorUnsealedValue == 1, "DoorUnsealedValue pin") && ok;

    // Residual 0977 / 1083 CanOpenDoor pins.
    ok = expect(CanOpenDoor(kAnimCloseDoor, 1), "closed + unSealed=1 should open") && ok;
    ok = expect(!CanOpenDoor(kAnimOpenDoor, 1), "already open should not open") && ok;
    ok = expect(!CanOpenDoor(kAnimCloseDoor, 0), "sealed door should not open") && ok;
    ok = expect(!CanOpenDoor(kAnimCloseDoor, 2), "unSealed must be exactly 1") && ok;
    ok = expect(CanOpenDoor(kAnimCloseDoor, kDoorUnsealedValue), "DoorUnsealedValue pin opens") && ok;
    ok = expect(!CanOpenDoor(kAnimCloseDoor, kDoorSealedValue), "DoorSealedValue pin blocks") && ok;

    // --- CanOpenDoor table ---
    const struct
    {
        uint8       animation;
        int32       unSealed;
        bool        want;
        const char* label;
    } cases[] = {
        { kAnimCloseDoor, 1, true, "close + unSealed 1" },
        { kAnimCloseDoor, kDoorUnsealedValue, true, "close + DoorUnsealedValue" },
        { kAnimOpenDoor, 1, false, "open + unSealed 1" },
        { kAnimCloseDoor, 0, false, "close + sealed 0" },
        { kAnimCloseDoor, kDoorSealedValue, false, "close + DoorSealedValue" },
        { kAnimCloseDoor, 2, false, "close + unSealed 2" },
        { kAnimOpenDoor, 0, false, "open + sealed" },
        { kAnimOpenDoor, 2, false, "open + unSealed 2" },
        { 0, 1, false, "anim 0 + unSealed 1" },
        { 9, 1, true, "literal CLOSE_DOOR 9 + 1" },
        { 8, 1, false, "literal OPEN_DOOR 8 + 1" },
        { kAnimCloseDoor, -1, false, "close + negative unSealed" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanOpenDoor(c.animation, c.unSealed);
        const bool inlineC = inlineCanOpenDoor(c.animation, c.unSealed);
        const bool compose = c.animation == kAnimCloseDoor && c.unSealed == kDoorUnsealedValue;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineC, "dual-wire CanOpenDoor == inline Lua") && ok;
        ok = expect(got == compose, "CanOpenDoor == AnimCloseDoor && DoorUnsealedValue") && ok;
    }

    // Host compose: animation and unSealed are independent injects; only the
    // pure conjunction of CLOSE_DOOR + unSealed==1 succeeds.
    const struct
    {
        uint8       animation;
        int32       unSealed;
        bool        want;
        const char* label;
    } composeCases[] = {
        { kAnimCloseDoor, kDoorUnsealedValue, true, "compose eligible close+unsealed" },
        { kAnimOpenDoor, kDoorUnsealedValue, false, "compose open blocks" },
        { kAnimCloseDoor, kDoorSealedValue, false, "compose sealed blocks" },
        { kAnimOpenDoor, kDoorSealedValue, false, "compose open+sealed blocks" },
        { kAnimCloseDoor, 2, false, "compose wrong unSealed blocks" },
    };

    for (const auto& c : composeCases)
    {
        const bool got     = CanOpenDoor(c.animation, c.unSealed);
        const bool inlineC = inlineCanOpenDoor(c.animation, c.unSealed);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineC, "compose dual-wire free == inline") && ok;
    }

    return ok;
}
