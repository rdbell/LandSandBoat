#include "test_petutils_extend_charm_3220.h"

#include "map/pet_mod_tandem_capacity.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "petutils ShouldExtendCharm 3220 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline ExtendCharm early-return formula for dual-wire cross-check
// (dedicated slice 3220):
//   isMob && isCharmed && minSeconds <= maxSeconds && maxSeconds != 0
auto inlineShouldExtendCharm(const bool isMob, const bool isCharmed, const std::uint16_t minSeconds,
                             const std::uint16_t maxSeconds) -> bool
{
    return isMob && isCharmed && minSeconds <= maxSeconds && maxSeconds != 0;
}

// Compact dual-wire pin matching Go pinShouldExtendCharm3220 / C++ capacity
// (direct return expression; formula unchanged from residual 2922 / pure 1624):
//   isMob && isCharmed && minSeconds <= maxSeconds && maxSeconds != 0
auto pinShouldExtendCharm(const bool isMob, const bool isCharmed, const std::uint16_t minSeconds,
                          const std::uint16_t maxSeconds) -> bool
{
    return isMob && isCharmed && minSeconds <= maxSeconds && maxSeconds != 0;
}

} // namespace

// Pure dual-wire expansion for petmodtandemhelpers::ShouldExtendCharm
// (ExtendCharm eligibility + range gates; dedicated slice 3220 expand
// residual 2922 / pure 1624).
//
// Coverage:
//   - free == inline == pin == formula
//   - residual 2922 poles: not-mob, not-charmed, min>max, max==0, happy path
//   - dense poles: all bool combos for isMob/isCharmed; range edges
//     (0,0)/(0,1)/(1,0)/(1,1)/(5,5)/(6,5)/(0,65535)/(65535,65535)/(65535,0)
auto runPetutilsExtendCharm3220SelfTests() -> bool
{
    using petmodtandemhelpers::CanExtendCharm;
    using petmodtandemhelpers::CharmSecondsRangeValid;
    using petmodtandemhelpers::ShouldExtendCharm;

    bool ok = true;

    // Residual 1624 / 2922 component pins still hold under dual-wire.
    ok = expect(CanExtendCharm(true, true), "residual 2922/1624: CanExtendCharm charmed mob") && ok;
    ok = expect(!CanExtendCharm(true, false), "residual 2922/1624: CanExtendCharm mob not charmed") && ok;
    ok = expect(!CanExtendCharm(false, true), "residual 2922/1624: CanExtendCharm non-mob charmed") && ok;
    ok = expect(!CanExtendCharm(false, false), "residual 2922/1624: CanExtendCharm non-mob not charmed") && ok;

    ok = expect(CharmSecondsRangeValid(1, 5), "residual 2922/1624: range 1..5 valid") && ok;
    ok = expect(CharmSecondsRangeValid(3, 3), "residual 2922/1624: range equal valid") && ok;
    ok = expect(!CharmSecondsRangeValid(5, 1), "residual 2922/1624: range inverted invalid") && ok;
    ok = expect(!CharmSecondsRangeValid(0, 0), "residual 2922/1624: range max 0 invalid") && ok;
    ok = expect(!CharmSecondsRangeValid(1, 0), "residual 2922/1624: range max 0 with min invalid") && ok;

    // Residual 2922 poles: happy path, not-mob, not-charmed, min>max, max==0.
    ok = expect(ShouldExtendCharm(true, true, 1, 10), "residual 2922: happy path") && ok;
    ok = expect(!ShouldExtendCharm(false, true, 1, 10), "residual 2922: not-mob blocks") && ok;
    ok = expect(!ShouldExtendCharm(true, false, 1, 10), "residual 2922: not-charmed blocks") && ok;
    ok = expect(!ShouldExtendCharm(true, true, 5, 1), "residual 2922: min>max blocks") && ok;
    ok = expect(!ShouldExtendCharm(true, true, 0, 0), "residual 2922: max==0 blocks") && ok;

    // --- Composition table: free == inline == pin ---
    const struct
    {
        bool          isMob;
        bool          isCharmed;
        std::uint16_t minSeconds;
        std::uint16_t maxSeconds;
        bool          want;
        const char*   label;
    } cases[] = {
        // Residual 2922 poles.
        { true, true, 1, 10, true, "residual happy path" },
        { false, true, 1, 10, false, "residual not-mob" },
        { true, false, 1, 10, false, "residual not-charmed" },
        { true, true, 5, 1, false, "residual min>max" },
        { true, true, 0, 0, false, "residual max==0" },
        { true, true, 3, 3, true, "residual equal range" },
        { true, true, 0, 1, true, "residual min 0 max 1" },
        { true, true, 0, 0xffff, true, "residual full uint16 range" },
        { false, false, 1, 10, false, "residual non-mob not charmed" },
        { true, true, 1, 0, false, "residual max 0 with min" },
        { false, true, 5, 1, false, "residual non-mob + inverted" },
        { true, false, 0, 0, false, "residual not charmed + max 0" },
        { true, true, 100, 100, true, "residual equal high range" },
        { true, true, 100, 99, false, "residual min > max by one" },

        // Range edge poles (required dense edges).
        { true, true, 0, 0, false, "edge (0,0) max 0" },
        { true, true, 0, 1, true, "edge (0,1) min 0 max 1" },
        { true, true, 1, 0, false, "edge (1,0) max 0 with min" },
        { true, true, 1, 1, true, "edge (1,1) equal" },
        { true, true, 5, 5, true, "edge (5,5) equal" },
        { true, true, 6, 5, false, "edge (6,5) min>max" },
        { true, true, 0, 65535, true, "edge (0,65535) full uint16" },
        { true, true, 65535, 65535, true, "edge (65535,65535) both max" },
        { true, true, 65535, 0, false, "edge (65535,0) max 0 min high" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldExtendCharm(c.isMob, c.isCharmed, c.minSeconds, c.maxSeconds);
        const bool inlineF = inlineShouldExtendCharm(c.isMob, c.isCharmed, c.minSeconds, c.maxSeconds);
        const bool pinF    = pinShouldExtendCharm(c.isMob, c.isCharmed, c.minSeconds, c.maxSeconds);
        const bool wantPin = c.isMob && c.isCharmed && c.minSeconds <= c.maxSeconds && c.maxSeconds != 0;
        const bool compose = CanExtendCharm(c.isMob, c.isCharmed) &&
                             CharmSecondsRangeValid(c.minSeconds, c.maxSeconds);

        ok = expect(got == c.want, c.label) && ok;
        // Positive form: free == inline == pin.
        ok = expect(got == inlineF && got == pinF, "dual-wire free == inline == pin") && ok;
        ok = expect(got == wantPin, "free == pin formula") && ok;
        ok = expect(got == compose, "free == CanExtendCharm && CharmSecondsRangeValid") && ok;
    }

    // Dense poles: all bool combos for isMob/isCharmed with a valid range.
    for (const bool isMob : { false, true })
    {
        for (const bool isCharmed : { false, true })
        {
            const bool got     = ShouldExtendCharm(isMob, isCharmed, 1, 10);
            const bool inlineF = inlineShouldExtendCharm(isMob, isCharmed, 1, 10);
            const bool pinF    = pinShouldExtendCharm(isMob, isCharmed, 1, 10);
            const bool want    = isMob && isCharmed;
            ok                 = expect(got == want, "bool dense free == want") && ok;
            ok                 = expect(got == inlineF && got == pinF, "bool dense free == inline == pin") && ok;
        }
    }

    // Dense range edges × full 2² bool space: free == inline == pin.
    const struct
    {
        std::uint16_t minSeconds;
        std::uint16_t maxSeconds;
    } rangeEdges[] = {
        { 0, 0 },
        { 0, 1 },
        { 1, 0 },
        { 1, 1 },
        { 5, 5 },
        { 6, 5 },
        { 0, 65535 },
        { 65535, 65535 },
        { 65535, 0 },
    };

    for (const bool isMob : { false, true })
    {
        for (const bool isCharmed : { false, true })
        {
            for (const auto& re : rangeEdges)
            {
                const bool got     = ShouldExtendCharm(isMob, isCharmed, re.minSeconds, re.maxSeconds);
                const bool inlineF = inlineShouldExtendCharm(isMob, isCharmed, re.minSeconds, re.maxSeconds);
                const bool pinF    = pinShouldExtendCharm(isMob, isCharmed, re.minSeconds, re.maxSeconds);
                const bool want    = isMob && isCharmed && re.minSeconds <= re.maxSeconds && re.maxSeconds != 0;
                const bool compose = CanExtendCharm(isMob, isCharmed) &&
                                     CharmSecondsRangeValid(re.minSeconds, re.maxSeconds);

                ok = expect(got == want, "compose free == pin formula") && ok;
                ok = expect(got == inlineF && got == pinF, "compose free == inline == pin") && ok;
                ok = expect(got == compose, "compose free == helpers AND") && ok;
            }
        }
    }

    // Host path semantics: single-gate inject before RNG / charmTime writeback.
    const struct
    {
        bool          isMob;
        bool          isCharmed;
        std::uint16_t minSeconds;
        std::uint16_t maxSeconds;
        bool          wantExtend;
        const char*   label;
    } hostCases[] = {
        { true, true, 1, 10, true, "charmed mob valid → RNG + charmTime path" },
        { false, true, 1, 10, false, "not-mob → early return" },
        { true, false, 1, 10, false, "not-charmed → early return" },
        { true, true, 5, 1, false, "min>max → early return" },
        { true, true, 0, 0, false, "max==0 → early return" },
        { true, true, 0, 1, true, "min 0 max 1 → RNG path" },
        { true, true, 65535, 65535, true, "max uint16 equal → RNG path" },
        { true, true, 65535, 0, false, "max 0 min high → early return" },
    };

    for (const auto& c : hostCases)
    {
        const bool got     = ShouldExtendCharm(c.isMob, c.isCharmed, c.minSeconds, c.maxSeconds);
        const bool inlineF = inlineShouldExtendCharm(c.isMob, c.isCharmed, c.minSeconds, c.maxSeconds);
        const bool pinF    = pinShouldExtendCharm(c.isMob, c.isCharmed, c.minSeconds, c.maxSeconds);

        ok = expect(got == c.wantExtend, c.label) && ok;
        ok = expect(got == inlineF && got == pinF, "host compose dual-wire free == inline == pin") && ok;
    }

    return ok;
}
