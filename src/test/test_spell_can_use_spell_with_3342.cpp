#include "test_spell_can_use_spell_with_3342.h"

#include "map/can_use_spell_with_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spell CanUseSpellWith 3342 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline spell::CanUseSpellWith formula for dual-wire cross-check
// (slice 3342 dedicated expand residual 2970; prior 3159 / residual 2970):
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

// Positive if/else pin matching free function / capacity body (slice 3342).
// Avoid De Morgan rewrites of the compound conjunction (QF1001).
//   if (hasSpell) { return level > jobLevel; }
//   return false;
auto pinCanUseSpellWith3342(const bool hasSpell, const std::uint8_t jobLevel, const std::uint8_t level) -> bool
{
    if (hasSpell)
    {
        return level > jobLevel; // STRICT greater-than, not >=
    }
    return false;
}

// One-liner pin still matches positive form.
auto pinCanUseSpellWithOneLiner(const bool hasSpell, const std::uint8_t jobLevel, const std::uint8_t level) -> bool
{
    return hasSpell && level > jobLevel;
}

} // namespace

// Pure dual-wire expansion for canusespellwithhelpers::CanUseSpellWith
// (hasSpell && level > jobLevel; STRICT >; dedicated expand residual 2970,
// slice 3342). Residual 1731 / 2970 suite remains under
// test_can_use_spell_with_1731 / test_spell_can_use_with_2970. Prior dedicated
// suite remains test_spell_can_use_spell_with_3159.
//
// Coverage:
//   - poles hasSpell false / level==jobLevel / level>jobLevel
//   - free == inline == pin (positive if/else)
//   - residual 1731 / 2970 / prior 3159 pins still hold
//   - host inject compose + strict > vs CanUseSpell >= independence
//     (do not thrash job_gates)
auto runSpellCanUseSpellWith3342SelfTests() -> bool
{
    using canusespellwithhelpers::CanUseSpellWith;

    bool ok = true;

    // Residual 1731 / dual-wire 2970 / prior dedicated 3159 pins still hold.
    ok = expect(!CanUseSpellWith(false, 1, 99), "residual: missing spell rejects") && ok;
    ok = expect(CanUseSpellWith(true, 10, 11), "residual: level 11 > job 10 permits") && ok;
    ok = expect(!CanUseSpellWith(true, 10, 10), "residual: equal level strict rejects") && ok;
    ok = expect(!CanUseSpellWith(true, 10, 9), "residual: level below rejects") && ok;
    ok = expect(!CanUseSpellWith(true, 255, 99), "residual: unusable jobLevel 255 rejects") && ok;
    ok = expect(!CanUseSpellWith(true, 255, 255), "residual: 255/255 rejects") && ok;
    ok = expect(CanUseSpellWith(true, 0, 1), "residual: job 0 level 1 permits") && ok;

    // --- Required poles (positive pin form) ---
    // hasSpell false
    ok = expect(!CanUseSpellWith(false, 0, 0), "pole hasSpell false zero levels rejects") && ok;
    ok = expect(!CanUseSpellWith(false, 1, 99), "pole hasSpell false high level rejects") && ok;
    ok = expect(!CanUseSpellWith(false, 255, 255), "pole hasSpell false 255/255 rejects") && ok;
    ok = expect(!CanUseSpellWith(false, 0, 255), "pole hasSpell false max level rejects") && ok;
    ok = expect(!CanUseSpellWith(false, 10, 11), "pole hasSpell false would-permit still rejects") && ok;
    // level == jobLevel → false (strict >)
    ok = expect(!CanUseSpellWith(true, 10, 10), "pole level==jobLevel 10 rejects (strict >)") && ok;
    ok = expect(!CanUseSpellWith(true, 0, 0), "pole level==jobLevel 0 rejects") && ok;
    ok = expect(!CanUseSpellWith(true, 1, 1), "pole level==jobLevel 1 rejects") && ok;
    ok = expect(!CanUseSpellWith(true, 99, 99), "pole level==jobLevel 99 rejects") && ok;
    ok = expect(!CanUseSpellWith(true, 255, 255), "pole level==jobLevel 255 rejects") && ok;
    // level > jobLevel → true
    ok = expect(CanUseSpellWith(true, 10, 11), "pole level>jobLevel 11>10 permits") && ok;
    ok = expect(CanUseSpellWith(true, 0, 1), "pole level>jobLevel 1>0 permits") && ok;
    ok = expect(CanUseSpellWith(true, 1, 99), "pole level>jobLevel 99>1 permits") && ok;
    ok = expect(CanUseSpellWith(true, 254, 255), "pole level>jobLevel 255>254 permits") && ok;

    // Core table: free == inline == pin (positive if/else).
    const struct
    {
        bool          hasSpell;
        std::uint8_t  jobLevel;
        std::uint8_t  level;
        bool          want;
        const char*   label;
    } cases[] = {
        // hasSpell false poles
        { false, 0, 0, false, "hasSpell false zero levels" },
        { false, 1, 99, false, "hasSpell false high level" },
        { false, 255, 255, false, "hasSpell false 255/255" },
        { false, 0, 255, false, "hasSpell false max level" },
        { false, 254, 255, false, "hasSpell false near-max still rejects" },
        { false, 10, 11, false, "hasSpell false would-permit levels still reject" },

        // level==jobLevel poles (strict > rejects)
        { true, 10, 10, false, "level==jobLevel 10" },
        { true, 0, 0, false, "level==jobLevel 0" },
        { true, 1, 1, false, "level==jobLevel 1" },
        { true, 99, 99, false, "level==jobLevel 99" },
        { true, 255, 255, false, "level==jobLevel 255" },

        // level>jobLevel poles
        { true, 10, 11, true, "level>jobLevel 11>10" },
        { true, 1, 99, true, "level>jobLevel 99>1" },
        { true, 0, 1, true, "level>jobLevel 1>0" },
        { true, 254, 255, true, "level>jobLevel 255>254" },
        { true, 99, 100, true, "level>jobLevel 100>99" },
        { true, 0, 255, true, "level>jobLevel max over 0" },

        // level<jobLevel poles
        { true, 10, 9, false, "level<jobLevel 9<10" },
        { true, 99, 98, false, "level<jobLevel 98<99" },
        { true, 1, 0, false, "level<jobLevel 0<1" },

        // job255 poles (unusable getJob mapping)
        { true, 255, 0, false, "job255 / 0" },
        { true, 255, 99, false, "job255 / 99" },
        { true, 255, 254, false, "job255 / 254" },
        { true, 255, 255, false, "job255 / 255" },

        // Residual 1731 / 2970 / prior 3159 re-pins
        { false, 1, 99, false, "residual 1731 missing spell" },
        { true, 10, 11, true, "residual 1731 level above" },
        { true, 10, 10, false, "residual 1731 equal" },
        { true, 10, 9, false, "residual 1731 below" },
        { true, 255, 99, false, "residual 1731 unusable" },
        { true, 0, 1, true, "residual 1731 job 0 level 1" },
        { false, 1, 99, false, "residual 2970 missing spell" },
        { true, 10, 11, true, "residual 2970 level above" },
        { true, 10, 10, false, "residual 2970 equal" },
        { true, 10, 9, false, "residual 2970 below" },
        { true, 255, 99, false, "residual 2970 unusable" },
        { true, 0, 1, true, "residual 2970 job 0 level 1" },
        { false, 0, 0, false, "prior 3159 no-spell zero" },
        { true, 10, 10, false, "prior 3159 level==job" },
        { true, 10, 11, true, "prior 3159 level>job" },
        { true, 255, 255, false, "prior 3159 job255" },
    };

    for (const auto& c : cases)
    {
        const bool got     = CanUseSpellWith(c.hasSpell, c.jobLevel, c.level);
        const bool inlineF = inlineCanUseSpellWith(c.hasSpell, c.jobLevel, c.level);
        const bool pin     = pinCanUseSpellWith3342(c.hasSpell, c.jobLevel, c.level);
        const bool oneLine = pinCanUseSpellWithOneLiner(c.hasSpell, c.jobLevel, c.level);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "CanUseSpellWith dual-wire == inline LSB formula") && ok;
        ok = expect(got == pin, "CanUseSpellWith == positive pin") && ok;
        ok = expect(got == oneLine, "CanUseSpellWith == one-liner pin") && ok;
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
        const bool pin     = pinCanUseSpellWith3342(c.hasSpell, c.jobLevel, c.level);

        ok = expect(got == c.wantUse, c.label) && ok;
        ok = expect(got == inlineF, "host compose dual-wire free == inline") && ok;
        ok = expect(got == pin, "host compose free == positive pin") && ok;
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
                const bool got = CanUseSpellWith(hasSpell, jobLevel, level);
                const bool pin = pinCanUseSpellWith3342(hasSpell, jobLevel, level);
                ok             = expect(got == pin, "compose free == positive pin") && ok;
                ok             = expect(got == inlineCanUseSpellWith(hasSpell, jobLevel, level),
                            "compose free == inline") &&
                     ok;
            }
        }
    }

    // Residual independence: strict > (this surface) vs CanUseSpell >=
    // (job gates residual 1732). Equal level differs. Do not thrash job_gates.
    ok = expect(!CanUseSpellWith(true, 10, 10),
                "strict > dual-wire must reject equal (contrast CanUseSpell >=)") &&
         ok;
    ok = expect(CanUseSpellWith(true, 10, 11), "strict > dual-wire must permit above") && ok;

    return ok;
}
