#include "test_party_level_sync_on_join_3305.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party ShouldApplyPartyLevelSyncOnJoin 3305 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline AddMember post-join level-sync gate for dual-wire cross-check
// (dedicated 3305; residual expand 2955 / prior dedicated 3274):
//   hasSyncTarget
auto inlineShouldApplyPartyLevelSyncOnJoin3305(const bool hasSyncTarget) -> bool
{
    return hasSyncTarget;
}

// Compact dual-wire pin matching Go pinShouldApplyPartyLevelSyncOnJoin3305 /
// C++ capacity positive form (formula unchanged from 1350 / residual 2955 /
// prior dedicated 3274):
//   hasSyncTarget
auto pinShouldApplyPartyLevelSyncOnJoin3305(const bool hasSyncTarget) -> bool
{
    return hasSyncTarget;
}

// Prior dedicated 3274 pin retained for cross-suite residual poles.
auto pinShouldApplyPartyLevelSyncOnJoin3274(const bool hasSyncTarget) -> bool
{
    return hasSyncTarget;
}

auto inlineShouldApplyPartyLevelSyncOnJoin3274(const bool hasSyncTarget) -> bool
{
    return hasSyncTarget;
}

} // namespace

// Pure dual-wire expansion for partyhelpers::ShouldApplyPartyLevelSyncOnJoin
// (hasSyncTarget / m_PSyncTarget != nullptr identity after join;
// OmegaXI internal/party; dedicated slice 3305; residual expand 2955 /
// prior dedicated 3274 / pure 1350).
//
// Coverage:
//   - free == inline == pin positive form
//   - residual 2955 / 1350 / prior 3274 pins still hold
//   - dense 2^1 boolean space free == inline == pin
//   - host inject poles + PC post-process integration
//   - residual independence (3217 seeking clear left alone)
auto runPartyLevelSyncOnJoin3305SelfTests() -> bool
{
    using partyhelpers::ShouldApplyPartyLevelSyncOnJoin;
    using partyhelpers::ShouldClearSeekingParty;
    using partyhelpers::ShouldRunPCAddPostProcess;

    bool ok = true;

    // Residual 1350 / 2955 pins still hold under dedicated dual-wire.
    ok = expect(!ShouldApplyPartyLevelSyncOnJoin(false), "residual: no sync target leaves level-sync unchanged") && ok;
    ok = expect(ShouldApplyPartyLevelSyncOnJoin(true), "residual: sync target applies level-sync on join") && ok;
    ok = expect(!ShouldApplyPartyLevelSyncOnJoin(false) && ShouldApplyPartyLevelSyncOnJoin(true),
                "residual 2955/1350 level-sync-on-join poles") &&
         ok;

    // Prior dedicated 3274 free==inline==pin poles still hold under 3305.
    ok = expect(ShouldApplyPartyLevelSyncOnJoin(true) == pinShouldApplyPartyLevelSyncOnJoin3274(true) &&
                    ShouldApplyPartyLevelSyncOnJoin(false) == pinShouldApplyPartyLevelSyncOnJoin3274(false),
                "prior dedicated 3274 free==pin residual poles") &&
         ok;
    ok = expect(ShouldApplyPartyLevelSyncOnJoin(true) == inlineShouldApplyPartyLevelSyncOnJoin3274(true) &&
                    ShouldApplyPartyLevelSyncOnJoin(false) == inlineShouldApplyPartyLevelSyncOnJoin3274(false),
                "prior dedicated 3274 free==inline residual poles") &&
         ok;

    // --- Core poles: free == inline == pin positive form ---
    const struct
    {
        bool        hasSyncTarget;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { false, false, "no sync target leaves level-sync unchanged" },
        { true, true, "sync target applies level-sync on join" },

        // Residual 1350 / 2955 pins.
        { false, false, "residual ShouldApplyPartyLevelSyncOnJoin(false)" },
        { true, true, "residual ShouldApplyPartyLevelSyncOnJoin(true)" },

        // Prior dedicated 3274 poles restated.
        { false, false, "prior 3274 no sync target" },
        { true, true, "prior 3274 sync target apply" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldApplyPartyLevelSyncOnJoin(c.hasSyncTarget);
        const bool inlineF = inlineShouldApplyPartyLevelSyncOnJoin3305(c.hasSyncTarget);
        const bool pin     = pinShouldApplyPartyLevelSyncOnJoin3305(c.hasSyncTarget);
        // Positive form pin composition (identity of hasSyncTarget).
        const bool wantPin = c.hasSyncTarget;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF && got == pin && got == wantPin,
                    "ShouldApplyPartyLevelSyncOnJoin free == inline == pin positive form") &&
             ok;
        // Cross-suite: free == prior 3274 inline/pin as well.
        ok = expect(got == inlineShouldApplyPartyLevelSyncOnJoin3274(c.hasSyncTarget) &&
                        got == pinShouldApplyPartyLevelSyncOnJoin3274(c.hasSyncTarget),
                    "free == prior 3274 inline/pin") &&
             ok;
    }

    // Free == pin across residual poles.
    ok = expect(ShouldApplyPartyLevelSyncOnJoin(true) == pinShouldApplyPartyLevelSyncOnJoin3305(true),
                "free == pin sync target apply") &&
         ok;
    ok = expect(ShouldApplyPartyLevelSyncOnJoin(false) == pinShouldApplyPartyLevelSyncOnJoin3305(false),
                "free == pin no sync target") &&
         ok;

    // Dense compose: full 2^1 boolean space free == inline == pin.
    for (const bool hasSyncTarget : { false, true })
    {
        const bool got     = ShouldApplyPartyLevelSyncOnJoin(hasSyncTarget);
        const bool inlineF = inlineShouldApplyPartyLevelSyncOnJoin3305(hasSyncTarget);
        const bool pin     = pinShouldApplyPartyLevelSyncOnJoin3305(hasSyncTarget);
        const bool want    = hasSyncTarget;
        ok                 = expect(got == want, "compose free == pin formula") && ok;
        ok                 = expect(got == inlineF && got == pin, "compose free == inline == pin") && ok;
        ok                 = expect(got == inlineShouldApplyPartyLevelSyncOnJoin3274(hasSyncTarget) &&
                            got == pinShouldApplyPartyLevelSyncOnJoin3274(hasSyncTarget),
                        "compose free == prior 3274 inline/pin") &&
             ok;
    }

    // --- Production CParty::AddMember PC post-process path semantics ---
    // Host injects:
    //   hasSyncTarget = m_PSyncTarget != nullptr
    // when true  → host may apply LevelSync (same-zone message / status / CharSync)
    // when false → leave level-sync state unchanged for the joiner
    ok = expect(ShouldApplyPartyLevelSyncOnJoin(true) && pinShouldApplyPartyLevelSyncOnJoin3305(true),
                "AddMember sync target → apply level-sync path free/pin dual-wire") &&
         ok;
    ok = expect(!ShouldApplyPartyLevelSyncOnJoin(false), "AddMember no sync target → leave level-sync unchanged") && ok;

    // Host inject poles + free == inline == pin identity.
    const struct
    {
        bool        hasSyncTarget;
        const char* label;
    } hostPoles[] = {
        { true, "sync target → apply level-sync on join" },
        { false, "no sync target → leave level-sync unchanged" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got     = ShouldApplyPartyLevelSyncOnJoin(p.hasSyncTarget);
        const bool inlineF = inlineShouldApplyPartyLevelSyncOnJoin3305(p.hasSyncTarget);
        const bool pin     = pinShouldApplyPartyLevelSyncOnJoin3305(p.hasSyncTarget);
        const bool want    = p.hasSyncTarget;
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF && got == pin, "host inject free == inline == pin") && ok;
    }

    // PC post-process outer gate is residual; free function is identity.
    ok = expect(ShouldRunPCAddPostProcess(true), "PC party post-process residual") && ok;
    ok = expect(!ShouldRunPCAddPostProcess(false), "mob party skips PC post-process residual") && ok;
    for (const bool hasSync : { false, true })
    {
        ok = expect(ShouldRunPCAddPostProcess(true), "PC post-process before level-sync inject") && ok;
        ok = expect(ShouldApplyPartyLevelSyncOnJoin(hasSync) == hasSync, "PC post-process inject identity") && ok;
        ok = expect(ShouldApplyPartyLevelSyncOnJoin(hasSync) == inlineShouldApplyPartyLevelSyncOnJoin3305(hasSync) &&
                        ShouldApplyPartyLevelSyncOnJoin(hasSync) == pinShouldApplyPartyLevelSyncOnJoin3305(hasSync),
                    "PC post-process inject free == inline == pin") &&
             ok;
    }

    // Residual independence (1350 / 2955 / 3217 / prior 3274):
    // level-sync-on-join is orthogonal to seeking-clear dual-wire identity.
    // Sibling ShouldClearSeekingParty remains dual-wired under 3217 (left alone).
    ok = expect(ShouldClearSeekingParty(true), "sibling residual seeking-clear true") && ok;
    ok = expect(!ShouldClearSeekingParty(false), "sibling residual seeking-clear false") && ok;
    for (const bool seeking : { false, true })
    {
        for (const bool hasSync : { false, true })
        {
            ok = expect(ShouldClearSeekingParty(seeking) == seeking, "seeking vs sync compose seeking") && ok;
            ok = expect(ShouldApplyPartyLevelSyncOnJoin(hasSync) == hasSync, "seeking vs sync compose hasSync") && ok;
            ok = expect(ShouldApplyPartyLevelSyncOnJoin(hasSync) == inlineShouldApplyPartyLevelSyncOnJoin3305(hasSync) &&
                            ShouldApplyPartyLevelSyncOnJoin(hasSync) == pinShouldApplyPartyLevelSyncOnJoin3305(hasSync),
                        "seeking vs sync dual-wire free == inline == pin") &&
                 ok;
        }
    }

    return ok;
}
