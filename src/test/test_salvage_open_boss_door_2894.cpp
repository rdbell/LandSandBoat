#include "test_salvage_open_boss_door_2894.h"

#include "map/salvage_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "salvage open boss door 2894 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua openBossDoor pure gate for dual-wire cross-check:
//   npc:getAnimation() == xi.anim.CLOSE_DOOR
auto inlineCanOpenBossDoor(const uint8 animation) -> bool
{
    return animation == salvagehelpers::kAnimCloseDoor;
}

} // namespace

// Pure dual-wire expansion for salvagehelpers::CanOpenBossDoor
// (Lua openBossDoor CLOSE_DOOR gate; slice 2894).
auto runSalvageOpenBossDoor2894SelfTests() -> bool
{
    using salvagehelpers::CanOpenBossDoor;
    using salvagehelpers::kAnimCloseDoor;
    using salvagehelpers::kAnimOpenDoor;

    bool ok = true;

    // Anim catalog pins (match Go AnimCloseDoor / AnimOpenDoor).
    ok = expect(kAnimOpenDoor == 8, "AnimOpenDoor pin") && ok;
    ok = expect(kAnimCloseDoor == 9, "AnimCloseDoor pin") && ok;

    // Residual 0977 CanOpenBossDoor pins.
    ok = expect(CanOpenBossDoor(kAnimCloseDoor), "boss door closed should open") && ok;
    ok = expect(!CanOpenBossDoor(kAnimOpenDoor), "boss door open should not re-open") && ok;
    ok = expect(!CanOpenBossDoor(0), "anim 0 should not open") && ok;
    ok = expect(CanOpenBossDoor(9), "literal CLOSE_DOOR 9 opens") && ok;
    ok = expect(!CanOpenBossDoor(8), "literal OPEN_DOOR 8 blocks") && ok;

    // --- CanOpenBossDoor table ---
    const struct
    {
        uint8       animation;
        bool        want;
        const char* label;
    } cases[] = {
        { kAnimCloseDoor, true, "close" },
        { kAnimOpenDoor, false, "open" },
        { 0, false, "anim 0" },
        { 9, true, "literal CLOSE_DOOR 9" },
        { 8, false, "literal OPEN_DOOR 8" },
        { 99, false, "anim 99" },
        { 1, false, "anim 1" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanOpenBossDoor(c.animation);
        const bool inlineC = inlineCanOpenBossDoor(c.animation);
        const bool compose = c.animation == kAnimCloseDoor;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineC, "dual-wire CanOpenBossDoor == inline Lua") && ok;
        ok = expect(got == compose, "CanOpenBossDoor == AnimCloseDoor") && ok;
    }

    // Host compose: animation is the sole inject; only CLOSE_DOOR succeeds.
    const struct
    {
        uint8       animation;
        bool        want;
        const char* label;
    } composeCases[] = {
        { kAnimCloseDoor, true, "compose eligible close" },
        { kAnimOpenDoor, false, "compose open blocks" },
        { 0, false, "compose anim 0 blocks" },
        { 9, true, "compose literal 9" },
        { 99, false, "compose wrong anim blocks" },
    };

    for (const auto& c : composeCases)
    {
        const bool got     = CanOpenBossDoor(c.animation);
        const bool inlineC = inlineCanOpenBossDoor(c.animation);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineC, "compose dual-wire free == inline") && ok;
    }

    return ok;
}
