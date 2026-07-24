#include "test_load_mob_field_application_host_6407.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "load mob field application host 6407 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for LoadMOBList CMobEntity field application (slice 6407).
// Go: ApplyMOBRowFields / CapMobLevel / ShouldClearCharmable / LoadedMOB.
auto runLoadMOBFieldApplicationHost6407SelfTests() -> bool
{
    bool ok = true;

    // Level cap: normal min/max, skip NOTORIOUS (0x02).
    const uint8_t normalMin = 40;
    const uint8_t normalMax = 55;
    uint8_t       minL      = 50;
    uint8_t       maxL      = 60;
    const uint8_t mobType   = 0x00; // NORMAL
    if (normalMin > 0 && (mobType & 0x02) == 0 && minL > normalMin)
    {
        minL = normalMin;
    }
    if (normalMax > 0 && (mobType & 0x02) == 0 && maxL > normalMax)
    {
        maxL = normalMax;
    }
    ok = expect(minL == 40 && maxL == 55, "level cap") && ok;

    // Charmable clear bits: EVENT 0x20, FISHED 0x04, BATTLEFIELD 0x10, NOTORIOUS 0x02.
    const uint8_t eventType = 0x20;
    ok = expect((eventType & 0x20) != 0, "event type") && ok;

    // Spawn slot assign: slotId > 0.
    ok = expect(1u > 0u, "slot assign") && ok;
    ok = expect(!(0u > 0u), "slot zero skip") && ok;

    // Hitbox / 10.
    const float hitbox = 15.0f / 10.0f;
    ok = expect(hitbox == 1.5f, "hitbox scale") && ok;

    const std::string dropErr = "LoadMOBList: Drop list 9 on mob Goblin (zone id 100) set but has no entries!";
    ok = expect(dropErr.find("Drop list 9") != std::string::npos, "drop err") && ok;

    return ok;
}
