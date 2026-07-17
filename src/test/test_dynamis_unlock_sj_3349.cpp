#include "test_dynamis_unlock_sj_3349.h"

#include "map/dynamis_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "dynamis ShouldUnlockSJRestriction 3349 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua / capacity formula for dual-wire cross-check
// (slice 3349 dedicated expand residual 2921/2857):
//   hasSJRestriction && option == 2
// Direct return form matching production free function / capacity.
auto inlineShouldUnlockSJRestriction(const bool hasSJ, const int option) -> bool
{
    return hasSJ && option == dynamishelpers::SomnialOptionUnlockSJ;
}

// Compact dual-wire pin matching free function / capacity body (slice 3349).
// Direct return only — same formula as production ShouldUnlockSJRestriction:
//   hasSJRestriction && IsSubjobUnlockOption(option)
auto pinShouldUnlockSJRestriction3349(const bool hasSJ, const int option) -> bool
{
    return hasSJ && dynamishelpers::IsSubjobUnlockOption(option);
}

} // namespace

// Pure dual-wire expansion for dynamishelpers::ShouldUnlockSJRestriction
// (somnialThresholdOnEventFinish unlock gate; OmegaXI internal/dynamis/zone.go;
// slice 3349 dedicated expand residual 2921/2857). Formula unchanged.
//
// Coverage:
//   - free == inline == pin == hasSJ && IsSubjobUnlockOption
//   - residual 1119 / 2921 pairing pins still hold
//   - residual poles: hasSJ on/off × unlock vs non-unlock option
//   - dense hasSJ × option poles
// Residual suite retained: test_dynamis_unlock_sj_2921.
// Sibling left alone: CanUnlockSJ / snap / eject.
// Not registered in CMake/main.
auto runDynamisUnlockSJ3349SelfTests() -> bool
{
    using dynamishelpers::CanUnlockSJ;
    using dynamishelpers::IsSubjobUnlockOption;
    using dynamishelpers::ShouldUnlockSJRestriction;
    using dynamishelpers::SomnialOptionUnlockSJ;

    bool ok = true;

    // Residual 1119 / 2921 pairing pins still hold under dual-wire.
    ok = expect(ShouldUnlockSJRestriction(true, SomnialOptionUnlockSJ), "residual: has SJ + unlock → true") && ok;
    ok = expect(!ShouldUnlockSJRestriction(false, SomnialOptionUnlockSJ), "residual: no SJ + unlock → false") && ok;
    ok = expect(!ShouldUnlockSJRestriction(true, 1), "residual: has SJ + leave → false") && ok;
    ok = expect(ShouldUnlockSJRestriction(true, 2), "residual: has SJ + option 2 → true") && ok;
    ok = expect(!ShouldUnlockSJRestriction(false, 2), "residual: no SJ + option 2 → false") && ok;

    // --- Residual poles: hasSJ on/off × unlock vs non-unlock option ---
    const struct
    {
        bool        hasSJ;
        int         option;
        bool        want;
        const char* label;
    } poles[] = {
        // Residual 1119 / 2921 classic poles.
        { true, SomnialOptionUnlockSJ, true, "residual has SJ + unlock → true" },
        { false, SomnialOptionUnlockSJ, false, "residual no SJ + unlock → false" },
        { true, 1, false, "residual has SJ + leave → false" },
        { false, 1, false, "residual no SJ + leave → false" },
        { true, 2, true, "residual has SJ + option 2 → true" },
        { false, 2, false, "residual no SJ + option 2 → false" },
        { true, 0, false, "residual has SJ + option 0 → false" },

        // Dual-wire polarity repeats.
        { true, SomnialOptionUnlockSJ, true, "polarity has SJ unlock" },
        { false, SomnialOptionUnlockSJ, false, "polarity no SJ unlock" },
        { true, 1, false, "polarity has SJ leave" },
        { false, 0, false, "polarity no SJ option 0" },
        { true, -1, false, "polarity has SJ invalid option" },
        { false, -1, false, "polarity no SJ invalid option" },
    };

    for (const auto& p : poles)
    {
        const bool got     = ShouldUnlockSJRestriction(p.hasSJ, p.option);
        const bool inlineF = inlineShouldUnlockSJRestriction(p.hasSJ, p.option);
        const bool pin     = pinShouldUnlockSJRestriction3349(p.hasSJ, p.option);
        const bool wantPin = p.hasSJ && IsSubjobUnlockOption(p.option);

        ok = expect(got == p.want, p.label) && ok;
        ok = expect(got == inlineF, "dual-wire free == inline LSB formula") && ok;
        ok = expect(got == pin, "dual-wire free == pin") && ok;
        ok = expect(got == wantPin, "dual-wire free == hasSJ && IsSubjobUnlockOption") && ok;
    }

    // Dense: free == inline == pin over hasSJ × host option poles.
    const int options[] = {
        SomnialOptionUnlockSJ, // 2
        1,                     // leave
        0,
        -1,
        3,
        2,
    };
    for (const bool hasSJ : { false, true })
    {
        for (const int option : options)
        {
            const bool got  = ShouldUnlockSJRestriction(hasSJ, option);
            const bool want = hasSJ && option == SomnialOptionUnlockSJ;
            ok              = expect(got == want, "dense free == pin formula") && ok;
            ok              = expect(got == inlineShouldUnlockSJRestriction(hasSJ, option), "dense free == inline") && ok;
            ok              = expect(got == pinShouldUnlockSJRestriction3349(hasSJ, option), "dense free == pin") && ok;
        }
    }

    // Explicit free-path poles matching capacity body (direct return).
    ok = expect(ShouldUnlockSJRestriction(true, 2), "has SJ + unlock must unlock") && ok;
    ok = expect(!ShouldUnlockSJRestriction(false, 2), "no SJ + unlock must not unlock") && ok;
    ok = expect(!ShouldUnlockSJRestriction(true, 1), "has SJ + leave must not unlock") && ok;
    ok = expect(!ShouldUnlockSJRestriction(false, 1), "no SJ + leave must not unlock") && ok;

    // Free == pin matching C++ across residual poles.
    ok = expect(ShouldUnlockSJRestriction(true, 2) == pinShouldUnlockSJRestriction3349(true, 2),
                "free==pin hasSJ+unlock") &&
         ok;
    ok = expect(ShouldUnlockSJRestriction(false, 2) == pinShouldUnlockSJRestriction3349(false, 2),
                "free==pin noSJ+unlock") &&
         ok;
    ok = expect(ShouldUnlockSJRestriction(true, 1) == pinShouldUnlockSJRestriction3349(true, 1),
                "free==pin hasSJ+leave") &&
         ok;

    // Branches distinct: hasSJ on/off with unlock; unlock vs leave with hasSJ.
    ok = expect(ShouldUnlockSJRestriction(true, 2) != ShouldUnlockSJRestriction(false, 2),
                "hasSJ on/off branches distinct with unlock") &&
         ok;
    ok = expect(ShouldUnlockSJRestriction(true, 2) != ShouldUnlockSJRestriction(true, 1),
                "unlock vs leave branches distinct with has SJ") &&
         ok;

    // Composition: free dual-wires hasSJ && IsSubjobUnlockOption (option half).
    ok = expect(ShouldUnlockSJRestriction(true, SomnialOptionUnlockSJ) == IsSubjobUnlockOption(SomnialOptionUnlockSJ),
                "compose unlock(true, unlock) == IsSubjobUnlockOption(unlock)") &&
         ok;
    ok = expect(!ShouldUnlockSJRestriction(true, 1), "compose unlock(true, leave) false") && ok;
    ok = expect(!ShouldUnlockSJRestriction(false, SomnialOptionUnlockSJ), "compose unlock(false, unlock) false") && ok;

    // Residual 2921 pairing: IsSubjobUnlockOption aliases option 2.
    ok = expect(IsSubjobUnlockOption(SomnialOptionUnlockSJ) && IsSubjobUnlockOption(2),
                "IsSubjobUnlockOption pins option 2") &&
         ok;
    ok = expect(!IsSubjobUnlockOption(1) && !IsSubjobUnlockOption(0),
                "IsSubjobUnlockOption rejects non-unlock") &&
         ok;

    // Residual 2921 CanUnlockSJ pairing left alone (not re-sliced): semantic
    // unlock gate matches CanUnlockSJ != 0 when option is unlock.
    ok = expect(CanUnlockSJ(true) == 1 && CanUnlockSJ(false) == 0, "CanUnlockSJ residual poles (left alone)") && ok;
    ok = expect(ShouldUnlockSJRestriction(CanUnlockSJ(true) != 0, SomnialOptionUnlockSJ),
                "compose: CanUnlockSJ(true)!=0 + unlock → unlock") &&
         ok;
    ok = expect(!ShouldUnlockSJRestriction(CanUnlockSJ(false) != 0, SomnialOptionUnlockSJ),
                "compose: CanUnlockSJ(false)==0 + unlock → no unlock") &&
         ok;

    return ok;
}
