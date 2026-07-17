#include "test_salvage_can_open_door_3358.h"

#include "map/salvage_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "salvage can open door 3358 self-test failed: " << label << '\n';
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

// Compact dual-wire pin matching Go pinCanOpenDoor3358:
//   animation == kAnimCloseDoor && unSealed == kDoorUnsealedValue
auto pinCanOpenDoor(const uint8 animation, const int32 unSealed) -> bool
{
    return animation == salvagehelpers::kAnimCloseDoor &&
           unSealed == salvagehelpers::kDoorUnsealedValue;
}

// Prior dedicated 3133 pin (same formula; residual suite identity).
auto pinCanOpenDoor3133(const uint8 animation, const int32 unSealed) -> bool
{
    return animation == salvagehelpers::kAnimCloseDoor &&
           unSealed == salvagehelpers::kDoorUnsealedValue;
}

} // namespace

// Pure dual-wire expansion for salvagehelpers::CanOpenDoor
// (Lua onDoorOpen CLOSE_DOOR + unSealed gate; OmegaXI internal/salvage;
// slice 3358 dedicated dual-wire expand residual 2892).
//
// Coverage:
//   - CLOSE_DOOR + unSealed==1 → open true
//   - open / sealed / wrong unSealed → open false
//   - free == inline == pin == AnimCloseDoor&&DoorUnsealedValue
//   - residual 0977 / 1083 / 2892 / 3133 pins still hold
auto runSalvageCanOpenDoor3358SelfTests() -> bool
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

    // Residual 0977 / 1083 / 2892 / 3133 CanOpenDoor pins still hold under dual-wire.
    ok = expect(CanOpenDoor(kAnimCloseDoor, 1), "residual: closed + unSealed=1 should open") && ok;
    ok = expect(!CanOpenDoor(kAnimOpenDoor, 1), "residual: already open should not open") && ok;
    ok = expect(!CanOpenDoor(kAnimCloseDoor, 0), "residual: sealed door should not open") && ok;
    ok = expect(!CanOpenDoor(kAnimCloseDoor, 2), "residual: unSealed must be exactly 1") && ok;
    ok = expect(CanOpenDoor(kAnimCloseDoor, kDoorUnsealedValue), "residual: DoorUnsealedValue pin opens") && ok;
    ok = expect(!CanOpenDoor(kAnimCloseDoor, kDoorSealedValue), "residual: DoorSealedValue pin blocks") && ok;
    ok = expect(CanOpenDoor(kAnimCloseDoor, kDoorUnsealedValue) ==
                    pinCanOpenDoor3133(kAnimCloseDoor, kDoorUnsealedValue),
                "residual 3133: free == pin for close+unsealed") &&
         ok;
    ok = expect(CanOpenDoor(kAnimCloseDoor, 0) == pinCanOpenDoor3133(kAnimCloseDoor, 0),
                "residual 3133: free == pin for sealed") &&
         ok;

    // --- Eligible open path ---
    ok = expect(CanOpenDoor(kAnimCloseDoor, kDoorUnsealedValue), "eligible close+unsealed") && ok;
    ok = expect(CanOpenDoor(kAnimCloseDoor, 1), "eligible close + literal 1") && ok;
    ok = expect(CanOpenDoor(9, 1), "eligible literal CLOSE_DOOR 9 + 1") && ok;

    // --- Blocked paths ---
    ok = expect(!CanOpenDoor(kAnimOpenDoor, kDoorUnsealedValue), "open + unsealed blocks") && ok;
    ok = expect(!CanOpenDoor(kAnimCloseDoor, kDoorSealedValue), "close + sealed blocks") && ok;
    ok = expect(!CanOpenDoor(kAnimCloseDoor, 2), "close + unSealed 2 blocks") && ok;
    ok = expect(!CanOpenDoor(kAnimCloseDoor, -1), "close + negative unSealed blocks") && ok;
    ok = expect(!CanOpenDoor(0, 1), "anim 0 + unSealed blocks") && ok;
    ok = expect(!CanOpenDoor(10, kDoorUnsealedValue), "anim 10 + unSealed blocks") && ok;
    ok = expect(!CanOpenDoor(99, kDoorUnsealedValue), "anim 99 + unSealed blocks") && ok;

    // --- Composition table: free == inline == pin == compose ---
    const struct
    {
        uint8       animation;
        int32       unSealed;
        bool        want;
        const char* label;
    } cases[] = {
        { kAnimCloseDoor, 1, true, "table close + unSealed 1" },
        { kAnimCloseDoor, kDoorUnsealedValue, true, "table close + DoorUnsealedValue" },
        { 9, 1, true, "table literal CLOSE_DOOR 9 + 1" },
        { kAnimOpenDoor, 1, false, "table open + unSealed 1" },
        { kAnimOpenDoor, kDoorUnsealedValue, false, "table open + DoorUnsealedValue" },
        { kAnimCloseDoor, 0, false, "table close + sealed 0" },
        { kAnimCloseDoor, kDoorSealedValue, false, "table close + DoorSealedValue" },
        { kAnimCloseDoor, 2, false, "table close + unSealed 2" },
        { kAnimCloseDoor, -1, false, "table close + negative unSealed" },
        { kAnimOpenDoor, 0, false, "table open + sealed" },
        { kAnimOpenDoor, 2, false, "table open + unSealed 2" },
        { 0, 1, false, "table anim 0 + unSealed 1" },
        { 8, 1, false, "table literal OPEN_DOOR 8 + 1" },
        { 10, kDoorUnsealedValue, false, "table anim 10 + unSealed" },
        { 99, kDoorUnsealedValue, false, "table anim 99 + unSealed" },
        { kAnimCloseDoor, 100, false, "table close + large unSealed" },
    };

    for (const auto& c : cases)
    {
        const bool got      = CanOpenDoor(c.animation, c.unSealed);
        const bool inlineC  = inlineCanOpenDoor(c.animation, c.unSealed);
        const bool pinGot   = pinCanOpenDoor(c.animation, c.unSealed);
        const bool pinPrior = pinCanOpenDoor3133(c.animation, c.unSealed);
        const bool compose  = c.animation == kAnimCloseDoor && c.unSealed == kDoorUnsealedValue;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineC, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == pinPrior, "dual-wire free==pinPrior3133") && ok;
        ok = expect(got == compose, "formula free==AnimCloseDoor&&DoorUnsealedValue") && ok;
    }

    // --- Production onDoorOpen path semantics ---
    // Eligible → may continue open writeback / stage / progress / untargetable.
    // Blocked → no open writeback.
    ok = expect(CanOpenDoor(kAnimCloseDoor, kDoorUnsealedValue), "onDoorOpen eligible → continue path") && ok;
    ok = expect(!CanOpenDoor(kAnimOpenDoor, kDoorUnsealedValue), "onDoorOpen open → blocked") && ok;
    ok = expect(!CanOpenDoor(kAnimCloseDoor, kDoorSealedValue), "onDoorOpen sealed → blocked") && ok;
    ok = expect(!CanOpenDoor(kAnimCloseDoor, 2), "onDoorOpen wrong unSealed → blocked") && ok;

    // Dense compose identity over representative animation / unSealed pairs.
    const uint8 composeAnims[] = { 0, 1, kAnimOpenDoor, kAnimCloseDoor, 8, 9, 10, 99, 255 };
    const int32 composeSeals[] = { -2, -1, 0, 1, 2, 3, kDoorSealedValue, kDoorUnsealedValue, 100 };
    for (const uint8 anim : composeAnims)
    {
        for (const int32 seal : composeSeals)
        {
            const bool got = CanOpenDoor(anim, seal);
            const bool want =
                anim == kAnimCloseDoor && seal == kDoorUnsealedValue;
            ok = expect(got == want, "compose AnimCloseDoor&&DoorUnsealedValue") && ok;
            ok = expect(got == inlineCanOpenDoor(anim, seal), "compose inline") && ok;
            ok = expect(got == pinCanOpenDoor(anim, seal), "compose pin") && ok;
            ok = expect(got == pinCanOpenDoor3133(anim, seal), "compose pinPrior3133") && ok;
        }
    }

    return ok;
}
