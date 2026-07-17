#include "test_party_apply_sync_member_2999.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party ShouldApplySyncToMember 2999 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CParty::RefreshSync per-member filter for dual-wire cross-check
// (slice 2999):
//   isPC && sameZoneAsSyncTarget
auto inlineShouldApplySyncToMember(const bool isPC, const bool sameZoneAsSyncTarget) -> bool
{
    return isPC && sameZoneAsSyncTarget;
}

} // namespace

// Pure dual-wire expansion for partyhelpers::ShouldApplySyncToMember
// (CParty::RefreshSync isPC && sameZoneAsSyncTarget gate; slice 2999).
auto runPartyApplySyncMember2999SelfTests() -> bool
{
    using partyhelpers::ResolveSyncMemberLevel;
    using partyhelpers::ShouldApplySyncToMember;
    using partyhelpers::ShouldRemoveSyncForLowLevel;

    bool ok = true;

    // Residual 1330 pins still hold under dual-wire.
    ok = expect(ShouldApplySyncToMember(true, true), "residual apply PC same-zone") && ok;
    ok = expect(!ShouldApplySyncToMember(true, false), "residual skip PC other-zone") && ok;
    ok = expect(!ShouldApplySyncToMember(false, true), "residual skip non-PC same-zone") && ok;

    const struct
    {
        bool        isPC;
        bool        sameZoneAsSyncTarget;
        bool        want;
        const char* label;
    } cases[] = {
        // Truth table poles — isPC × sameZone.
        { true, true, true, "PC same-zone applies" },
        { true, false, false, "PC other-zone skips" },
        { false, true, false, "non-PC same-zone skips" },
        { false, false, false, "non-PC other-zone skips" },

        // Residual 1330 polarity pins.
        { true, true, true, "residual apply both true" },
        { true, false, false, "residual skip other zone" },
        { false, true, false, "residual skip non-PC" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldApplySyncToMember(c.isPC, c.sameZoneAsSyncTarget);
        const bool inlineF = inlineShouldApplySyncToMember(c.isPC, c.sameZoneAsSyncTarget);
        const bool wantPin = c.isPC && c.sameZoneAsSyncTarget;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldApplySyncToMember dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldApplySyncToMember == pin formula isPC && sameZone") && ok;
    }

    // Pin composition: both TYPE_PC and same zone required.
    ok = expect(ShouldApplySyncToMember(true, true), "PC same-zone must apply") && ok;
    ok = expect(!ShouldApplySyncToMember(true, false), "PC other-zone must skip") && ok;
    ok = expect(!ShouldApplySyncToMember(false, true), "non-PC same-zone must skip") && ok;
    ok = expect(!ShouldApplySyncToMember(false, false), "non-PC other-zone must skip") && ok;

    // Dense compose: full 2×2 free == inline == pin.
    for (const bool isPC : { false, true })
    {
        for (const bool sameZone : { false, true })
        {
            const bool got  = ShouldApplySyncToMember(isPC, sameZone);
            const bool want = isPC && sameZone;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldApplySyncToMember(isPC, sameZone),
                        "compose free == inline") &&
                 ok;
        }
    }

    // --- Production CParty::RefreshSync path semantics ---
    // Host injects:
    //   isPC                = i->objtype == TYPE_PC
    //   sameZoneAsSyncTarget = i->getZone() == sync->getZone()
    // when true  → ResolveSyncMemberLevel / status-effect power / SetMLevel
    // when false → continue (skip member)
    ok = expect(ShouldApplySyncToMember(true, true), "RefreshSync PC same-zone → apply path") && ok;
    ok = expect(!ShouldApplySyncToMember(true, false), "RefreshSync PC other-zone → skip path") && ok;
    ok = expect(!ShouldApplySyncToMember(false, true), "RefreshSync non-PC same-zone → skip path") && ok;
    ok = expect(!ShouldApplySyncToMember(false, false), "RefreshSync non-PC other-zone → skip path") && ok;

    // Explicit dual-wire: free function is isPC && sameZone of injects.
    for (const bool isPC : { false, true })
    {
        for (const bool sameZone : { false, true })
        {
            const bool want = isPC && sameZone;
            ok              = expect(ShouldApplySyncToMember(isPC, sameZone) == want,
                        "host inject identity") &&
                 ok;
            ok = expect(ShouldApplySyncToMember(isPC, sameZone) ==
                            inlineShouldApplySyncToMember(isPC, sameZone),
                        "host inject dual-wire free == inline") &&
                 ok;
        }
    }

    // Residual sibling RefreshSync halves remain independent of the apply
    // filter (1330 suite coexistence; sibling 2974 low-level remove).
    ok = expect(ResolveSyncMemberLevel(30, 50) == 30, "residual ResolveSyncMemberLevel sync < member") && ok;
    ok = expect(ResolveSyncMemberLevel(50, 30) == 30, "residual ResolveSyncMemberLevel member < sync") && ok;
    ok = expect(ShouldRemoveSyncForLowLevel(0), "residual ShouldRemoveSyncForLowLevel remove at 0") && ok;
    ok = expect(!ShouldRemoveSyncForLowLevel(10), "residual ShouldRemoveSyncForLowLevel keep at 10") && ok;

    return ok;
}
