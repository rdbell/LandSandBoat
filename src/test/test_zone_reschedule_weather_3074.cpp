#include "test_zone_reschedule_weather_3074.h"

#include "map/zone_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone ShouldRescheduleDynamicWeather 3074 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline UpdateWeather timer-wake reschedule formula for dual-wire
// cross-check (slice 3074):
//   !isStatic
auto inlineShouldRescheduleDynamicWeather(const bool isStatic) -> bool
{
    return !isStatic;
}

auto pinShouldRescheduleDynamicWeather(const bool isStatic) -> bool
{
    return !isStatic;
}

} // namespace

// Pure dual-wire expansion for zonehelpers::ShouldRescheduleDynamicWeather
// (!weather().isStatic() on UpdateWeather timer wake; slice 3074).
// Dense 2¹ boolean space.
auto runZoneRescheduleWeather3074SelfTests() -> bool
{
    using zonehelpers::ShouldForceMorningFog;
    using zonehelpers::ShouldRescheduleDynamicWeather;

    bool ok = true;

    // Residual 1363 pins still hold under dual-wire.
    ok = expect(ShouldRescheduleDynamicWeather(false), "residual !isStatic → reschedule") && ok;
    ok = expect(!ShouldRescheduleDynamicWeather(true), "residual isStatic → do not reschedule") && ok;

    const struct
    {
        bool        isStatic;
        bool        want;
        const char* label;
    } cases[] = {
        // Full dense 2¹ boolean space.
        { false, true, "dynamic weather → reschedule" },
        { true, false, "static weather → no reschedule" },

        // Residual 1363 pins.
        { false, true, "residual ShouldRescheduleDynamicWeather(false)" },
        { true, false, "residual ShouldRescheduleDynamicWeather(true)" },

        // Identity / polarity repeats for dual-wire stability.
        { false, true, "identity reschedule pole" },
        { true, false, "identity static pole" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRescheduleDynamicWeather(c.isStatic);
        const bool inlineF = inlineShouldRescheduleDynamicWeather(c.isStatic);
        const bool wantPin = pinShouldRescheduleDynamicWeather(c.isStatic);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRescheduleDynamicWeather dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldRescheduleDynamicWeather == pin formula !isStatic") && ok;
    }

    // Pin composition: reschedule iff NOT isStatic.
    ok = expect(ShouldRescheduleDynamicWeather(false), "isStatic false must reschedule") && ok;
    ok = expect(!ShouldRescheduleDynamicWeather(true), "isStatic true must not reschedule") && ok;

    // Explicit polarity: reschedule is exact NOT of inject flag.
    for (const bool isStatic : { false, true })
    {
        const bool got  = ShouldRescheduleDynamicWeather(isStatic);
        const bool want = !isStatic;
        ok              = expect(got == want, "polarity: reschedule == !isStatic") && ok;
        ok              = expect(!(got && isStatic), "polarity: never reschedule when isStatic true") && ok;
        ok              = expect(!(!got && !isStatic), "polarity: always reschedule when isStatic false") && ok;
    }

    // Host-style compose: UpdateWeather posts a main-thread task that
    // yields for WeatherNextUpdate, then injects weather().isStatic() into
    // ShouldRescheduleDynamicWeather; on true calls UpdateWeather() again.
    for (const bool isStatic : { false, true })
    {
        const bool reschedule = ShouldRescheduleDynamicWeather(isStatic);
        const bool want       = !isStatic;
        ok                    = expect(reschedule == want, "compose dual-wire identity") && ok;
        ok                    = expect(reschedule == inlineShouldRescheduleDynamicWeather(isStatic),
                      "compose free == inline") &&
             ok;
        ok = expect(!(reschedule && isStatic), "compose: never reschedule when weather is static") && ok;
        ok = expect(!(!reschedule && !isStatic), "compose: dynamic weather must reschedule") && ok;
    }

    // Production UpdateWeather path semantics (after SetWeather /
    // OnZoneWeatherChange + scheduler yield):
    // reschedule → call UpdateWeather() again (dynamic cycle);
    // else       → leave weather static; no re-arm.
    ok = expect(ShouldRescheduleDynamicWeather(false), "UpdateWeather dynamic → reschedule path") && ok;
    ok = expect(!ShouldRescheduleDynamicWeather(true), "UpdateWeather static → no-reschedule path") && ok;

    // Sibling morning-fog dual-wire (3053) is orthogonal: runs earlier in
    // UpdateWeather before SetWeather; free reschedule formula does not
    // depend on fog override poles.
    ok = expect(ShouldForceMorningFog(true, true, false), "residual morning fog force when window+below+!city") && ok;
    ok = expect(!ShouldForceMorningFog(true, true, true), "residual morning fog leave when city") && ok;
    for (const bool isStatic : { false, true })
    {
        for (const bool window : { false, true })
        {
            for (const bool below : { false, true })
            {
                for (const bool city : { false, true })
                {
                    // Orthogonal: fog force iff window&&below&&!city;
                    // reschedule iff !isStatic.
                    const bool wantFog = window && below && !city;
                    ok                 = expect(ShouldForceMorningFog(window, below, city) == wantFog,
                                "reschedule vs fog compose fog") &&
                         ok;
                    const bool wantReschedule = !isStatic;
                    ok                        = expect(ShouldRescheduleDynamicWeather(isStatic) == wantReschedule,
                                "reschedule vs fog compose reschedule") &&
                         ok;
                }
            }
        }
    }

    // Dense edge compose: free == inline == pin for full 2¹.
    for (const bool isStatic : { false, true })
    {
        const bool got  = ShouldRescheduleDynamicWeather(isStatic);
        const bool want = pinShouldRescheduleDynamicWeather(isStatic);
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldRescheduleDynamicWeather(isStatic),
                    "compose free == inline") &&
             ok;
    }

    return ok;
}
