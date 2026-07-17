#include "test_login_zone_cap_2841.h"

#include "login/login_helpers_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "login zone cap 2841 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectU16(const uint16 got, const uint16 want, const char* const label) -> bool
{
    if (got != want)
    {
        std::cerr << "login zone cap 2841 self-test failed: " << label
                  << " got=" << got << " want=" << want << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runLoginZoneCap2841SelfTests() -> bool
{
    using loginHelpers::IsZoneAtPlayerCap;
    using loginHelpers::IsZoneTypeInstanced;
    using loginHelpers::ZonePlayerCapThreshold;
    using loginHelpers::ZoneTypeInstanced;

    bool ok = true;

    // --- IsZoneTypeInstanced bit ---
    ok = expect(!IsZoneTypeInstanced(0), "zonetype zero") && ok;
    ok = expect(!IsZoneTypeInstanced(0x0001), "city only") && ok;
    ok = expect(IsZoneTypeInstanced(ZoneTypeInstanced), "instanced only") && ok;
    ok = expect(IsZoneTypeInstanced(0x0001 | ZoneTypeInstanced), "city and instanced") && ok;
    ok = expect(!IsZoneTypeInstanced(0x0080), "dynamis without instanced") && ok;
    ok = expect(ZoneTypeInstanced == 0x0100, "ZoneTypeInstanced pin") && ok;

    // --- ZonePlayerCapThreshold ---
    ok = expectU16(ZonePlayerCapThreshold(700, 5, true), 700, "gm uses full cap") && ok;
    ok = expectU16(ZonePlayerCapThreshold(700, 5, false), 695, "player subtracts reserved") && ok;
    ok = expectU16(ZonePlayerCapThreshold(5, 5, false), 0, "player reserved equals cap") && ok;
    ok = expectU16(ZonePlayerCapThreshold(3, 5, false), 0, "player reserved exceeds cap") && ok;
    ok = expectU16(ZonePlayerCapThreshold(10, 0, true), 10, "gm zero reserved") && ok;
    ok = expectU16(ZonePlayerCapThreshold(10, 0, false), 10, "player zero reserved") && ok;

    // --- IsZoneAtPlayerCap golden values (match Go TestIsZoneAtPlayerCapGoldenValues) ---
    ok = expect(!IsZoneAtPlayerCap(0, 5, false, 999, false), "cap disabled") && ok;
    ok = expect(!IsZoneAtPlayerCap(0, 5, true, 0, true), "cap disabled ignores instanced") && ok;
    ok = expect(!IsZoneAtPlayerCap(700, 5, false, 1000, true), "instanced never at cap") && ok;
    ok = expect(!IsZoneAtPlayerCap(700, 5, false, 694, false), "player under threshold") && ok;
    ok = expect(IsZoneAtPlayerCap(700, 5, false, 695, false), "player at threshold") && ok;
    ok = expect(IsZoneAtPlayerCap(700, 5, false, 696, false), "player above threshold") && ok;
    ok = expect(!IsZoneAtPlayerCap(700, 5, true, 699, false), "gm under full cap") && ok;
    ok = expect(IsZoneAtPlayerCap(700, 5, true, 700, false), "gm at full cap") && ok;
    ok = expect(IsZoneAtPlayerCap(700, 5, true, 701, false), "gm above full cap") && ok;
    ok = expect(IsZoneAtPlayerCap(3, 5, false, 0, false), "player reserved clamp empty zone") && ok;
    ok = expect(IsZoneAtPlayerCap(5, 5, false, 0, false), "player reserved equals cap empty") && ok;
    ok = expect(!IsZoneAtPlayerCap(700, 5, false, 0, false), "default map settings player not full") && ok;

    // --- Host path composition: instanced := IsZoneTypeInstanced(zoneType) ---
    ok = expect(
        !IsZoneAtPlayerCap(10, 0, false, 10, IsZoneTypeInstanced(ZoneTypeInstanced)),
        "instanced zone type bit forces false") &&
        ok;
    ok = expect(
        IsZoneAtPlayerCap(10, 0, false, 10, IsZoneTypeInstanced(0x0001)),
        "non-instanced city type at cap true") &&
        ok;

    // --- Composition: pure IsZoneAtPlayerCap dual-wires threshold + instanced ---
    const struct
    {
        uint16      cap;
        uint16      reserved;
        bool        isGM;
        uint32      pop;
        uint16      zoneType;
        bool        want;
        const char* label;
    } cases[] = {
        { 700, 5, false, 695, 0x0001, true, "compose player at threshold city" },
        { 700, 5, false, 695, ZoneTypeInstanced, false, "compose player at threshold instanced" },
        { 700, 5, true, 700, 0x0001, true, "compose gm at full cap" },
        { 0, 5, false, 999, 0x0001, false, "compose cap disabled" },
        { 10, 0, false, 9, 0, false, "compose under cap" },
        { 10, 0, false, 10, 0, true, "compose at cap" },
    };
    for (const auto& c : cases)
    {
        const bool instanced = IsZoneTypeInstanced(c.zoneType);
        const bool got       = IsZoneAtPlayerCap(c.cap, c.reserved, c.isGM, c.pop, instanced);
        ok                   = expect(got == c.want, c.label) && ok;

        // Cross-check expanded form
        bool expanded = false;
        if (c.cap != 0 && !instanced)
        {
            expanded = c.pop >= static_cast<uint32>(ZonePlayerCapThreshold(c.cap, c.reserved, c.isGM));
        }
        ok = expect(got == expanded, "compose expanded form") && ok;
    }

    return ok;
}
