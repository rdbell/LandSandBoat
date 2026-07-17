#include "test_zone_morning_fog_3053.h"

#include "map/zone_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone ShouldForceMorningFog 3053 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline UpdateWeather morning-fog override formula for dual-wire
// cross-check (slice 3053):
//   inFogWindow && selectedBelowHotSpell && !isCity
auto inlineShouldForceMorningFog(
    const bool inFogWindow,
    const bool selectedBelowHotSpell,
    const bool isCity) -> bool
{
    return inFogWindow && selectedBelowHotSpell && !isCity;
}

auto pinShouldForceMorningFog(
    const bool inFogWindow,
    const bool selectedBelowHotSpell,
    const bool isCity) -> bool
{
    return inFogWindow && selectedBelowHotSpell && !isCity;
}

} // namespace

// Pure dual-wire expansion for zonehelpers::ShouldForceMorningFog
// (inFogWindow && selectedBelowHotSpell && !isCity on UpdateWeather;
// slice 3053). Dense 2³ boolean space.
auto runZoneMorningFog3053SelfTests() -> bool
{
    using zonehelpers::ShouldForceMorningFog;
    using zonehelpers::ShouldRescheduleDynamicWeather;

    bool ok = true;

    // Residual 1363 pins still hold under dual-wire.
    ok = expect(ShouldForceMorningFog(true, true, false), "residual fog window + below hot + non-city → force fog") && ok;
    ok = expect(!ShouldForceMorningFog(true, true, true), "residual fog window + below hot + city → leave weather") && ok;

    const struct
    {
        bool        inFogWindow;
        bool        selectedBelowHotSpell;
        bool        isCity;
        bool        want;
        const char* label;
    } cases[] = {
        // Full dense 2³ boolean space.
        { false, false, false, false, "no window, not below, non-city → leave" },
        { false, false, true, false, "no window, not below, city → leave" },
        { false, true, false, false, "no window, below, non-city → leave" },
        { false, true, true, false, "no window, below, city → leave" },
        { true, false, false, false, "window, not below, non-city → leave" },
        { true, false, true, false, "window, not below, city → leave" },
        { true, true, false, true, "window + below + non-city → force fog" },
        { true, true, true, false, "window + below + city → leave" },

        // Residual 1363 pins.
        { true, true, false, true, "residual force fog non-city" },
        { true, true, true, false, "residual city blocks fog" },

        // Additional poles for dual-wire stability.
        { false, true, false, false, "below hot outside window → leave" },
        { true, false, false, false, "elemental/hot+ in window → leave" },
        { false, false, true, false, "city outside window → leave" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldForceMorningFog(c.inFogWindow, c.selectedBelowHotSpell, c.isCity);
        const bool inlineF = inlineShouldForceMorningFog(c.inFogWindow, c.selectedBelowHotSpell, c.isCity);
        const bool wantPin = pinShouldForceMorningFog(c.inFogWindow, c.selectedBelowHotSpell, c.isCity);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldForceMorningFog dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldForceMorningFog == pin formula window&&below&&!city") && ok;
    }

    // Pin composition: force iff inFogWindow && selectedBelowHotSpell && !isCity.
    ok = expect(ShouldForceMorningFog(true, true, false), "window + below + non-city must force fog") && ok;
    ok = expect(!ShouldForceMorningFog(true, true, true), "window + below + city must not force fog") && ok;
    ok = expect(!ShouldForceMorningFog(true, false, false), "window without below-hot must not force fog") && ok;
    ok = expect(!ShouldForceMorningFog(false, true, false), "below-hot outside window must not force fog") && ok;

    // Explicit polarity over dense 2³ inject space.
    for (const bool window : { false, true })
    {
        for (const bool below : { false, true })
        {
            for (const bool city : { false, true })
            {
                const bool got  = ShouldForceMorningFog(window, below, city);
                const bool want = window && below && !city;
                ok              = expect(got == want, "polarity: force == window&&below&&!city") && ok;
                ok              = expect(!(got && !window), "polarity: never force when !inFogWindow") && ok;
                ok              = expect(!(got && !below), "polarity: never force when !selectedBelowHotSpell") && ok;
                ok              = expect(!(got && city), "polarity: never force when isCity") && ok;
                ok              = expect(!(!got && window && below && !city),
                           "polarity: always force when window && below && !city") &&
                     ok;
            }
        }
    }

    // Host-style compose: UpdateWeather injects fog-window range,
    // selectedWeather < HotSpell, and CITY mask into ShouldForceMorningFog;
    // on true sets Fog and clamps next update to EndFogVanaDate.
    for (const bool below : { false, true })
    {
        for (const bool city : { false, true })
        {
            // Production path only forces when all three align (window true).
            const bool force = ShouldForceMorningFog(true, below, city);
            const bool want  = below && !city;
            ok               = expect(force == want, "compose window=true dual-wire identity") && ok;
            ok               = expect(force == inlineShouldForceMorningFog(true, below, city),
                        "compose window=true free == inline") &&
                 ok;
            ok = expect(!(force && (!below || city)), "compose: never force when not-below or city") && ok;
            ok = expect(!(!force && below && !city), "compose: below + non-city in window must force") && ok;
        }
    }

    // Production UpdateWeather path semantics (after SelectWeatherBand):
    // force → selectedWeather = Fog + clamp WeatherNextUpdate to fog end;
    // else  → leave selected weather, proceed to SetWeather.
    ok = expect(ShouldForceMorningFog(true, true, false), "UpdateWeather morning non-city → force fog path") && ok;
    ok = expect(!ShouldForceMorningFog(true, true, true), "UpdateWeather morning city → leave weather path") && ok;
    ok = expect(!ShouldForceMorningFog(true, false, false), "UpdateWeather elemental/hot+ → leave weather path") && ok;
    ok = expect(!ShouldForceMorningFog(false, true, false), "UpdateWeather outside fog window → leave weather path") && ok;

    // Sibling residual reschedule gate still composes under dual-wire package.
    // ShouldRescheduleDynamicWeather remains residual 1363 (not this slice).
    ok = expect(ShouldRescheduleDynamicWeather(false), "sibling residual reschedule non-static") && ok;
    ok = expect(!ShouldRescheduleDynamicWeather(true), "sibling residual reschedule static") && ok;

    // Dense edge compose: free == inline == pin for full 2³.
    for (const bool window : { false, true })
    {
        for (const bool below : { false, true })
        {
            for (const bool city : { false, true })
            {
                const bool got  = ShouldForceMorningFog(window, below, city);
                const bool want = pinShouldForceMorningFog(window, below, city);
                ok              = expect(got == want, "compose free == pin formula") && ok;
                ok              = expect(got == inlineShouldForceMorningFog(window, below, city),
                           "compose free == inline") &&
                     ok;
            }
        }
    }

    return ok;
}
