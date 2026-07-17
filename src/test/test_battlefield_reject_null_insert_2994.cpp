#include "test_battlefield_reject_null_insert_2994.h"

#include "map/battlefield_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "battlefield ShouldRejectNullInsert 2994 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline InsertEntity null-PEntity formula for dual-wire cross-check (slice 2994):
//   entityNull
auto inlineShouldRejectNullInsert(const bool entityNull) -> bool
{
    return entityNull;
}

} // namespace

// Pure dual-wire expansion for battlefieldhelpers::ShouldRejectNullInsert
// (entityNull identity; slice 2994).
auto runBattlefieldRejectNullInsert2994SelfTests() -> bool
{
    using battlefieldhelpers::FormatInsertEntityNullWarning;
    using battlefieldhelpers::ShouldAcceptPCUnderCapacity;
    using battlefieldhelpers::ShouldRejectAlreadyInBattlefield;
    using battlefieldhelpers::ShouldRejectNullInsert;

    bool ok = true;

    // Residual 1361 truth-table pins.
    ok = expect(ShouldRejectNullInsert(true), "residual: null entity rejects") && ok;
    ok = expect(!ShouldRejectNullInsert(false), "residual: non-null entity proceeds") && ok;

    const struct
    {
        bool        entityNull;
        bool        want;
        const char* label;
    } cases[] = {
        { true, true, "null entity rejects" },
        { false, false, "non-null entity proceeds" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectNullInsert(c.entityNull);
        const bool inlineF = inlineShouldRejectNullInsert(c.entityNull);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectNullInsert dual-wire == inline LSB formula") && ok;
        ok = expect(got == c.entityNull, "free == entityNull (identity)") && ok;
    }

    // Pin composition: identity of entityNull only.
    ok = expect(ShouldRejectNullInsert(true), "entityNull must reject") && ok;
    ok = expect(!ShouldRejectNullInsert(false), "!entityNull must not reject") && ok;

    // Host path: CBattlefield::InsertEntity before other gates.
    const struct
    {
        bool        entityNull;
        bool        wantReject;
        const char* label;
    } hostCases[] = {
        { true, true, "PEntity == nullptr → warning + false before other gates" },
        { false, false, "PEntity non-null → proceed to already-in / capacity" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldRejectNullInsert(c.entityNull);
        const bool inlineF = inlineShouldRejectNullInsert(c.entityNull);

        ok = expect(got == c.wantReject, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == c.entityNull, "host compose free == entityNull (identity)") && ok;
    }

    // Dense compose: free == inline == pin formula for both bools.
    for (const bool entityNull : { false, true })
    {
        const bool got  = ShouldRejectNullInsert(entityNull);
        const bool want = entityNull;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldRejectNullInsert(entityNull),
                    "compose free == inline") &&
             ok;
    }

    // Residual independence (1361 / 2930): null reject is distinct from
    // already-in-battlefield and PC under-capacity gates.
    ok = expect(ShouldRejectNullInsert(true) && ShouldRejectAlreadyInBattlefield(true),
                "null reject and already-in both block via different helpers") &&
         ok;
    ok = expect(!ShouldRejectAlreadyInBattlefield(false),
                "not already-in must not reject via already-in gate") &&
         ok;
    ok = expect(!ShouldRejectNullInsert(false),
                "non-null must proceed even if later already-in / capacity fails") &&
         ok;
    ok = expect(ShouldAcceptPCUnderCapacity(2, 6) && !ShouldAcceptPCUnderCapacity(6, 6),
                "PC under-capacity residual still holds under dual-wire") &&
         ok;
    ok = expect(FormatInsertEntityNullWarning() == "CBattlefield::InsertEntity() - PEntity is null.",
                "null warning string residual") &&
         ok;

    return ok;
}
