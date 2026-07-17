#include "test_mapsession_null_char_2954.h"

#include "map/map_session_container_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "mapsession ShouldRejectNullCharLookup 2954 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline getSessionByChar null-PChar gate for dual-wire cross-check
// (slice 2954):
//   charNull
auto inlineShouldRejectNullCharLookup(const bool charNull) -> bool
{
    return charNull;
}

} // namespace

// Pure dual-wire expansion for mapsessionhelpers::ShouldRejectNullCharLookup
// (charNull identity; slice 2954).
auto runMapsessionNullChar2954SelfTests() -> bool
{
    using mapsessionhelpers::SessionMatchesCharID;
    using mapsessionhelpers::ShouldRejectNullCharLookup;

    bool ok = true;

    // Residual 2790 truth-table pins.
    ok = expect(ShouldRejectNullCharLookup(true), "residual: null char rejects") && ok;
    ok = expect(!ShouldRejectNullCharLookup(false), "residual: non-null char proceeds") && ok;

    const struct
    {
        bool        charNull;
        bool        want;
        const char* label;
    } cases[] = {
        { true, true, "null char rejects" },
        { false, false, "non-null char proceeds" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectNullCharLookup(c.charNull);
        const bool inlineF = inlineShouldRejectNullCharLookup(c.charNull);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectNullCharLookup dual-wire == inline LSB formula") && ok;
    }

    // Pin composition: identity of charNull only.
    ok = expect(ShouldRejectNullCharLookup(true), "charNull must reject") && ok;
    ok = expect(!ShouldRejectNullCharLookup(false), "!charNull must not reject") && ok;

    // Host path: before confirmed-map scan.
    const struct
    {
        bool        charNull;
        bool        wantReject;
        const char* label;
    } hostCases[] = {
        { true, true, "PChar == nullptr → reject before scan" },
        { false, false, "PChar non-null → proceed to scan" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldRejectNullCharLookup(c.charNull);
        const bool inlineF = inlineShouldRejectNullCharLookup(c.charNull);

        ok = expect(got == c.wantReject, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == c.charNull, "host compose free == charNull (identity)") && ok;
    }

    // Dense compose: free == inline == pin formula for both bools.
    for (const bool charNull : { false, true })
    {
        const bool got  = ShouldRejectNullCharLookup(charNull);
        const bool want = charNull;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldRejectNullCharLookup(charNull),
                    "compose free == inline") &&
             ok;
    }

    // Residual independence (2790): null reject and no-char match both block
    // via different pure helpers.
    ok = expect(ShouldRejectNullCharLookup(true) && !SessionMatchesCharID(false, 1, 1),
                "null reject and no-char mismatch both block") &&
         ok;
    ok = expect(!ShouldRejectNullCharLookup(false),
                "non-null must proceed even if later match fails") &&
         ok;
    ok = expect(SessionMatchesCharID(true, 42, 42),
                "has-char id match still holds under dual-wire residual") &&
         ok;

    return ok;
}
