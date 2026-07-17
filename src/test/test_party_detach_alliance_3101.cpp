#include "test_party_detach_alliance_3101.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party ShouldDetachAllianceOnDisband 3101 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CParty::DisbandParty alliance detach gate for dual-wire
// cross-check (slice 3101):
//   hasAlliance
auto inlineShouldDetachAllianceOnDisband(const bool hasAlliance) -> bool
{
    return hasAlliance;
}

} // namespace

// Pure dual-wire expansion for partyhelpers::ShouldDetachAllianceOnDisband
// (DisbandParty alliance detach gate; slice 3101).
auto runPartyDetachAlliance3101SelfTests() -> bool
{
    using path = partyhelpers::disband_party_member_path;
    using partyhelpers::ClassifyDisbandPartyMemberPath;
    using partyhelpers::MsgLevelSyncRemoveLeftParty;
    using partyhelpers::ShouldDetachAllianceOnDisband;
    using partyhelpers::ShouldNotifyPartyDisbandIPC;
    using partyhelpers::ShouldReplaceSoloTreasurePool;

    bool ok = true;

    // Residual 1345 pins still hold under dual-wire.
    ok = expect(ShouldDetachAllianceOnDisband(true), "residual has alliance detaches") && ok;
    ok = expect(!ShouldDetachAllianceOnDisband(false), "residual no alliance skips detach") && ok;

    const struct
    {
        bool        hasAlliance;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic dual poles.
        { false, false, "no alliance skips removeParty" },
        { true, true, "has alliance calls removeParty" },

        // Residual 1345 polarity pins.
        { false, false, "residual ShouldDetachAllianceOnDisband(false)" },
        { true, true, "residual ShouldDetachAllianceOnDisband(true)" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldDetachAllianceOnDisband(c.hasAlliance);
        const bool inlineF = inlineShouldDetachAllianceOnDisband(c.hasAlliance);
        const bool wantPin = c.hasAlliance;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldDetachAllianceOnDisband dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldDetachAllianceOnDisband == pin formula hasAlliance") && ok;
    }

    // Pin composition: identity only.
    ok = expect(!ShouldDetachAllianceOnDisband(false), "false must not detach") && ok;
    ok = expect(ShouldDetachAllianceOnDisband(true), "true must detach") && ok;

    // Dense compose: full 2^1 boolean space.
    // free == inline identity == pin formula hasAlliance.
    for (const bool hasAlliance : { false, true })
    {
        const bool got  = ShouldDetachAllianceOnDisband(hasAlliance);
        const bool want = hasAlliance;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldDetachAllianceOnDisband(hasAlliance),
                    "compose free == inline") &&
             ok;
    }

    // --- Production CParty::DisbandParty path semantics ---
    // Host injects:
    //   hasAlliance = m_PAlliance != nullptr
    // when true  → m_PAlliance->removeParty(this); then clear m_PAlliance
    // when false → skip removeParty; still clear m_PAlliance (already null)
    // Host runs this before ClassifyDisbandPartyMemberPath / PC_FULL body.
    ok = expect(ShouldDetachAllianceOnDisband(true), "DisbandParty has alliance → removeParty path") && ok;
    ok = expect(!ShouldDetachAllianceOnDisband(false), "DisbandParty no alliance → skip removeParty") && ok;

    // Explicit dual-wire: free == identity inline == pin for host poles.
    const struct
    {
        bool        hasAlliance;
        const char* label;
    } hostPoles[] = {
        { true, "alliance removeParty path" },
        { false, "solo party skip removeParty path" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got     = ShouldDetachAllianceOnDisband(p.hasAlliance);
        const bool inlineF = inlineShouldDetachAllianceOnDisband(p.hasAlliance);
        const bool want    = p.hasAlliance;
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF, "host inject dual-wire free == inline") && ok;
    }

    // Residual ClassifyDisbandPartyMemberPath coexistence (1345 suite).
    ok = expect(ClassifyDisbandPartyMemberPath(true, false) == path::PC_FULL, "residual Classify PC") && ok;
    ok = expect(ClassifyDisbandPartyMemberPath(false, true) == path::MOB_CLEAR, "residual Classify MOB") && ok;
    ok = expect(ClassifyDisbandPartyMemberPath(false, false) == path::NONE, "residual Classify NONE") && ok;

    // Host compose: detach gate first, then classify member path.
    for (const bool hasAlliance : { false, true })
    {
        ok = expect(ShouldDetachAllianceOnDisband(hasAlliance) == hasAlliance, "detach gate polarity") && ok;
        ok = expect(ShouldDetachAllianceOnDisband(hasAlliance) == inlineShouldDetachAllianceOnDisband(hasAlliance),
                    "detach gate dual-wire") &&
             ok;
        ok = expect(ClassifyDisbandPartyMemberPath(true, false) == path::PC_FULL, "PC path after detach inject") && ok;
        ok = expect(ClassifyDisbandPartyMemberPath(false, true) == path::MOB_CLEAR, "MOB path after detach inject") && ok;
    }

    // Residual PC-path gates still hold under dual-wire.
    ok = expect(ShouldNotifyPartyDisbandIPC(true), "residual IPC notify true") && ok;
    ok = expect(!ShouldNotifyPartyDisbandIPC(false), "residual IPC notify false") && ok;
    ok = expect(ShouldReplaceSoloTreasurePool(true, false), "residual replace solo") && ok;
    ok = expect(!ShouldReplaceSoloTreasurePool(true, true), "residual keep zone pool") && ok;
    ok = expect(!ShouldReplaceSoloTreasurePool(false, false), "residual no pool") && ok;
    ok = expect(MsgLevelSyncRemoveLeftParty == 553, "residual MsgStd 553") && ok;

    // Compose paths with gate polarity (mirror Go Test...Compose3101).
    const struct
    {
        bool        isPC;
        bool        isMob;
        path        want;
        const char* label;
    } paths[] = {
        { true, false, path::PC_FULL, "pc full" },
        { false, true, path::MOB_CLEAR, "mob clear" },
        { false, false, path::NONE, "none" },
        { true, true, path::PC_FULL, "pc over mob" },
    };
    for (const auto& p : paths)
    {
        for (const bool hasAlliance : { false, true })
        {
            ok = expect(ShouldDetachAllianceOnDisband(hasAlliance) == hasAlliance, "path detach gate") && ok;
        }
        ok = expect(ClassifyDisbandPartyMemberPath(p.isPC, p.isMob) == p.want, p.label) && ok;
    }

    return ok;
}
