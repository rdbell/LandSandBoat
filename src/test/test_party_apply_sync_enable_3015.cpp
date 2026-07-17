#include "test_party_apply_sync_enable_3015.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party ShouldApplySyncEnableToMember 3015 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CParty::SetSyncTarget enable-path per-member filter for dual-wire
// cross-check (slice 3015):
//   isPC && notDisappear && sameZoneAsDesignee
auto inlineShouldApplySyncEnableToMember(
    const bool isPC,
    const bool notDisappear,
    const bool sameZoneAsDesignee) -> bool
{
    return isPC && notDisappear && sameZoneAsDesignee;
}

} // namespace

// Pure dual-wire expansion for partyhelpers::ShouldApplySyncEnableToMember
// (CParty::SetSyncTarget ENABLE isPC && notDisappear && sameZoneAsDesignee
// gate; slice 3015).
auto runPartyApplySyncEnable3015SelfTests() -> bool
{
    using partyhelpers::ClassifySetSyncTarget;
    using partyhelpers::set_sync_target_gate;
    using partyhelpers::ShouldApplySyncDisableToMember;
    using partyhelpers::ShouldApplySyncEnableToMember;
    using partyhelpers::ShouldApplySyncToMember;

    bool ok = true;

    // Residual 1334 pins still hold under dual-wire.
    ok = expect(ShouldApplySyncEnableToMember(true, true, true), "residual apply PC visible same-zone") && ok;
    ok = expect(!ShouldApplySyncEnableToMember(true, false, true), "residual skip PC disappear same-zone") && ok;
    ok = expect(!ShouldApplySyncEnableToMember(true, true, false), "residual skip PC visible other-zone") && ok;
    ok = expect(!ShouldApplySyncEnableToMember(false, true, true), "residual skip non-PC visible same-zone") && ok;

    const struct
    {
        bool        isPC;
        bool        notDisappear;
        bool        sameZoneAsDesignee;
        bool        want;
        const char* label;
    } cases[] = {
        // Truth table poles — isPC × notDisappear × sameZone.
        { true, true, true, true, "PC visible same-zone applies" },
        { true, true, false, false, "PC visible other-zone skips" },
        { true, false, true, false, "PC disappear same-zone skips" },
        { true, false, false, false, "PC disappear other-zone skips" },
        { false, true, true, false, "non-PC visible same-zone skips" },
        { false, true, false, false, "non-PC visible other-zone skips" },
        { false, false, true, false, "non-PC disappear same-zone skips" },
        { false, false, false, false, "non-PC disappear other-zone skips" },

        // Residual 1334 polarity pins.
        { true, true, true, true, "residual apply all true" },
        { true, false, true, false, "residual skip disappear" },
        { true, true, false, false, "residual skip other zone" },
        { false, true, true, false, "residual skip non-PC" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldApplySyncEnableToMember(c.isPC, c.notDisappear, c.sameZoneAsDesignee);
        const bool inlineF = inlineShouldApplySyncEnableToMember(c.isPC, c.notDisappear, c.sameZoneAsDesignee);
        const bool wantPin = c.isPC && c.notDisappear && c.sameZoneAsDesignee;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldApplySyncEnableToMember dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldApplySyncEnableToMember == pin formula isPC && notDisappear && sameZone") && ok;
    }

    // Pin composition: all three gates required.
    ok = expect(ShouldApplySyncEnableToMember(true, true, true), "PC visible same-zone must apply") && ok;
    ok = expect(!ShouldApplySyncEnableToMember(true, false, true), "PC disappear must skip") && ok;
    ok = expect(!ShouldApplySyncEnableToMember(true, true, false), "PC other-zone must skip") && ok;
    ok = expect(!ShouldApplySyncEnableToMember(false, true, true), "non-PC same-zone must skip") && ok;
    ok = expect(!ShouldApplySyncEnableToMember(false, false, false), "all-false must skip") && ok;

    // Dense compose: full 2×2×2 free == inline == pin.
    for (const bool isPC : { false, true })
    {
        for (const bool notDisappear : { false, true })
        {
            for (const bool sameZone : { false, true })
            {
                const bool got  = ShouldApplySyncEnableToMember(isPC, notDisappear, sameZone);
                const bool want = isPC && notDisappear && sameZone;
                ok              = expect(got == want, "compose free == pin formula") && ok;
                ok              = expect(got == inlineShouldApplySyncEnableToMember(isPC, notDisappear, sameZone),
                            "compose free == inline") &&
                     ok;
            }
        }
    }

    // --- Production CParty::SetSyncTarget ENABLE path semantics ---
    // Host injects:
    //   isPC               = i->objtype == TYPE_PC
    //   notDisappear       = member != nullptr && member->status != DISAPPEAR
    //   sameZoneAsDesignee = member != nullptr && member->getZone() == PChar->getZone()
    // when true  → message / DelStatusEffectsByFlag / AddStatusEffectSilent / CharSync
    // when false → continue (skip member)
    ok = expect(ShouldApplySyncEnableToMember(true, true, true), "SetSyncTarget ENABLE PC visible same-zone → apply path") && ok;
    ok = expect(!ShouldApplySyncEnableToMember(true, false, true), "SetSyncTarget ENABLE PC disappear same-zone → skip path") && ok;
    ok = expect(!ShouldApplySyncEnableToMember(true, true, false), "SetSyncTarget ENABLE PC visible other-zone → skip path") && ok;
    ok = expect(!ShouldApplySyncEnableToMember(false, true, true), "SetSyncTarget ENABLE non-PC visible same-zone → skip path") && ok;
    ok = expect(!ShouldApplySyncEnableToMember(false, false, false), "SetSyncTarget ENABLE all-false → skip path") && ok;

    // Explicit dual-wire: free function is isPC && notDisappear && sameZone of injects.
    for (const bool isPC : { false, true })
    {
        for (const bool notDisappear : { false, true })
        {
            for (const bool sameZone : { false, true })
            {
                const bool want = isPC && notDisappear && sameZone;
                ok              = expect(ShouldApplySyncEnableToMember(isPC, notDisappear, sameZone) == want,
                            "host inject identity") &&
                     ok;
                ok = expect(ShouldApplySyncEnableToMember(isPC, notDisappear, sameZone) ==
                                inlineShouldApplySyncEnableToMember(isPC, notDisappear, sameZone),
                            "host inject dual-wire free == inline") &&
                     ok;
            }
        }
    }

    // Residual sibling SetSyncTarget halves remain independent of the enable
    // filter (1334 suite coexistence; sibling 2999 RefreshSync apply filter).
    ok = expect(ClassifySetSyncTarget(false, true, true, 50, true, false) == set_sync_target_gate::DISABLED,
                "residual ClassifySetSyncTarget disabled pin") &&
         ok;
    ok = expect(ClassifySetSyncTarget(true, true, true, 50, true, false) == set_sync_target_gate::ENABLE,
                "residual ClassifySetSyncTarget enable pin") &&
         ok;
    ok = expect(ShouldApplySyncDisableToMember(true, true), "residual ShouldApplySyncDisableToMember true") && ok;
    ok = expect(!ShouldApplySyncDisableToMember(false, true), "residual ShouldApplySyncDisableToMember non-PC") && ok;
    ok = expect(ShouldApplySyncToMember(true, true), "residual ShouldApplySyncToMember true (sibling 2999)") && ok;
    ok = expect(!ShouldApplySyncToMember(false, true), "residual ShouldApplySyncToMember non-PC (sibling 2999)") && ok;

    return ok;
}
