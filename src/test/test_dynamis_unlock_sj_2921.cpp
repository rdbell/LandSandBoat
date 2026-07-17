#include "test_dynamis_unlock_sj_2921.h"

#include "map/dynamis_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "dynamis CanUnlockSJ 2921 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua formula for dual-wire cross-check:
//   hasStatusEffect(SJ_RESTRICTION) and 1 or 0
auto inlineCanUnlockSJ(const bool hasSJRestriction) -> int
{
    return hasSJRestriction ? 1 : 0;
}

} // namespace

// Pure dual-wire expansion for dynamishelpers::CanUnlockSJ
// (somnialThresholdOnTrigger startEvent param; OmegaXI internal/dynamis).
auto runDynamisUnlockSJ2921SelfTests() -> bool
{
    using dynamishelpers::CanUnlockSJ;

    bool ok = true;

    // --- Residual 1077 pins ---
    ok = expect(CanUnlockSJ(true) == 1, "has SJ_RESTRICTION → 1") && ok;
    ok = expect(CanUnlockSJ(false) == 0, "no SJ_RESTRICTION → 0") && ok;

    // --- Dual-wire free == inline Lua formula ---
    const struct
    {
        bool        hasSJ;
        int         want;
        const char* label;
    } cases[] = {
        { true, 1, "true → 1 (unlock available)" },
        { false, 0, "false → 0 (no unlock option)" },
    };

    for (const auto& c : cases)
    {
        const int got     = CanUnlockSJ(c.hasSJ);
        const int inlineF = inlineCanUnlockSJ(c.hasSJ);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "dual-wire free == inline Lua formula") && ok;
    }

    // Composition: free function is single source of truth for startEvent param2.
    ok = expect(CanUnlockSJ(true) == 1 && CanUnlockSJ(false) == 0, "compose both branches") && ok;
    ok = expect(CanUnlockSJ(true) != CanUnlockSJ(false), "branches distinct") && ok;

    // menuBits pairing (somnial residual): 5 + canUnlockSJ*2
    ok = expect(5 + CanUnlockSJ(false) * 2 == 5, "menuBits no SJ = 5") && ok;
    ok = expect(5 + CanUnlockSJ(true) * 2 == 7, "menuBits with SJ = 7") && ok;

    return ok;
}
