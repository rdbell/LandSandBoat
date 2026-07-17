#include "test_treasure_skip_rare_3094.h"

#include "map/treasure_pool_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure ShouldSkipRareCheck 3094 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline addItem skip-rare formula for dual-wire cross-check (slice 3094):
//   !isSoloPool && itemHasNoRareCheck
auto inlineShouldSkipRareCheck(const bool isSoloPool, const bool itemHasNoRareCheck) -> bool
{
    return !isSoloPool && itemHasNoRareCheck;
}

} // namespace

// Pure dual-wire expansion for treasurepoolhelpers::ShouldSkipRareCheck
// (!isSoloPool && itemHasNoRareCheck skip-rare gate; slice 3094). Dense 2².
auto runTreasureSkipRare3094SelfTests() -> bool
{
    using treasurepoolhelpers::ShouldApplyRareMemberCheck;
    using treasurepoolhelpers::ShouldRejectNullItem;
    using treasurepoolhelpers::ShouldRejectNullMember;
    using treasurepoolhelpers::ShouldRejectRareAllHave;
    using treasurepoolhelpers::ShouldSkipRareCheck;

    bool ok = true;

    const struct
    {
        bool        isSoloPool;
        bool        itemHasNoRareCheck;
        bool        want;
        const char* label;
    } cases[] = {
        // Dense 2² dual poles (all combinations of two bools).
        { false, false, false, "party + rare-check item does not skip" },
        { false, true, true, "party + NoRareCheck skips" },
        { true, false, false, "solo + rare-check item does not skip" },
        { true, true, false, "solo + NoRareCheck does not skip" },

        // Residual 1367 pins.
        { false, true, true, "residual party NoRareCheck skip" },
        { true, true, false, "residual solo NoRareCheck no skip" },

        // Boundary re-pins of the skip cell and non-skip neighbors.
        { false, true, true, "party NoRareCheck boundary skip" },
        { false, false, false, "party rare-check boundary no skip" },
        { true, true, false, "solo NoRareCheck boundary no skip" },
        { true, false, false, "solo rare-check boundary no skip" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldSkipRareCheck(c.isSoloPool, c.itemHasNoRareCheck);
        const bool inlineF = inlineShouldSkipRareCheck(c.isSoloPool, c.itemHasNoRareCheck);
        const bool wantPin = !c.isSoloPool && c.itemHasNoRareCheck;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldSkipRareCheck dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldSkipRareCheck == pin formula !solo && noRareCheck") && ok;
    }

    // Pin composition: only non-solo AND NoRareCheck skips.
    ok = expect(!ShouldSkipRareCheck(false, false), "party rare-check item must not skip") && ok;
    ok = expect(ShouldSkipRareCheck(false, true), "party NoRareCheck must skip") && ok;
    ok = expect(!ShouldSkipRareCheck(true, false), "solo rare-check item must not skip") && ok;
    ok = expect(!ShouldSkipRareCheck(true, true), "solo NoRareCheck must not skip") && ok;

    // Dense 2² compose over both bool domains (exactly four cells).
    for (const bool solo : { false, true })
    {
        for (const bool noRare : { false, true })
        {
            const bool got  = ShouldSkipRareCheck(solo, noRare);
            const bool want = !solo && noRare;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldSkipRareCheck(solo, noRare), "compose free == inline") && ok;
        }
    }

    // Explicit dual-wire identity: free == inline == pin formula.
    for (const bool solo : { false, true })
    {
        for (const bool noRare : { false, true })
        {
            const bool freeF   = ShouldSkipRareCheck(solo, noRare);
            const bool inlineF = inlineShouldSkipRareCheck(solo, noRare);
            const bool pin     = !solo && noRare;
            ok                 = expect(freeF == inlineF && freeF == pin, "dual-wire free==inline==pin") && ok;
        }
    }

    // Residual 1367 pins still hold under dual-wire.
    ok = expect(ShouldSkipRareCheck(false, true) && !ShouldSkipRareCheck(true, true), "residual skip rare pins") && ok;

    // Sibling dual-wire independence: 3060 / 3067 remain distinct.
    ok = expect(ShouldRejectNullMember(true, false) && ShouldRejectNullMember(false, true),
                "sibling ShouldRejectNullMember dual-wire polarity") &&
         ok;
    ok = expect(!ShouldRejectNullMember(false, false), "sibling valid member proceeds") && ok;
    ok = expect(ShouldRejectNullItem(true) && !ShouldRejectNullItem(false),
                "sibling ShouldRejectNullItem dual-wire polarity") &&
         ok;

    // Production path semantics (host inject model / addItem rare chain):
    // skipRareCheck := ShouldSkipRareCheck(isSolo, noRareCheck)
    // apply := ShouldApplyRareMemberCheck(itemIsRare, skipRareCheck)
    ok = expect(ShouldSkipRareCheck(false, true), "party NoRareCheck → skip path") && ok;
    ok = expect(!ShouldApplyRareMemberCheck(true, ShouldSkipRareCheck(false, true)),
                "skip suppresses rare member check") &&
         ok;
    ok = expect(!ShouldSkipRareCheck(false, false), "party rare-check → no skip") && ok;
    ok = expect(ShouldApplyRareMemberCheck(true, ShouldSkipRareCheck(false, false)),
                "rare item applies member check when not skipped") &&
         ok;

    // Solo always no-skip regardless of NoRareCheck flag.
    for (const bool noRare : { false, true })
    {
        const bool skip = ShouldSkipRareCheck(true, noRare);
        ok              = expect(!skip, "solo never skips") && ok;
        ok              = expect(ShouldApplyRareMemberCheck(true, skip), "solo rare applies") && ok;
        ok              = expect(!ShouldApplyRareMemberCheck(false, skip), "solo non-rare does not apply") && ok;
    }

    // Dense 2²: free gate polarity matches residual apply-rare composition.
    for (const bool solo : { false, true })
    {
        for (const bool noRare : { false, true })
        {
            const bool skip = ShouldSkipRareCheck(solo, noRare);
            ok              = expect(skip == inlineShouldSkipRareCheck(solo, noRare), "rare-chain free == inline") && ok;
            ok              = expect(skip == (!solo && noRare), "rare-chain free == pin") && ok;

            const bool applyRare = ShouldApplyRareMemberCheck(true, skip);
            ok                   = expect(applyRare == !skip, "rare item apply == !skip") && ok;
            ok                   = expect(!ShouldApplyRareMemberCheck(false, skip), "non-rare never applies") && ok;

            if (applyRare)
            {
                ok = expect(ShouldRejectRareAllHave(true, false), "all-have rejects when apply") && ok;
                ok = expect(!ShouldRejectRareAllHave(true, true), "someone-missing does not reject") && ok;
            }
            else
            {
                ok = expect(!ShouldRejectRareAllHave(false, false), "!apply must not all-have reject") && ok;
            }
        }
    }

    // Residual policy compose still wires skip into apply rare.
    ok = expect(!ShouldApplyRareMemberCheck(true, ShouldSkipRareCheck(false, true)),
                "compose helpers: party NoRareCheck suppresses apply") &&
         ok;
    ok = expect(ShouldApplyRareMemberCheck(true, ShouldSkipRareCheck(false, false)),
                "compose helpers: party rare-check applies") &&
         ok;
    ok = expect(ShouldApplyRareMemberCheck(true, ShouldSkipRareCheck(true, true)),
                "compose helpers: solo NoRareCheck still applies") &&
         ok;

    return ok;
}
