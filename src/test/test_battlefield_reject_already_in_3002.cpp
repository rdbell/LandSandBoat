#include "test_battlefield_reject_already_in_3002.h"

#include "map/battlefield_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "battlefield ShouldRejectAlreadyInBattlefield 3002 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline InsertEntity already-in-battlefield formula for dual-wire cross-check
// (slice 3002):
//   hasBattlefield
auto inlineShouldRejectAlreadyInBattlefield(const bool hasBattlefield) -> bool
{
    return hasBattlefield;
}

} // namespace

// Pure dual-wire expansion for battlefieldhelpers::ShouldRejectAlreadyInBattlefield
// (hasBattlefield identity; slice 3002).
auto runBattlefieldRejectAlreadyIn3002SelfTests() -> bool
{
    using battlefieldhelpers::FormatInsertEntityNullWarning;
    using battlefieldhelpers::ShouldAcceptPCUnderCapacity;
    using battlefieldhelpers::ShouldRejectAlreadyInBattlefield;
    using battlefieldhelpers::ShouldRejectNullInsert;

    bool ok = true;

    // Residual 1361 truth-table pins.
    ok = expect(ShouldRejectAlreadyInBattlefield(true), "residual: already-in battlefield rejects") && ok;
    ok = expect(!ShouldRejectAlreadyInBattlefield(false), "residual: not already-in proceeds") && ok;

    const struct
    {
        bool        hasBattlefield;
        bool        want;
        const char* label;
    } cases[] = {
        { true, true, "already-in battlefield rejects" },
        { false, false, "not already-in proceeds" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectAlreadyInBattlefield(c.hasBattlefield);
        const bool inlineF = inlineShouldRejectAlreadyInBattlefield(c.hasBattlefield);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectAlreadyInBattlefield dual-wire == inline LSB formula") && ok;
        ok = expect(got == c.hasBattlefield, "free == hasBattlefield (identity)") && ok;
    }

    // Pin composition: identity of hasBattlefield only.
    ok = expect(ShouldRejectAlreadyInBattlefield(true), "hasBattlefield must reject") && ok;
    ok = expect(!ShouldRejectAlreadyInBattlefield(false), "!hasBattlefield must not reject") && ok;

    // Host path: CBattlefield::InsertEntity after null gate, before capacity.
    const struct
    {
        bool        hasBattlefield;
        bool        wantReject;
        const char* label;
    } hostCases[] = {
        { true, true, "PBattlefield != nullptr → return false before capacity / type" },
        { false, false, "PBattlefield == nullptr → proceed to capacity / type" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldRejectAlreadyInBattlefield(c.hasBattlefield);
        const bool inlineF = inlineShouldRejectAlreadyInBattlefield(c.hasBattlefield);

        ok = expect(got == c.wantReject, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == c.hasBattlefield, "host compose free == hasBattlefield (identity)") && ok;
    }

    // Dense compose: free == inline == pin formula for both bools.
    for (const bool hasBattlefield : { false, true })
    {
        const bool got  = ShouldRejectAlreadyInBattlefield(hasBattlefield);
        const bool want = hasBattlefield;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldRejectAlreadyInBattlefield(hasBattlefield),
                    "compose free == inline") &&
             ok;
    }

    // Residual independence (1361 / 2994 / 2930): already-in reject is distinct
    // from null-insert and PC under-capacity gates.
    ok = expect(ShouldRejectAlreadyInBattlefield(true) && ShouldRejectNullInsert(true),
                "already-in and null-insert both block via different helpers") &&
         ok;
    ok = expect(!ShouldRejectNullInsert(false),
                "non-null must not reject via null-insert gate") &&
         ok;
    ok = expect(!ShouldRejectAlreadyInBattlefield(false),
                "not already-in must proceed even if later capacity fails") &&
         ok;
    ok = expect(ShouldAcceptPCUnderCapacity(2, 6) && !ShouldAcceptPCUnderCapacity(6, 6),
                "PC under-capacity residual still holds under dual-wire") &&
         ok;
    ok = expect(FormatInsertEntityNullWarning() == "CBattlefield::InsertEntity() - PEntity is null.",
                "null warning string residual") &&
         ok;

    return ok;
}
