#include "test_spell_can_use_with_2970.h"

#include "map/can_use_spell_with_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spell CanUseSpellWith 2970 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline spell::CanUseSpellWith formula for dual-wire cross-check (slice 2970):
//   if !hasSpell → false
//   else         → level > jobLevel
auto inlineCanUseSpellWith(const bool hasSpell, const std::uint8_t jobLevel, const std::uint8_t level) -> bool
{
    if (!hasSpell)
    {
        return false;
    }
    return level > jobLevel;
}

// Pin one-liner: hasSpell && level > jobLevel
auto pinCanUseSpellWith(const bool hasSpell, const std::uint8_t jobLevel, const std::uint8_t level) -> bool
{
    return hasSpell && level > jobLevel;
}

} // namespace

// Pure dual-wire expansion for canusespellwithhelpers::CanUseSpellWith
// (!hasSpell → false; else level > jobLevel; slice 2970).
auto runSpellCanUseWith2970SelfTests() -> bool
{
    using canusespellwithhelpers::CanUseSpellWith;

    bool ok = true;

    // Residual 1731 truth-table pins still hold under dual-wire.
    ok = expect(!CanUseSpellWith(false, 1, 99), "residual: missing spell rejects") && ok;
    ok = expect(CanUseSpellWith(true, 10, 11), "residual: level 11 > job 10 permits") && ok;
    ok = expect(!CanUseSpellWith(true, 10, 10), "residual: equal level strict rejects") && ok;
    ok = expect(!CanUseSpellWith(true, 10, 9), "residual: level below rejects") && ok;
    ok = expect(!CanUseSpellWith(true, 255, 99), "residual: unusable jobLevel 255 rejects") && ok;
    ok = expect(!CanUseSpellWith(true, 255, 255), "residual: 255/255 rejects") && ok;
    ok = expect(CanUseSpellWith(true, 0, 1), "residual: job 0 level 1 permits") && ok;

    const struct
    {
        bool          hasSpell;
        std::uint8_t  jobLevel;
        std::uint8_t  level;
        bool          want;
        const char*   label;
    } cases[] = {
        // Missing spell short-circuit (any levels).
        { false, 0, 0, false, "missing spell zero levels" },
        { false, 1, 99, false, "missing spell high level" },
        { false, 255, 255, false, "missing spell 255/255" },
        { false, 0, 255, false, "missing spell max level" },
        { false, 254, 255, false, "missing spell near-max still rejects" },

        // Strict greater-than when hasSpell.
        { true, 10, 11, true, "level 11 > job 10" },
        { true, 1, 99, true, "level 99 > job 1" },
        { true, 10, 10, false, "level == jobLevel false" },
        { true, 10, 9, false, "level < jobLevel false" },
        { true, 0, 0, false, "both zero false" },
        { true, 0, 1, true, "job 0 level 1 true" },
        { true, 254, 255, true, "254/255 true" },
        { true, 255, 255, false, "255/255 false" },

        // getJob unusable mapping: stored 0 → 255.
        { true, 255, 0, false, "unusable jobLevel 255 / 0" },
        { true, 255, 99, false, "unusable jobLevel 255 / 99" },
        { true, 255, 254, false, "unusable jobLevel 255 / 254" },
        { true, 255, 255, false, "unusable jobLevel 255 / 255" },

        // Boundary poles.
        { true, 0, 255, true, "job 0 max level permits" },
        { true, 1, 1, false, "equal 1 rejects" },
        { true, 1, 2, true, "1 < 2 permits" },
        { true, 99, 100, true, "99 < 100 permits" },
        { true, 99, 99, false, "equal 99 rejects" },
        { true, 99, 98, false, "below 99 rejects" },

        // Residual 1731 re-pins.
        { false, 1, 99, false, "residual missing spell" },
        { true, 10, 11, true, "residual level above" },
        { true, 10, 10, false, "residual equal" },
        { true, 10, 9, false, "residual below" },
        { true, 255, 99, false, "residual unusable" },
        { true, 0, 1, true, "residual job 0 level 1" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanUseSpellWith(c.hasSpell, c.jobLevel, c.level);
        const bool inlineF = inlineCanUseSpellWith(c.hasSpell, c.jobLevel, c.level);
        const bool wantPin = pinCanUseSpellWith(c.hasSpell, c.jobLevel, c.level);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanUseSpellWith dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "CanUseSpellWith == pin formula") && ok;
    }

    // Pin composition: free function is the three-arg inject only.
    ok = expect(!CanUseSpellWith(false, 0, 255), "!hasSpell must reject") && ok;
    ok = expect(CanUseSpellWith(true, 10, 11), "hasSpell + level > jobLevel must permit") && ok;
    ok = expect(!CanUseSpellWith(true, 10, 10), "hasSpell + equal must reject (strict >)") && ok;
    ok = expect(!CanUseSpellWith(true, 255, 255), "jobLevel 255 must reject") && ok;

    // Host path: after GetSpell + getJob inject (mobutils odd-job assign).
    const struct
    {
        bool          hasSpell;
        std::uint8_t  jobLevel;
        std::uint8_t  level;
        bool          wantUse;
        const char*   label;
    } hostCases[] = {
        { false, 10, 99, false, "GetSpell nil → reject before getJob" },
        { false, 0, 0, false, "GetSpell nil ignores levels" },
        { true, 10, 11, true, "has spell level > jobLevel → assign" },
        { true, 10, 10, false, "has spell equal level → no assign (strict)" },
        { true, 10, 9, false, "has spell below → no assign" },
        { true, 255, 99, false, "unusable job (getJob 0→255) → no assign" },
        { true, 255, 255, false, "unusable job max level still no assign" },
        { true, 1, 99, true, "low jobLevel high level → assign" },
        { true, 0, 1, true, "jobLevel 0 level 1 → assign" },
        { true, 254, 255, true, "near-max → assign" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = CanUseSpellWith(c.hasSpell, c.jobLevel, c.level);
        const bool inlineF = inlineCanUseSpellWith(c.hasSpell, c.jobLevel, c.level);

        ok = expect(got == c.wantUse, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == pinCanUseSpellWith(c.hasSpell, c.jobLevel, c.level),
                    "host compose free == pin formula") &&
             ok;
    }

    // Dense compose over representative poles: free == pin == inline.
    const bool          bools[]  = { false, true };
    const std::uint8_t  levels[] = { 0, 1, 9, 10, 11, 99, 254, 255 };
    for (const bool hasSpell : bools)
    {
        for (const std::uint8_t jobLevel : levels)
        {
            for (const std::uint8_t level : levels)
            {
                const bool got  = CanUseSpellWith(hasSpell, jobLevel, level);
                const bool want = pinCanUseSpellWith(hasSpell, jobLevel, level);
                ok              = expect(got == want, "compose free == pin formula") && ok;
                ok              = expect(got == inlineCanUseSpellWith(hasSpell, jobLevel, level),
                            "compose free == inline") &&
                     ok;
            }
        }
    }

    // Residual independence: strict > (this surface) vs CanUseSpell >=
    // (job gates residual 1732). Equal level differs.
    ok = expect(!CanUseSpellWith(true, 10, 10),
                "strict > dual-wire must reject equal (contrast CanUseSpell >=)") &&
         ok;
    ok = expect(CanUseSpellWith(true, 10, 11), "strict > dual-wire must permit above") && ok;

    return ok;
}
