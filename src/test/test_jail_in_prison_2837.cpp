#include "test_jail_in_prison_2837.h"

#include "map/utils/jailutils_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "jail in prison 2837 self-test failed: " << label << '\n';
    }
    return condition;
}

// Production ZONE_MORDION_GAOL pin (zone.h) without pulling zone headers.
constexpr uint16 ZoneMordionGaol = 131;

} // namespace

auto runJailInPrison2837SelfTests() -> bool
{
    using jailutilshelpers::InPrison;
    using jailutilshelpers::InPrisonPure;

    bool ok = true;

    // InPrisonPure: gmLevelIsZero && zoneIsMordion
    ok = expect(InPrisonPure(true, true), "pure both true") && ok;
    ok = expect(!InPrisonPure(true, false), "pure gm zero wrong zone") && ok;
    ok = expect(!InPrisonPure(false, true), "pure GM in mordion") && ok;
    ok = expect(!InPrisonPure(false, false), "pure both false") && ok;

    // InPrison(gmLevel, zoneID, mordionGaolZoneID)
    ok = expect(InPrison(0, ZoneMordionGaol, ZoneMordionGaol), "mordion gm0") && ok;
    ok = expect(!InPrison(1, ZoneMordionGaol, ZoneMordionGaol), "mordion gm1") && ok;
    ok = expect(!InPrison(255, ZoneMordionGaol, ZoneMordionGaol), "mordion gm255") && ok;
    ok = expect(!InPrison(0, 100, ZoneMordionGaol), "other zone gm0") && ok;
    ok = expect(!InPrison(0, 0, ZoneMordionGaol), "zone zero gm0") && ok;
    ok = expect(!InPrison(0, 130, ZoneMordionGaol), "zone 130 gm0") && ok;
    ok = expect(!InPrison(0, 132, ZoneMordionGaol), "zone 132 gm0") && ok;
    ok = expect(!InPrison(1, 100, ZoneMordionGaol), "other zone gm1") && ok;

    // Composition: 3-arg InPrison dual-wires InPrisonPure
    const struct
    {
        uint8       gmLevel;
        uint16      zoneID;
        bool        want;
        const char* label;
    } cases[] = {
        { 0, ZoneMordionGaol, true, "compose mordion gm0" },
        { 1, ZoneMordionGaol, false, "compose mordion gm1" },
        { 0, 100, false, "compose other zone" },
        { 5, 100, false, "compose both false" },
    };
    for (const auto& c : cases)
    {
        const bool got     = InPrison(c.gmLevel, c.zoneID, ZoneMordionGaol);
        const bool pureGot = InPrisonPure(c.gmLevel == 0, c.zoneID == ZoneMordionGaol);
        ok                 = expect(got == c.want, c.label) && ok;
        ok                 = expect(got == pureGot, "compose InPrison == InPrisonPure") && ok;
    }

    // Injected mordion id: comparison uses the host-supplied constant
    ok = expect(InPrison(0, 99, 99), "injected mordion match") && ok;
    ok = expect(!InPrison(0, 99, ZoneMordionGaol), "injected mordion mismatch") && ok;

    return ok;
}
