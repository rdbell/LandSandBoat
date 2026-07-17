#include "test_recast_stamp_zero_2931.h"

#include "map/recast_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "recast ShouldStampOnZeroRecast 2931 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Load charged-path stamp gate for dual-wire cross-check (slice 2931):
//   recastTimeIsZero
auto inlineShouldStampOnZeroRecast(const bool recastTimeIsZero) -> bool
{
    return recastTimeIsZero;
}

} // namespace

// Pure dual-wire expansion for recasthelpers::ShouldStampOnZeroRecast
// (RecastTime == 0 stamp before charged duration add; slice 2931).
auto runRecastStampZero2931SelfTests() -> bool
{
    using recasthelpers::ShouldStampOnZeroRecast;

    bool ok = true;

    const struct
    {
        bool        recastTimeIsZero;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { true, true, "zero recast stamps TimeStamp" },
        { false, false, "nonzero recast keeps TimeStamp path" },

        // Residual 1370 pins.
        { true, true, "residual stamp on zero" },
        { false, false, "residual no stamp when nonzero" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldStampOnZeroRecast(c.recastTimeIsZero);
        const bool inlineF = inlineShouldStampOnZeroRecast(c.recastTimeIsZero);
        const bool wantPin = c.recastTimeIsZero;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldStampOnZeroRecast dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldStampOnZeroRecast == pin formula recastTimeIsZero") && ok;
    }

    // Pin composition: identity only.
    ok = expect(ShouldStampOnZeroRecast(true), "zero recast must stamp") && ok;
    ok = expect(!ShouldStampOnZeroRecast(false), "nonzero recast must not stamp") && ok;

    // Dense compose: both bool inputs (full domain).
    for (const bool recastTimeIsZero : { false, true })
    {
        const bool got  = ShouldStampOnZeroRecast(recastTimeIsZero);
        const bool want = recastTimeIsZero;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldStampOnZeroRecast(recastTimeIsZero), "compose free == inline") && ok;
    }

    // Host-style inject poles: RecastTime == 0s / nonzero as bools.
    // (Live container Load stamp is residual 1370 / recast_container tests.)
    for (const bool recastIsZero : { true, false })
    {
        const bool inject = recastIsZero; // mirrors recast->RecastTime == 0s
        ok                = expect(ShouldStampOnZeroRecast(inject) == inject, "host inject dual-wire identity") && ok;
        ok                = expect(ShouldStampOnZeroRecast(inject) == inlineShouldStampOnZeroRecast(inject),
                    "host inject free == inline") &&
             ok;
    }

    return ok;
}
