#include "test_party_clear_seeking_2955.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party ShouldClearSeekingParty 2955 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline AddMember post-join seeking clear for dual-wire cross-check (slice 2955):
//   isSeekingParty
auto inlineShouldClearSeekingParty(const bool isSeekingParty) -> bool
{
    return isSeekingParty;
}

} // namespace

// Pure dual-wire expansion for partyhelpers::ShouldClearSeekingParty
// (isSeekingParty after join; slice 2955).
auto runPartyClearSeeking2955SelfTests() -> bool
{
    using partyhelpers::ShouldApplyPartyLevelSyncOnJoin;
    using partyhelpers::ShouldClearSeekingParty;
    using partyhelpers::ShouldRunPCAddPostProcess;

    bool ok = true;

    const struct
    {
        bool        isSeekingParty;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { false, false, "not seeking leaves flag" },
        { true, true, "seeking clears InviteFlg" },

        // Residual 1350 pins.
        { false, false, "residual ShouldClearSeekingParty(false)" },
        { true, true, "residual ShouldClearSeekingParty(true)" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldClearSeekingParty(c.isSeekingParty);
        const bool inlineF = inlineShouldClearSeekingParty(c.isSeekingParty);
        const bool wantPin = c.isSeekingParty;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldClearSeekingParty dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldClearSeekingParty == pin formula isSeekingParty") && ok;
    }

    // Pin composition: identity only.
    ok = expect(!ShouldClearSeekingParty(false), "false must not clear") && ok;
    ok = expect(ShouldClearSeekingParty(true), "true must clear") && ok;

    // Dense compose: full 2^1 boolean space.
    for (const bool isSeekingParty : { false, true })
    {
        const bool got  = ShouldClearSeekingParty(isSeekingParty);
        const bool want = isSeekingParty;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldClearSeekingParty(isSeekingParty), "compose free == inline") && ok;
    }

    // --- Production CParty::AddMember PC post-process path semantics ---
    // Host injects:
    //   isSeekingParty = PChar->isSeekingParty()  // InviteFlg / LFP
    // when true  → InviteFlg = false; updatemask |= UPDATE_HP; SaveCharStats
    // when false → leave seeking flag unchanged
    ok = expect(!ShouldClearSeekingParty(false), "AddMember not seeking → leave InviteFlg") && ok;
    ok = expect(ShouldClearSeekingParty(true), "AddMember seeking → clear InviteFlg path") && ok;

    // PC post-process outer gate is residual; seeking free function is identity.
    ok = expect(ShouldRunPCAddPostProcess(true), "PC party post-process residual") && ok;
    ok = expect(!ShouldRunPCAddPostProcess(false), "mob party skips PC post-process residual") && ok;
    for (const bool seeking : { false, true })
    {
        ok = expect(ShouldRunPCAddPostProcess(true), "PC post-process before seeking inject") && ok;
        ok = expect(ShouldClearSeekingParty(seeking) == seeking, "PC post-process inject identity") && ok;
        ok = expect(ShouldClearSeekingParty(seeking) == inlineShouldClearSeekingParty(seeking),
                    "PC post-process inject free == inline") &&
             ok;
    }

    // Sibling post-join level-sync gate remains residual identity.
    ok = expect(ShouldApplyPartyLevelSyncOnJoin(true), "residual level-sync-on-join true") && ok;
    ok = expect(!ShouldApplyPartyLevelSyncOnJoin(false), "residual level-sync-on-join false") && ok;
    for (const bool seeking : { false, true })
    {
        for (const bool hasSync : { false, true })
        {
            ok = expect(ShouldClearSeekingParty(seeking) == seeking, "seeking vs sync compose seeking") && ok;
            ok = expect(ShouldApplyPartyLevelSyncOnJoin(hasSync) == hasSync, "seeking vs sync compose hasSync") && ok;
        }
    }

    // Explicit dual-wire: free == isSeekingParty inject for host-style poles.
    for (const bool isSeekingParty : { false, true })
    {
        ok = expect(ShouldClearSeekingParty(isSeekingParty) == isSeekingParty, "host inject dual-wire identity") && ok;
        ok = expect(ShouldClearSeekingParty(isSeekingParty) == inlineShouldClearSeekingParty(isSeekingParty),
                    "host inject free == inline") &&
             ok;
    }

    // Residual 1350 poles still hold under dual-wire.
    ok = expect(!ShouldClearSeekingParty(false), "residual not seeking") && ok;
    ok = expect(ShouldClearSeekingParty(true), "residual seeking") && ok;

    return ok;
}
