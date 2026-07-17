#include "test_party_clear_seeking_party_3217.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party ShouldClearSeekingParty 3217 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline AddMember post-join seeking clear for dual-wire cross-check
// (dedicated 3217):
//   isSeekingParty
auto inlineShouldClearSeekingParty3217(const bool isSeekingParty) -> bool
{
    return isSeekingParty;
}

// Compact dual-wire pin matching Go pinShouldClearSeekingParty3217 / C++ capacity
// positive form (formula unchanged from 1350 / 2955):
//   isSeekingParty
auto pinShouldClearSeekingParty3217(const bool isSeekingParty) -> bool
{
    return isSeekingParty;
}

} // namespace

// Pure dual-wire expansion for partyhelpers::ShouldClearSeekingParty
// (isSeekingParty identity after join;
// OmegaXI internal/party; dedicated slice 3217; residual expand 2955 /
// pure 1350).
//
// Coverage:
//   - free == inline == pin positive form
//   - residual 2955 / 1350 pins still hold
//   - dense 2^1 boolean space free == inline == pin
//   - host inject poles + PC post-process integration
//   - residual independence (3200 full gate left alone; level-sync residual)
auto runPartyClearSeekingParty3217SelfTests() -> bool
{
    using partyhelpers::ShouldApplyPartyLevelSyncOnJoin;
    using partyhelpers::ShouldClearSeekingParty;
    using partyhelpers::ShouldRejectPCAddFull;
    using partyhelpers::ShouldRunPCAddPostProcess;

    bool ok = true;

    // Residual 1350 / 2955 pins still hold under dedicated dual-wire.
    ok = expect(!ShouldClearSeekingParty(false), "residual: not seeking leaves flag") && ok;
    ok = expect(ShouldClearSeekingParty(true), "residual: seeking clears InviteFlg") && ok;
    ok = expect(!ShouldClearSeekingParty(false) && ShouldClearSeekingParty(true),
                "residual 2955/1350 seeking clear poles") &&
         ok;

    // --- Core poles: free == inline == pin positive form ---
    const struct
    {
        bool        isSeekingParty;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { false, false, "not seeking leaves flag" },
        { true, true, "seeking clears InviteFlg" },

        // Residual 1350 / 2955 pins.
        { false, false, "residual ShouldClearSeekingParty(false)" },
        { true, true, "residual ShouldClearSeekingParty(true)" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldClearSeekingParty(c.isSeekingParty);
        const bool inlineF = inlineShouldClearSeekingParty3217(c.isSeekingParty);
        const bool pin     = pinShouldClearSeekingParty3217(c.isSeekingParty);
        // Positive form pin composition (identity of isSeekingParty).
        const bool wantPin = c.isSeekingParty;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin && got == wantPin,
                    "ShouldClearSeekingParty free == inline == pin positive form") &&
             ok;
    }

    // Free == pin across residual poles.
    ok = expect(ShouldClearSeekingParty(true) == pinShouldClearSeekingParty3217(true),
                "free == pin seeking clear") &&
         ok;
    ok = expect(ShouldClearSeekingParty(false) == pinShouldClearSeekingParty3217(false),
                "free == pin not seeking") &&
         ok;

    // Dense compose: full 2^1 boolean space free == inline == pin.
    for (const bool isSeekingParty : { false, true })
    {
        const bool got     = ShouldClearSeekingParty(isSeekingParty);
        const bool inlineF = inlineShouldClearSeekingParty3217(isSeekingParty);
        const bool pin     = pinShouldClearSeekingParty3217(isSeekingParty);
        const bool want    = isSeekingParty;
        ok                 = expect(got == want, "compose free == pin formula") && ok;
        ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
    }

    // --- Production CParty::AddMember PC post-process path semantics ---
    // Host injects:
    //   isSeekingParty = PChar->isSeekingParty()  // InviteFlg / LFP
    // when true  → InviteFlg = false; updatemask |= UPDATE_HP; SaveCharStats
    // when false → leave seeking flag unchanged
    ok = expect(ShouldClearSeekingParty(true) && pinShouldClearSeekingParty3217(true),
                "AddMember seeking → clear InviteFlg path free/pin dual-wire") &&
         ok;
    ok = expect(!ShouldClearSeekingParty(false), "AddMember not seeking → leave InviteFlg") && ok;

    // Host inject poles + free == inline == pin identity.
    const struct
    {
        bool        isSeekingParty;
        const char* label;
    } hostPoles[] = {
        { true, "seeking → clear InviteFlg" },
        { false, "not seeking → leave InviteFlg" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got     = ShouldClearSeekingParty(p.isSeekingParty);
        const bool inlineF = inlineShouldClearSeekingParty3217(p.isSeekingParty);
        const bool pin     = pinShouldClearSeekingParty3217(p.isSeekingParty);
        const bool want    = p.isSeekingParty;
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
    }

    // PC post-process outer gate is residual; free function is identity.
    ok = expect(ShouldRunPCAddPostProcess(true), "PC party post-process residual") && ok;
    ok = expect(!ShouldRunPCAddPostProcess(false), "mob party skips PC post-process residual") && ok;
    for (const bool seeking : { false, true })
    {
        ok = expect(ShouldRunPCAddPostProcess(true), "PC post-process before seeking inject") && ok;
        ok = expect(ShouldClearSeekingParty(seeking) == seeking, "PC post-process inject identity") && ok;
        ok = expect(ShouldClearSeekingParty(seeking) == inlineShouldClearSeekingParty3217(seeking) &&
                        ShouldClearSeekingParty(seeking) == pinShouldClearSeekingParty3217(seeking),
                    "PC post-process inject free == inline == pin") &&
             ok;
    }

    // Residual independence (1350 / 2955 / 3200):
    // seeking clear is orthogonal to level-sync-on-join residual identity.
    // Sibling ShouldRejectPCAddFull remains dual-wired under 3200 (left alone).
    ok = expect(ShouldApplyPartyLevelSyncOnJoin(true), "residual level-sync-on-join true") && ok;
    ok = expect(!ShouldApplyPartyLevelSyncOnJoin(false), "residual level-sync-on-join false") && ok;
    ok = expect(ShouldRejectPCAddFull(true, true, true), "sibling 3200 residual: full gate still rejects") && ok;
    ok = expect(!ShouldRejectPCAddFull(true, true, false), "sibling 3200 residual: not full still allows") && ok;
    for (const bool seeking : { false, true })
    {
        for (const bool hasSync : { false, true })
        {
            ok = expect(ShouldClearSeekingParty(seeking) == seeking, "seeking vs sync compose seeking") && ok;
            ok = expect(ShouldApplyPartyLevelSyncOnJoin(hasSync) == hasSync, "seeking vs sync compose hasSync") && ok;
        }
    }

    return ok;
}
