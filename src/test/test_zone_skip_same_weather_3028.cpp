#include "test_zone_skip_same_weather_3028.h"

#include "map/zone_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone ShouldSkipSameWeather 3028 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline SetWeather already-current skip formula for dual-wire
// cross-check (slice 3028):
//   alreadyCurrent
auto inlineShouldSkipSameWeather(const bool alreadyCurrent) -> bool
{
    return alreadyCurrent;
}

} // namespace

// Pure dual-wire expansion for zonehelpers::ShouldSkipSameWeather
// (alreadyCurrent identity on SetWeather; slice 3028).
// Dense 2¹ boolean space.
auto runZoneSkipSameWeather3028SelfTests() -> bool
{
    using zonehelpers::ShouldSkipSameWeather;

    bool ok = true;

    // Residual 1363 pins still hold under dual-wire.
    ok = expect(ShouldSkipSameWeather(true), "residual already current → skip") && ok;
    ok = expect(!ShouldSkipSameWeather(false), "residual not current → apply") && ok;

    const struct
    {
        bool        alreadyCurrent;
        bool        want;
        const char* label;
    } cases[] = {
        // Full dense 2¹ boolean space.
        { true, true, "already current → skip" },
        { false, false, "not current → apply" },

        // Residual 1363 pins.
        { true, true, "residual same weather" },
        { false, false, "residual different weather" },

        // Identity / polarity repeats for dual-wire stability.
        { true, true, "identity skip pole" },
        { false, false, "identity apply pole" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldSkipSameWeather(c.alreadyCurrent);
        const bool inlineF = inlineShouldSkipSameWeather(c.alreadyCurrent);
        const bool wantPin = c.alreadyCurrent;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldSkipSameWeather dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldSkipSameWeather == pin formula alreadyCurrent") && ok;
    }

    // Pin composition: skip iff alreadyCurrent (identity).
    ok = expect(ShouldSkipSameWeather(true), "alreadyCurrent true must skip") && ok;
    ok = expect(!ShouldSkipSameWeather(false), "alreadyCurrent false must not skip") && ok;

    // Explicit polarity: skip is exact identity of inject flag.
    for (const bool current : { false, true })
    {
        const bool got  = ShouldSkipSameWeather(current);
        const bool want = current;
        ok              = expect(got == want, "polarity: skip == alreadyCurrent") && ok;
        ok              = expect(!(got && !current), "polarity: never skip when alreadyCurrent false") && ok;
        ok              = expect(!(!got && current), "polarity: always skip when alreadyCurrent true") && ok;
    }

    // Host-style inject poles: weather_.current() == weather in SetWeather.
    for (const bool current : { false, true })
    {
        const bool skip = ShouldSkipSameWeather(current);
        const bool want = current;
        ok              = expect(skip == want, "host inject dual-wire identity") && ok;
        ok              = expect(skip == inlineShouldSkipSameWeather(current),
                    "host inject free == inline") &&
             ok;
        // skip only when already current.
        ok = expect(!(skip && !current), "never skip when current != weather") && ok;
        ok = expect(!(!skip && current), "already current must skip") && ok;
    }

    // Production SetWeather path semantics (after invalid-weather gate):
    // alreadyCurrent=true  → return early (no WeatherChange / set / packet);
    // alreadyCurrent=false → WeatherChange + weather_.set + WEATHER packet.
    ok = expect(ShouldSkipSameWeather(true), "SetWeather already current → skip path") && ok;
    ok = expect(!ShouldSkipSameWeather(false), "SetWeather not current → apply path") && ok;

    // Dense compose: full 2¹ free == inline == pin.
    for (const bool current : { false, true })
    {
        const bool got  = ShouldSkipSameWeather(current);
        const bool want = current;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldSkipSameWeather(current),
                    "compose free == inline") &&
             ok;
    }

    return ok;
}
