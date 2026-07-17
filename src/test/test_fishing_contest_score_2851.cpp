#include "test_fishing_contest_score_2851.h"

#include "map/fishing_contest_capacity.h"
#include "map/fishingcontest.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "fishing contest score 2851 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua scoreFish formula for dual-wire checks.
auto inlineScoreFish(const uint32 length, const uint32 weight, const uint8 criteria) -> uint32
{
    if (criteria == 0) // SIZE
    {
        return length;
    }
    if (criteria == 1) // WEIGHT
    {
        return weight;
    }
    return length + weight;
}

} // namespace

// Pure dual-wire expansion for fishingcontesthelpers::ScoreFish /
// fishingcontest::ScoreFish (Lua local scoreFish parity).
// SIZE→length, WEIGHT→weight, else length+weight.
auto runFishingContestScore2851SelfTests() -> bool
{
    using fishingcontesthelpers::ScoreFish;

    bool ok = true;

    // SIZE → length only.
    ok = expect(ScoreFish(120, 450, 0) == 120, "SIZE returns length") && ok;
    ok = expect(ScoreFish(0, 999, 0) == 0, "SIZE zero length") && ok;
    ok = expect(ScoreFish(300, 0, 0) == 300, "SIZE ignores weight") && ok;

    // WEIGHT → weight only.
    ok = expect(ScoreFish(120, 450, 1) == 450, "WEIGHT returns weight") && ok;
    ok = expect(ScoreFish(999, 0, 1) == 0, "WEIGHT zero weight") && ok;
    ok = expect(ScoreFish(0, 1500, 1) == 1500, "WEIGHT ignores length") && ok;

    // BOTH (2) → length + weight.
    ok = expect(ScoreFish(120, 450, 2) == 570, "BOTH length+weight") && ok;
    ok = expect(ScoreFish(0, 0, 2) == 0, "BOTH zeros") && ok;
    ok = expect(ScoreFish(300, 1500, 2) == 1800, "BOTH uint16-range") && ok;

    // Unknown criteria falls through to length + weight (Lua else).
    ok = expect(ScoreFish(10, 20, 99) == 30, "unknown criteria else") && ok;
    ok = expect(ScoreFish(1, 2, 255) == 3, "max criteria else") && ok;

    // Dual-wire matches inline formula across a small table.
    const struct
    {
        uint32      length;
        uint32      weight;
        uint8       criteria;
        uint32      want;
        const char* label;
    } cases[] = {
        { 120, 450, 0, 120, "table SIZE" },
        { 120, 450, 1, 450, "table WEIGHT" },
        { 120, 450, 2, 570, "table BOTH" },
        { 10, 20, 99, 30, "table unknown" },
        { 0, 0, 0, 0, "table zeros SIZE" },
        { 0, 0, 1, 0, "table zeros WEIGHT" },
        { 0, 0, 2, 0, "table zeros BOTH" },
        { 1, 0, 0, 1, "table length only SIZE" },
        { 0, 1, 1, 1, "table weight only WEIGHT" },
        { 0xFFFF, 0xFFFF, 2, 0x1FFFE, "table max uint16 sum" },
    };

    for (const auto& c : cases)
    {
        const uint32 got       = ScoreFish(c.length, c.weight, c.criteria);
        const uint32 inlineGot = inlineScoreFish(c.length, c.weight, c.criteria);
        ok                     = expect(got == c.want, c.label) && ok;
        ok                     = expect(got == inlineGot, "dual-wire matches inline formula") && ok;
    }

    // Production free function dual-wires pure helper with enum criteria.
    ok = expect(fishingcontest::ScoreFish(120, 450, FISHING_CONTEST_CRITERIA::SIZE) == 120, "host SIZE") && ok;
    ok = expect(fishingcontest::ScoreFish(120, 450, FISHING_CONTEST_CRITERIA::WEIGHT) == 450, "host WEIGHT") && ok;
    ok = expect(fishingcontest::ScoreFish(120, 450, FISHING_CONTEST_CRITERIA::BOTH) == 570, "host BOTH") && ok;
    ok = expect(
             fishingcontest::ScoreFish(120, 450, FISHING_CONTEST_CRITERIA::SIZE) ==
                 ScoreFish(120, 450, static_cast<uint8>(FISHING_CONTEST_CRITERIA::SIZE)),
             "host dual-wires pure SIZE") &&
         ok;
    ok = expect(
             fishingcontest::ScoreFish(120, 450, FISHING_CONTEST_CRITERIA::WEIGHT) ==
                 ScoreFish(120, 450, static_cast<uint8>(FISHING_CONTEST_CRITERIA::WEIGHT)),
             "host dual-wires pure WEIGHT") &&
         ok;
    ok = expect(
             fishingcontest::ScoreFish(120, 450, FISHING_CONTEST_CRITERIA::BOTH) ==
                 ScoreFish(120, 450, static_cast<uint8>(FISHING_CONTEST_CRITERIA::BOTH)),
             "host dual-wires pure BOTH") &&
         ok;

    return ok;
}
