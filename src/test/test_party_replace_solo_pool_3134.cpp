#include "test_party_replace_solo_pool_3134.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party ShouldReplaceSoloTreasurePool 3134 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CParty::DisbandParty Solo treasure-pool replace gate for dual-wire
// cross-check (slice 3134):
//   hasTreasurePool && !isZonePool
auto inlineShouldReplaceSoloTreasurePool(const bool hasTreasurePool, const bool isZonePool) -> bool
{
    return hasTreasurePool && !isZonePool;
}

} // namespace

// Pure dual-wire expansion for partyhelpers::ShouldReplaceSoloTreasurePool
// (DisbandParty Solo treasure-pool replace gate; slice 3134).
auto runPartyReplaceSoloPool3134SelfTests() -> bool
{
    using path = partyhelpers::disband_party_member_path;
    using partyhelpers::ClassifyDisbandPartyMemberPath;
    using partyhelpers::MsgLevelSyncRemoveLeftParty;
    using partyhelpers::ShouldDetachAllianceOnDisband;
    using partyhelpers::ShouldNotifyPartyDisbandIPC;
    using partyhelpers::ShouldReplaceSoloTreasurePool;
    using partyhelpers::ShouldStartSyncDisableCountdown;

    bool ok = true;

    // Residual 1345 pins still hold under dual-wire.
    ok = expect(ShouldReplaceSoloTreasurePool(true, false), "residual replace solo") && ok;
    ok = expect(!ShouldReplaceSoloTreasurePool(true, true), "residual keep zone pool") && ok;
    ok = expect(!ShouldReplaceSoloTreasurePool(false, false), "residual no pool") && ok;

    const struct
    {
        bool        hasTreasurePool;
        bool        isZonePool;
        bool        want;
        const char* label;
    } cases[] = {
        // Truth table poles — hasTreasurePool × isZonePool (dense 2^2).
        { true, false, true, "non-null non-zone pool replaces with Solo" },
        { true, true, false, "non-null zone pool kept" },
        { false, false, false, "missing pool skips replace" },
        { false, true, false, "missing pool zone flag ignored" },

        // Residual 1345 polarity pins.
        { true, false, true, "residual replace solo" },
        { true, true, false, "residual keep zone" },
        { false, false, false, "residual no pool" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldReplaceSoloTreasurePool(c.hasTreasurePool, c.isZonePool);
        const bool inlineF = inlineShouldReplaceSoloTreasurePool(c.hasTreasurePool, c.isZonePool);
        const bool wantPin = c.hasTreasurePool && !c.isZonePool;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldReplaceSoloTreasurePool dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldReplaceSoloTreasurePool == pin formula hasTreasurePool && !isZonePool") && ok;
    }

    // Pin composition: pool present and not Zone required.
    ok = expect(ShouldReplaceSoloTreasurePool(true, false), "non-zone pool must replace") && ok;
    ok = expect(!ShouldReplaceSoloTreasurePool(true, true), "zone pool must keep") && ok;
    ok = expect(!ShouldReplaceSoloTreasurePool(false, false), "missing pool must skip") && ok;
    ok = expect(!ShouldReplaceSoloTreasurePool(false, true), "missing pool zone pole must skip") && ok;

    // Dense compose: full 2^2 boolean space.
    // free == inline == pin formula hasTreasurePool && !isZonePool.
    for (const bool hasTreasurePool : { false, true })
    {
        for (const bool isZonePool : { false, true })
        {
            const bool got  = ShouldReplaceSoloTreasurePool(hasTreasurePool, isZonePool);
            const bool want = hasTreasurePool && !isZonePool;
            ok              = expect(got == want, "compose free == pin formula") && ok;
            ok              = expect(got == inlineShouldReplaceSoloTreasurePool(hasTreasurePool, isZonePool),
                        "compose free == inline") &&
                 ok;
        }
    }

    // --- Production CParty::DisbandParty path semantics ---
    // Host injects:
    //   hasTreasurePool = PChar->PTreasurePool != nullptr
    //   isZonePool      = pool != nullptr && getPoolType() == TreasurePoolType::Zone
    // when true  → delMember, new Solo pool, addMember, updatePool
    // when false → keep existing pool (null or Zone)
    // Host only evaluates this gate on PC_FULL member loop.
    ok = expect(ShouldReplaceSoloTreasurePool(true, false), "DisbandParty solo/party pool → replace Solo") && ok;
    ok = expect(!ShouldReplaceSoloTreasurePool(true, true), "DisbandParty zone pool → keep") && ok;
    ok = expect(!ShouldReplaceSoloTreasurePool(false, false), "DisbandParty null pool → skip") && ok;

    // Explicit dual-wire: free == pin inline for host poles.
    const struct
    {
        bool        hasTreasurePool;
        bool        isZonePool;
        const char* label;
    } hostPoles[] = {
        { true, false, "non-zone pool Solo replace path" },
        { true, true, "zone pool keep path" },
        { false, false, "null pool skip path" },
        { false, true, "null pool zone flag skip path" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got     = ShouldReplaceSoloTreasurePool(p.hasTreasurePool, p.isZonePool);
        const bool inlineF = inlineShouldReplaceSoloTreasurePool(p.hasTreasurePool, p.isZonePool);
        const bool want    = p.hasTreasurePool && !p.isZonePool;
        ok                 = expect(got == want, p.label) && ok;
        ok                 = expect(got == inlineF, "host inject dual-wire free == inline") && ok;
    }

    // Residual ClassifyDisbandPartyMemberPath coexistence (1345 suite).
    ok = expect(ClassifyDisbandPartyMemberPath(true, false) == path::PC_FULL, "residual Classify PC") && ok;
    ok = expect(ClassifyDisbandPartyMemberPath(false, true) == path::MOB_CLEAR, "residual Classify MOB") && ok;
    ok = expect(ClassifyDisbandPartyMemberPath(false, false) == path::NONE, "residual Classify NONE") && ok;

    // Host compose: classify member path first; treasure only on PC_FULL.
    for (const bool hasTreasurePool : { false, true })
    {
        for (const bool isZonePool : { false, true })
        {
            const bool want = hasTreasurePool && !isZonePool;
            ok              = expect(ShouldReplaceSoloTreasurePool(hasTreasurePool, isZonePool) == want, "treasure gate polarity") && ok;
            ok              = expect(ShouldReplaceSoloTreasurePool(hasTreasurePool, isZonePool) ==
                            inlineShouldReplaceSoloTreasurePool(hasTreasurePool, isZonePool),
                        "treasure gate dual-wire") &&
                 ok;
            ok = expect(ClassifyDisbandPartyMemberPath(true, false) == path::PC_FULL, "PC path before treasure inject") && ok;
            ok = expect(ClassifyDisbandPartyMemberPath(false, true) == path::MOB_CLEAR, "MOB path no treasure host") && ok;
        }
    }

    // Residual alliance detach / IPC / MsgStd still hold under dual-wire.
    ok = expect(ShouldDetachAllianceOnDisband(true), "residual alliance detach true") && ok;
    ok = expect(!ShouldDetachAllianceOnDisband(false), "residual alliance detach false") && ok;
    ok = expect(ShouldNotifyPartyDisbandIPC(true), "residual IPC notify true") && ok;
    ok = expect(!ShouldNotifyPartyDisbandIPC(false), "residual IPC notify false") && ok;
    ok = expect(MsgLevelSyncRemoveLeftParty == 553, "residual MsgStd 553") && ok;

    // Compose paths with gate polarity (mirror Go Test...Compose3134).
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
        for (const bool hasTreasurePool : { false, true })
        {
            for (const bool isZonePool : { false, true })
            {
                const bool want = hasTreasurePool && !isZonePool;
                ok              = expect(ShouldReplaceSoloTreasurePool(hasTreasurePool, isZonePool) == want, "path treasure gate") && ok;
            }
        }
        ok = expect(ClassifyDisbandPartyMemberPath(p.isPC, p.isMob) == p.want, p.label) && ok;
        if (p.want == path::PC_FULL)
        {
            ok = expect(ShouldReplaceSoloTreasurePool(true, false), "PC_FULL non-zone Solo replace") && ok;
            ok = expect(!ShouldReplaceSoloTreasurePool(true, true), "PC_FULL zone keep") && ok;
            ok = expect(!ShouldReplaceSoloTreasurePool(false, false), "PC_FULL null skip") && ok;
        }
    }

    // Sync countdown residual reused by DisbandParty PC path (3016 surface).
    ok = expect(ShouldStartSyncDisableCountdown(true, true), "residual sync countdown infinite") && ok;
    ok = expect(!ShouldStartSyncDisableCountdown(true, false), "residual sync countdown timed") && ok;

    return ok;
}
