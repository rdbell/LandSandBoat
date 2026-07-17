#include "test_fishing_contest_placeholders_3523.h"

#include "map/fishingcontest.h"
#include "map/fishingcontest_placeholders_capacity.h"

#include <cstdint>
#include <iostream>
#include <limits>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "fishing contest placeholders 3523 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline GeneratePlaceholderEntries gate for dual-wire cross-check (dedicated 3523).
// Capacity takes presenting bool; host injects status == PRESENTING:
//   presenting && realEntries < maxEntries
auto inlineShouldGeneratePlaceholderEntries3523(
    const bool         presenting,
    const std::uint8_t maxEntries,
    const std::uint8_t realEntries) -> bool
{
    return presenting && realEntries < maxEntries;
}

// Compact dual-wire pin matching Go pinShouldGeneratePlaceholderEntries3523 /
// C++ capacity AND form (formula unchanged from 2645 / 2855 / 3380):
//   presenting && realEntries < maxEntries
auto pinShouldGeneratePlaceholderEntries3523(
    const bool         presenting,
    const std::uint8_t maxEntries,
    const std::uint8_t realEntries) -> bool
{
    return presenting && realEntries < maxEntries;
}

// Prior dedicated 3380 pin (retained agreement check under 3523).
auto pinShouldGeneratePlaceholderEntries3380(
    const bool         presenting,
    const std::uint8_t maxEntries,
    const std::uint8_t realEntries) -> bool
{
    return presenting && realEntries < maxEntries;
}

} // namespace

// Pure dual-wire expansion for
// fishingcontestplaceholderhelpers::ShouldGeneratePlaceholderEntries
// (GeneratePlaceholderEntries PRESENTING + room gate;
// OmegaXI internal/fishingcontest; dedicated slice 3523; residual expand 2855 /
// prior dedicated 3380 / pure 2645 — formula unchanged).
//
// Coverage:
//   - free == inline == pin == (presenting && realEntries < maxEntries)
//   - residual 2855 / 2645 / prior dedicated 3380 pins still hold
//   - dense presenting × count edges free == inline == pin
//   - host inject poles for GeneratePlaceholderEntries path
//   - residual independence (PlaceholderEntryScore residual-ok)
auto runFishingContestPlaceholders3523SelfTests() -> bool
{
    using fishingcontestplaceholderhelpers::PlaceholderEntryScore;
    using fishingcontestplaceholderhelpers::ShouldGeneratePlaceholderEntries;

    bool ok = true;

    // Residual 2855 / 2645 / prior dedicated 3380 pins still hold under dedicated dual-wire.
    ok = expect(ShouldGeneratePlaceholderEntries(true, 15, 0), "residual: presenting empty generates") && ok;
    ok = expect(ShouldGeneratePlaceholderEntries(true, 15, 14), "residual: presenting one short generates") && ok;
    ok = expect(ShouldGeneratePlaceholderEntries(true, 4, 2), "residual: presenting half generates") && ok;
    ok = expect(!ShouldGeneratePlaceholderEntries(true, 15, 15), "residual: presenting full does not") && ok;
    ok = expect(!ShouldGeneratePlaceholderEntries(true, 4, 4), "residual: presenting equal max does not") && ok;
    ok = expect(!ShouldGeneratePlaceholderEntries(true, 4, 5), "residual: presenting over max does not") && ok;
    ok = expect(!ShouldGeneratePlaceholderEntries(false, 15, 0), "residual: non-presenting empty does not") && ok;
    ok = expect(!ShouldGeneratePlaceholderEntries(false, 4, 2), "residual: non-presenting partial does not") && ok;
    ok = expect(!ShouldGeneratePlaceholderEntries(true, 0, 0), "residual: zero max full does not") && ok;

    // --- Core poles: free == inline == pin AND form ---
    const struct
    {
        bool         presenting;
        std::uint8_t maxEntries;
        std::uint8_t realEntries;
        bool         want;
        const char*  label;
    } cases[] = {
        // Residual 2855 / 2645 poles — presenting with room.
        { true, 15, 0, true, "residual presenting empty" },
        { true, 15, 14, true, "residual presenting one short" },
        { true, 4, 2, true, "residual presenting half" },
        { true, 20, 18, true, "residual presenting 20/18 (2645 smallest host)" },

        // Residual — presenting full / over.
        { true, 15, 15, false, "residual presenting full" },
        { true, 4, 4, false, "residual presenting equal max" },
        { true, 4, 5, false, "residual presenting over max" },
        { true, 0, 0, false, "residual zero max full" },
        { true, 3, 3, false, "residual 2645 gate full" },

        // Residual — non-presenting never generates.
        { false, 15, 0, false, "residual non-presenting empty" },
        { false, 4, 0, false, "residual non-presenting room (host dual-wire)" },
        { false, 4, 2, false, "residual non-presenting partial" },
        { false, 255, 0, false, "non-presenting max255 empty" },
        { false, 255, 254, false, "non-presenting one short" },

        // Classic accept poles — PRESENTING + room.
        { true, 1, 0, true, "presenting max1 empty" },
        { true, 255, 0, true, "presenting max255 empty" },
        { true, 255, 254, true, "presenting max255 one short" },
        { true, 4, 0, true, "presenting 4 empty" },
        { true, 4, 3, true, "presenting 4 three filled" },

        // Boundary reject — PRESENTING but full / over.
        { true, 255, 255, false, "presenting max255 full" },
        { true, 1, 1, false, "presenting max1 full" },
        { true, 1, 2, false, "presenting max1 over" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldGeneratePlaceholderEntries(c.presenting, c.maxEntries, c.realEntries);
        const bool inlineF = inlineShouldGeneratePlaceholderEntries3523(c.presenting, c.maxEntries, c.realEntries);
        const bool pin     = pinShouldGeneratePlaceholderEntries3523(c.presenting, c.maxEntries, c.realEntries);
        // Positive form pin composition (explicit PRESENTING + room).
        const bool wantPin = c.presenting && c.realEntries < c.maxEntries;

        ok = expect(got == c.want, c.label) && ok;
        // Positive form: free == inline == pin.
        ok = expect(got == inlineF && got == pin, "dual-wire free == inline == pin") && ok;
        ok = expect(got == wantPin, "free == pin formula presenting && real < max") && ok;
    }

    // Free == pin across residual poles.
    ok = expect(ShouldGeneratePlaceholderEntries(true, 15, 0) ==
                    pinShouldGeneratePlaceholderEntries3523(true, 15, 0),
                "free == pin presenting empty generate") &&
         ok;
    ok = expect(ShouldGeneratePlaceholderEntries(true, 15, 14) ==
                    pinShouldGeneratePlaceholderEntries3523(true, 15, 14),
                "free == pin presenting one short generate") &&
         ok;
    ok = expect(ShouldGeneratePlaceholderEntries(true, 15, 15) ==
                    pinShouldGeneratePlaceholderEntries3523(true, 15, 15),
                "free == pin presenting full reject") &&
         ok;
    ok = expect(ShouldGeneratePlaceholderEntries(true, 4, 5) ==
                    pinShouldGeneratePlaceholderEntries3523(true, 4, 5),
                "free == pin presenting over reject") &&
         ok;
    ok = expect(ShouldGeneratePlaceholderEntries(false, 15, 0) ==
                    pinShouldGeneratePlaceholderEntries3523(false, 15, 0),
                "free == pin non-presenting reject") &&
         ok;
    ok = expect(ShouldGeneratePlaceholderEntries(true, 0, 0) ==
                    pinShouldGeneratePlaceholderEntries3523(true, 0, 0),
                "free == pin zero max reject") &&
         ok;

    // Dense compose: presenting × count edges free == inline == pin.
    const struct
    {
        std::uint8_t maxEntries;
        std::uint8_t realEntries;
    } countEdges[] = {
        { 0, 0 }, { 1, 0 }, { 1, 1 }, { 1, 2 },
        { 4, 0 }, { 4, 2 }, { 4, 3 }, { 4, 4 }, { 4, 5 },
        { 15, 0 }, { 15, 14 }, { 15, 15 },
        { 20, 18 }, { 255, 0 }, { 255, 254 }, { 255, 255 },
    };
    for (const bool presenting : { false, true })
    {
        for (const auto& c : countEdges)
        {
            const bool got     = ShouldGeneratePlaceholderEntries(presenting, c.maxEntries, c.realEntries);
            const bool inlineF = inlineShouldGeneratePlaceholderEntries3523(presenting, c.maxEntries, c.realEntries);
            const bool pin     = pinShouldGeneratePlaceholderEntries3523(presenting, c.maxEntries, c.realEntries);
            const bool want    = presenting && c.realEntries < c.maxEntries;
            ok                 = expect(got == want, "compose free == pin formula") && ok;
            ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
        }
    }

    // Production GeneratePlaceholderEntries path semantics:
    //   presenting && real < max → build placeholders (native max-entry loop)
    //   else → return empty
    ok = expect(ShouldGeneratePlaceholderEntries(true, 4, 2) &&
                    pinShouldGeneratePlaceholderEntries3523(true, 4, 2),
                "Generate path PRESENTING room → generate free/pin dual-wire") &&
         ok;
    ok = expect(!ShouldGeneratePlaceholderEntries(false, 4, 0) &&
                    !pinShouldGeneratePlaceholderEntries3523(false, 4, 0),
                "Generate path non-presenting → empty free/pin dual-wire") &&
         ok;
    ok = expect(!ShouldGeneratePlaceholderEntries(true, 4, 4) &&
                    !pinShouldGeneratePlaceholderEntries3523(true, 4, 4),
                "Generate path full → empty free/pin dual-wire") &&
         ok;

    // Host inject poles: free == inline == pin for GeneratePlaceholderEntries edges.
    // Host injects presenting = (status == PRESENTING).
    const struct
    {
        FISHING_CONTEST_STATUS status;
        std::uint8_t           maxEntries;
        std::uint8_t           realEntries;
        const char*            label;
    } hostPoles[] = {
        { FISHING_CONTEST_STATUS::ACCEPTING, 4, 0, "host ACCEPTING empty" },
        { FISHING_CONTEST_STATUS::PRESENTING, 4, 4, "host PRESENTING full" },
        { FISHING_CONTEST_STATUS::PRESENTING, 4, 2, "host PRESENTING half (greatest host)" },
        { FISHING_CONTEST_STATUS::PRESENTING, 20, 18, "host PRESENTING 20/18 (smallest host)" },
        { FISHING_CONTEST_STATUS::PRESENTING, 3, 0, "host PRESENTING empty max3" },
        { FISHING_CONTEST_STATUS::PRESENTING, 3, 3, "host PRESENTING full max3" },
        { FISHING_CONTEST_STATUS::CONTESTING, 15, 0, "host CONTESTING empty" },
        { FISHING_CONTEST_STATUS::CLOSED, 15, 14, "host CLOSED one short" },
    };
    for (const auto& p : hostPoles)
    {
        const bool presenting = p.status == FISHING_CONTEST_STATUS::PRESENTING;
        const bool got        = ShouldGeneratePlaceholderEntries(presenting, p.maxEntries, p.realEntries);
        const bool inlineF    = inlineShouldGeneratePlaceholderEntries3523(presenting, p.maxEntries, p.realEntries);
        const bool pin        = pinShouldGeneratePlaceholderEntries3523(presenting, p.maxEntries, p.realEntries);
        const bool inject     = presenting && p.realEntries < p.maxEntries;
        ok                    = expect(got == inject, p.label) && ok;
        ok                    = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;

        // Host dual-wire: free gate matches empty/non-empty GeneratePlaceholderEntries.
        const auto entries = fishingcontest::GeneratePlaceholderEntries(
            p.status, FISHING_CONTEST_MEASURE::GREATEST, p.maxEntries, p.realEntries);
        if (got)
        {
            ok = expect(!entries.empty(), "gate true → host non-empty") && ok;
        }
        else
        {
            ok = expect(entries.empty(), "gate false → host empty") && ok;
        }
    }

    // Residual independence (2855 score ladder residual-ok):
    // gate is distinct from score helpers.
    ok = expect(PlaceholderEntryScore(2, 1, false) == 2, "residual PlaceholderEntryScore greatest n1") && ok;
    ok = expect(PlaceholderEntryScore(2, 4, false) == std::numeric_limits<uint32>::max(),
                "residual PlaceholderEntryScore greatest underflow") &&
         ok;
    ok = expect(PlaceholderEntryScore(2, 1, true) == 9998, "residual PlaceholderEntryScore smallest n1") && ok;

    // Boundary polarity independent of score ladder.
    ok = expect(ShouldGeneratePlaceholderEntries(true, 4, 2), "PRESENTING room must generate via free gate") && ok;
    ok = expect(!ShouldGeneratePlaceholderEntries(true, 4, 4), "PRESENTING full must not generate via free gate") && ok;

    // Host still dual-wires gate then score when admitted.
    const auto greatest = fishingcontest::GeneratePlaceholderEntries(
        FISHING_CONTEST_STATUS::PRESENTING, FISHING_CONTEST_MEASURE::GREATEST, 4, 2);
    ok = expect(greatest.size() == 4, "residual host max-entry loop after gate admit") && ok;
    if (greatest.size() == 4)
    {
        ok = expect(greatest[0].score == PlaceholderEntryScore(2, 1, false),
                    "residual host score dual-wire after gate admit") &&
             ok;
        ok = expect(greatest[0].contestRank == 3, "residual 2645 contestRank pin after gate admit") && ok;
    }

    // Prior dedicated 3380 pin helpers still agree with free dual-wire.
    ok = expect(ShouldGeneratePlaceholderEntries(true, 4, 2) ==
                    pinShouldGeneratePlaceholderEntries3380(true, 4, 2),
                "prior dedicated 3380 pin PRESENTING room") &&
         ok;
    ok = expect(ShouldGeneratePlaceholderEntries(false, 4, 0) ==
                    pinShouldGeneratePlaceholderEntries3380(false, 4, 0),
                "prior dedicated 3380 pin non-presenting") &&
         ok;

    return ok;
}
