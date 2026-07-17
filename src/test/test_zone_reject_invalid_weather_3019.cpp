#include "test_zone_reject_invalid_weather_3019.h"

#include "map/zone_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone ShouldRejectInvalidWeather 3019 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline SetWeather invalid-enum reject formula for dual-wire
// cross-check (slice 3019):
//   !isValidEnum
auto inlineShouldRejectInvalidWeather(const bool isValidEnum) -> bool
{
    return !isValidEnum;
}

} // namespace

// Pure dual-wire expansion for zonehelpers::ShouldRejectInvalidWeather
// (!isValidEnum / !enum_contains on SetWeather; slice 3019).
// Dense 2¹ boolean space.
auto runZoneRejectInvalidWeather3019SelfTests() -> bool
{
    using zonehelpers::ShouldRejectInvalidWeather;

    bool ok = true;

    // Residual 1363 pins still hold under dual-wire.
    ok = expect(ShouldRejectInvalidWeather(false), "residual invalid enum → reject") && ok;
    ok = expect(!ShouldRejectInvalidWeather(true), "residual valid enum → admit") && ok;

    const struct
    {
        bool        isValidEnum;
        bool        want;
        const char* label;
    } cases[] = {
        // Full dense 2¹ boolean space.
        { false, true, "invalid enum → reject" },
        { true, false, "valid enum → admit" },

        // Residual 1363 pins.
        { false, true, "residual invalid weather" },
        { true, false, "residual valid weather" },

        // Identity / polarity repeats for dual-wire stability.
        { false, true, "identity reject pole" },
        { true, false, "identity admit pole" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectInvalidWeather(c.isValidEnum);
        const bool inlineF = inlineShouldRejectInvalidWeather(c.isValidEnum);
        const bool wantPin = !c.isValidEnum;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectInvalidWeather dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldRejectInvalidWeather == pin formula !isValidEnum") && ok;
    }

    // Pin composition: reject iff NOT isValidEnum.
    ok = expect(ShouldRejectInvalidWeather(false), "isValidEnum false must reject") && ok;
    ok = expect(!ShouldRejectInvalidWeather(true), "isValidEnum true must not reject") && ok;

    // Explicit polarity: reject is exact NOT of inject flag.
    for (const bool valid : { false, true })
    {
        const bool got  = ShouldRejectInvalidWeather(valid);
        const bool want = !valid;
        ok              = expect(got == want, "polarity: reject == !isValidEnum") && ok;
        ok              = expect(!(got && valid), "polarity: never reject when isValidEnum true") && ok;
        ok              = expect(!(!got && !valid), "polarity: always reject when isValidEnum false") && ok;
    }

    // Host-style inject poles: magic_enum::enum_contains<Weather>(weather)
    // in SetWeather.
    for (const bool valid : { false, true })
    {
        const bool reject = ShouldRejectInvalidWeather(valid);
        const bool want   = !valid;
        ok                = expect(reject == want, "host inject dual-wire identity") && ok;
        ok                = expect(reject == inlineShouldRejectInvalidWeather(valid),
                    "host inject free == inline") &&
             ok;
        // reject only when enum_contains is false.
        ok = expect(!(reject && valid), "never reject when enum_contains true") && ok;
        ok = expect(!(!reject && !valid), "invalid enum must reject") && ok;
    }

    // Production SetWeather path semantics:
    // isValidEnum=false → FormatInvalidWeatherWarning + return;
    // isValidEnum=true  → continue to ShouldSkipSameWeather / WeatherChange.
    ok = expect(ShouldRejectInvalidWeather(false), "SetWeather invalid enum → reject path") && ok;
    ok = expect(!ShouldRejectInvalidWeather(true), "SetWeather valid enum → admit path") && ok;

    // Dense compose: full 2¹ free == inline == pin.
    for (const bool valid : { false, true })
    {
        const bool got  = ShouldRejectInvalidWeather(valid);
        const bool want = !valid;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldRejectInvalidWeather(valid),
                    "compose free == inline") &&
             ok;
    }

    return ok;
}
