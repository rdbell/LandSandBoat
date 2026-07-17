#include "test_party_include_group_effects_3036.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party ShouldIncludeInGroupEffects 3036 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CParty::PushEffectsPacket partyInfo_t filter for dual-wire cross-check
// (slice 3036) using ordered short-circuits:
//   infoPartyID != partyID → false
//   infoCharID == selfCharID → false
//   !charFound → false
//   !sameZone → false
//   else true
auto inlineShouldIncludeInGroupEffects(
    const uint32 infoPartyID,
    const uint32 partyID,
    const uint32 infoCharID,
    const uint32 selfCharID,
    const bool   charFound,
    const bool   sameZone) -> bool
{
    if (infoPartyID != partyID)
    {
        return false;
    }
    if (infoCharID == selfCharID)
    {
        return false;
    }
    if (!charFound)
    {
        return false;
    }
    if (!sameZone)
    {
        return false;
    }
    return true;
}

// Boolean-compose pin of the PushEffectsPacket partyInfo_t filter (slice 3036).
auto pinShouldIncludeInGroupEffects(
    const uint32 infoPartyID,
    const uint32 partyID,
    const uint32 infoCharID,
    const uint32 selfCharID,
    const bool   charFound,
    const bool   sameZone) -> bool
{
    return infoPartyID == partyID && infoCharID != selfCharID && charFound && sameZone;
}

} // namespace

// Pure dual-wire expansion for partyhelpers::ShouldIncludeInGroupEffects
// (CParty::PushEffectsPacket partyInfo_t sameZoneMembers filter; slice 3036).
auto runPartyIncludeGroupEffects3036SelfTests() -> bool
{
    using partyhelpers::ShouldIncludeInGroupEffects;
    using partyhelpers::ShouldPushEffectsPacket;
    using partyhelpers::ShouldPushPartyPacketToMember;

    bool ok = true;

    // Residual 1336 pins still hold under dual-wire.
    ok = expect(ShouldIncludeInGroupEffects(10, 10, 2, 1, true, true), "residual include") && ok;
    ok = expect(!ShouldIncludeInGroupEffects(11, 10, 2, 1, true, true), "residual other party") && ok;
    ok = expect(!ShouldIncludeInGroupEffects(10, 10, 1, 1, true, true), "residual self") && ok;
    ok = expect(!ShouldIncludeInGroupEffects(10, 10, 2, 1, false, true), "residual not found") && ok;
    ok = expect(!ShouldIncludeInGroupEffects(10, 10, 2, 1, true, false), "residual other zone") && ok;

    const struct
    {
        uint32      infoPartyID;
        uint32      partyID;
        uint32      infoCharID;
        uint32      selfCharID;
        bool        charFound;
        bool        sameZone;
        bool        want;
        const char* label;
    } cases[] = {
        // Happy paths.
        { 10, 10, 2, 1, true, true, true, "same party not-self found same-zone" },
        { 99, 99, 5, 4, true, true, true, "alt IDs include" },
        { 0, 0, 2, 1, true, true, true, "party id 0 match include" },
        { 1, 1, 0, 1, true, true, true, "info char id 0 not self include" },

        // Ordered short-circuits.
        { 11, 10, 2, 1, true, true, false, "infoPartyID != partyID short-circuit" },
        { 10, 10, 1, 1, true, true, false, "infoCharID == selfCharID short-circuit" },
        { 10, 10, 2, 1, false, true, false, "!charFound short-circuit" },
        { 10, 10, 2, 1, true, false, false, "!sameZone short-circuit" },

        // Residual 1336 polarity pins.
        { 10, 10, 2, 1, true, true, true, "residual include table" },
        { 11, 10, 2, 1, true, true, false, "residual other party table" },
        { 10, 10, 1, 1, true, true, false, "residual self table" },
        { 10, 10, 2, 1, false, true, false, "residual not found table" },
        { 10, 10, 2, 1, true, false, false, "residual other zone table" },

        // Multi-gate fail still false.
        { 11, 10, 1, 1, false, false, false, "all-reject poles" },
        { 11, 10, 2, 1, true, true, false, "other party rejects even found same-zone" },
        { 10, 10, 1, 1, true, true, false, "self rejects even found same-zone" },
        { 10, 10, 2, 1, false, true, false, "not found rejects even same-zone flag" },
        { 10, 10, 2, 1, true, false, false, "other zone rejects even found" },
        { 10, 10, 2, 1, false, false, false, "not found and other zone" },

        // ID edges.
        { 10, 10, 2, 3, true, true, true, "different self id include" },
        { 10, 10, 0xffffffff, 1, true, true, true, "max info char id include" },
        { 10, 10, 1, 0xffffffff, true, true, true, "max self char id include" },
        { 0xffffffff, 0xffffffff, 2, 1, true, true, true, "max party id match include" },
        { 0xffffffff, 0xfffffffe, 2, 1, true, true, false, "near-max party id miss" },
    };

    for (const auto& c : cases)
    {
        const bool got = ShouldIncludeInGroupEffects(
            c.infoPartyID, c.partyID, c.infoCharID, c.selfCharID, c.charFound, c.sameZone);
        const bool inlineF = inlineShouldIncludeInGroupEffects(
            c.infoPartyID, c.partyID, c.infoCharID, c.selfCharID, c.charFound, c.sameZone);
        const bool wantPin = pinShouldIncludeInGroupEffects(
            c.infoPartyID, c.partyID, c.infoCharID, c.selfCharID, c.charFound, c.sameZone);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldIncludeInGroupEffects dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldIncludeInGroupEffects == pin boolean compose") && ok;
    }

    // Pin composition: every short-circuit is required independently.
    ok = expect(ShouldIncludeInGroupEffects(10, 10, 2, 1, true, true),
                "happy same-party not-self found same-zone must include") &&
         ok;
    ok = expect(!ShouldIncludeInGroupEffects(11, 10, 2, 1, true, true), "other party must skip") && ok;
    ok = expect(!ShouldIncludeInGroupEffects(10, 10, 1, 1, true, true), "self must skip") && ok;
    ok = expect(!ShouldIncludeInGroupEffects(10, 10, 2, 1, false, true), "not found must skip") && ok;
    ok = expect(!ShouldIncludeInGroupEffects(10, 10, 2, 1, true, false), "other zone must skip") && ok;

    // Dense compose: bool poles × representative party/char ID samples.
    // free == inline ordered short-circuit == boolean pin compose.
    const uint32 infoPartyIDs[] = { 10, 11 };
    const uint32 partyIDs[]     = { 10 };
    const uint32 infoCharIDs[]  = { 1, 2 };
    const uint32 selfCharIDs[]  = { 1 };
    for (const uint32 infoPartyID : infoPartyIDs)
    {
        for (const uint32 partyID : partyIDs)
        {
            for (const uint32 infoCharID : infoCharIDs)
            {
                for (const uint32 selfCharID : selfCharIDs)
                {
                    for (const bool charFound : { false, true })
                    {
                        for (const bool sameZone : { false, true })
                        {
                            const bool got = ShouldIncludeInGroupEffects(
                                infoPartyID, partyID, infoCharID, selfCharID, charFound, sameZone);
                            const bool want = pinShouldIncludeInGroupEffects(
                                infoPartyID, partyID, infoCharID, selfCharID, charFound, sameZone);
                            ok = expect(got == want, "compose free == pin formula") && ok;
                            ok = expect(got == inlineShouldIncludeInGroupEffects(
                                                   infoPartyID, partyID, infoCharID, selfCharID, charFound, sameZone),
                                        "compose free == inline") &&
                                 ok;
                        }
                    }
                }
            }
        }
    }

    // --- Production CParty::PushEffectsPacket path semantics ---
    // Host injects:
    //   infoPartyID = memberinfo.partyid
    //   partyID     = m_PartyID
    //   infoCharID  = memberinfo.id
    //   selfCharID  = PMemberChar->id
    //   charFound   = zoneutils::GetChar(memberinfo.id) != nullptr
    //   sameZone    = charFound && GetChar->getZone() == PMemberChar->getZone()
    // when true  → sameZoneMembers.push_back(PPartyMember)
    // when false → continue (skip info row)
    // Host GetPartyInfo / GetChar / zone compare / GROUP_EFFECTS packet sit outside.
    ok = expect(ShouldIncludeInGroupEffects(10, 10, 2, 1, true, true),
                "PushEffects same party not-self found same-zone → include path") &&
         ok;
    ok = expect(!ShouldIncludeInGroupEffects(11, 10, 2, 1, true, true),
                "PushEffects other party → skip path") &&
         ok;
    ok = expect(!ShouldIncludeInGroupEffects(10, 10, 1, 1, true, true),
                "PushEffects self → skip path") &&
         ok;
    ok = expect(!ShouldIncludeInGroupEffects(10, 10, 2, 1, false, true),
                "PushEffects not found → skip path") &&
         ok;
    ok = expect(!ShouldIncludeInGroupEffects(10, 10, 2, 1, true, false),
                "PushEffects other zone → skip path") &&
         ok;

    // Explicit dual-wire: free == ordered short-circuit == pin for host poles.
    const struct
    {
        uint32      infoPartyID;
        uint32      partyID;
        uint32      infoCharID;
        uint32      selfCharID;
        bool        charFound;
        bool        sameZone;
        const char* label;
    } hostPoles[] = {
        { 10, 10, 2, 1, true, true, "include path" },
        { 11, 10, 2, 1, true, true, "other party skip" },
        { 10, 10, 1, 1, true, true, "self skip" },
        { 10, 10, 2, 1, false, true, "not found skip" },
        { 10, 10, 2, 1, true, false, "other zone skip" },
        { 10, 10, 2, 1, false, false, "not found other zone skip" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got = ShouldIncludeInGroupEffects(
            p.infoPartyID, p.partyID, p.infoCharID, p.selfCharID, p.charFound, p.sameZone);
        const bool inlineF = inlineShouldIncludeInGroupEffects(
            p.infoPartyID, p.partyID, p.infoCharID, p.selfCharID, p.charFound, p.sameZone);
        const bool want = pinShouldIncludeInGroupEffects(
            p.infoPartyID, p.partyID, p.infoCharID, p.selfCharID, p.charFound, p.sameZone);
        ok = expect(got == want, p.label) && ok;
        ok = expect(got == inlineF, "host inject dual-wire free == inline") && ok;
    }

    // Residual sibling PushEffects gate + PushPacket filter remain independent
    // of the group-effects partyInfo_t filter (1335 / 1336 / 3031 coexistence).
    ok = expect(ShouldPushEffectsPacket(true), "residual ShouldPushEffectsPacket true") && ok;
    ok = expect(!ShouldPushEffectsPacket(false), "residual ShouldPushEffectsPacket false") && ok;
    ok = expect(ShouldPushPartyPacketToMember(true, 2, 1, true, false, 0, 230),
                "residual ShouldPushPartyPacketToMember all-zones") &&
         ok;
    ok = expect(!ShouldPushPartyPacketToMember(true, 1, 1, true, false, 0, 230),
                "residual ShouldPushPartyPacketToMember sender") &&
         ok;

    return ok;
}
