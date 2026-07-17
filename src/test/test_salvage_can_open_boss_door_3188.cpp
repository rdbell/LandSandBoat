#include "test_salvage_can_open_boss_door_3188.h"

#include "map/salvage_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "salvage can open boss door 3188 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua openBossDoor pure gate for dual-wire cross-check:
//   npc:getAnimation() == xi.anim.CLOSE_DOOR
auto inlineCanOpenBossDoor(const uint8 animation) -> bool
{
    return animation == salvagehelpers::kAnimCloseDoor;
}

// Compact dual-wire pin matching Go pinCanOpenBossDoor3188:
//   animation == kAnimCloseDoor
auto pinCanOpenBossDoor(const uint8 animation) -> bool
{
    return animation == salvagehelpers::kAnimCloseDoor;
}

} // namespace

// Pure dual-wire expansion for salvagehelpers::CanOpenBossDoor
// (Lua openBossDoor CLOSE_DOOR gate; OmegaXI internal/salvage;
// slice 3188).
//
// Coverage:
//   - CLOSE_DOOR (9) → open true (positive pin form)
//   - OPEN_DOOR / other animations → open false
//   - free == inline == pin == animation==kAnimCloseDoor
//   - dense poles: open / close / other (0,1,8,9,10,99,255 + named pins)
//   - residual 0977 / 2894 pins still hold
auto runSalvageCanOpenBossDoor3188SelfTests() -> bool
{
    using salvagehelpers::CanOpenBossDoor;
    using salvagehelpers::kAnimCloseDoor;
    using salvagehelpers::kAnimOpenDoor;

    bool ok = true;

    // Anim catalog pins (match Go AnimCloseDoor / AnimOpenDoor).
    ok = expect(kAnimOpenDoor == 8, "AnimOpenDoor pin") && ok;
    ok = expect(kAnimCloseDoor == 9, "AnimCloseDoor pin") && ok;

    // Residual 0977 / 2894 CanOpenBossDoor pins still hold under dual-wire.
    ok = expect(CanOpenBossDoor(kAnimCloseDoor), "residual: boss door closed should open") && ok;
    ok = expect(!CanOpenBossDoor(kAnimOpenDoor), "residual: boss door open should not re-open") && ok;
    ok = expect(!CanOpenBossDoor(0), "residual: anim 0 should not open") && ok;
    ok = expect(CanOpenBossDoor(9), "residual: literal CLOSE_DOOR 9 opens") && ok;
    ok = expect(!CanOpenBossDoor(8), "residual: literal OPEN_DOOR 8 blocks") && ok;
    ok = expect(CanOpenBossDoor(kAnimCloseDoor), "residual 2894: AnimCloseDoor pin should open") && ok;
    ok = expect(!CanOpenBossDoor(kAnimOpenDoor), "residual 2894: AnimOpenDoor pin should not open") && ok;

    // --- Eligible open path (positive pin form) ---
    ok = expect(CanOpenBossDoor(kAnimCloseDoor), "eligible AnimCloseDoor should open") && ok;
    ok = expect(CanOpenBossDoor(9), "eligible literal CLOSE_DOOR 9 should open") && ok;
    ok = expect(pinCanOpenBossDoor(kAnimCloseDoor), "eligible pin AnimCloseDoor should open") && ok;
    ok = expect(pinCanOpenBossDoor(9), "eligible pin literal 9 should open") && ok;

    // --- Blocked paths (open / other) ---
    ok = expect(!CanOpenBossDoor(kAnimOpenDoor), "open should block boss door") && ok;
    ok = expect(!CanOpenBossDoor(8), "literal OPEN_DOOR 8 should block") && ok;
    ok = expect(!CanOpenBossDoor(0), "anim 0 should block") && ok;
    ok = expect(!CanOpenBossDoor(1), "anim 1 should block") && ok;
    ok = expect(!CanOpenBossDoor(10), "anim 10 should block") && ok;
    ok = expect(!CanOpenBossDoor(99), "anim 99 should block") && ok;
    ok = expect(!CanOpenBossDoor(255), "anim 255 should block") && ok;

    // --- Composition table: free == inline == pin == compose ---
    // Dense poles: open / close / other.
    const struct
    {
        uint8       animation;
        bool        want;
        const char* label;
    } cases[] = {
        { kAnimCloseDoor, true, "table close AnimCloseDoor pin" },
        { 9, true, "table close literal CLOSE_DOOR 9" },
        { kAnimOpenDoor, false, "table open AnimOpenDoor pin" },
        { 8, false, "table open literal OPEN_DOOR 8" },
        { 0, false, "table other anim 0" },
        { 1, false, "table other anim 1" },
        { 10, false, "table other anim 10" },
        { 99, false, "table other anim 99" },
        { 255, false, "table other anim max uint8" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanOpenBossDoor(c.animation);
        const bool inlineC = inlineCanOpenBossDoor(c.animation);
        const bool pinGot  = pinCanOpenBossDoor(c.animation);
        const bool compose = c.animation == kAnimCloseDoor;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineC, "dual-wire free==inline") && ok;
        ok = expect(got == pinGot, "dual-wire free==pin") && ok;
        ok = expect(got == compose, "formula free==animation==kAnimCloseDoor") && ok;
    }

    // --- Production openBossDoor path semantics ---
    // Eligible → may continue openDoor(15) / queue arch openDoor(10).
    // Blocked → no open writeback.
    ok = expect(CanOpenBossDoor(kAnimCloseDoor), "openBossDoor eligible → continue path") && ok;
    ok = expect(!CanOpenBossDoor(kAnimOpenDoor), "openBossDoor open → blocked") && ok;
    ok = expect(!CanOpenBossDoor(0), "openBossDoor other anim → blocked") && ok;

    // Dense compose identity over required poles.
    const uint8 composeAnims[] = { 0, 1, kAnimOpenDoor, kAnimCloseDoor, 8, 9, 10, 99, 255 };
    for (const uint8 anim : composeAnims)
    {
        const bool got  = CanOpenBossDoor(anim);
        const bool want = anim == kAnimCloseDoor;
        ok = expect(got == want, "compose animation==kAnimCloseDoor") && ok;
        ok = expect(got == inlineCanOpenBossDoor(anim), "compose inline") && ok;
        ok = expect(got == pinCanOpenBossDoor(anim), "compose pin") && ok;
    }

    // Compose identity: free function is the gate; host injects animation only.
    ok = expect(CanOpenBossDoor(kAnimCloseDoor) == pinCanOpenBossDoor(kAnimCloseDoor),
                "formula free == pin AnimCloseDoor") &&
         ok;
    ok = expect(!CanOpenBossDoor(kAnimOpenDoor), "formula open path must block") && ok;
    ok = expect(CanOpenBossDoor(kAnimOpenDoor) == pinCanOpenBossDoor(kAnimOpenDoor),
                "formula open free == pin") &&
         ok;

    return ok;
}
