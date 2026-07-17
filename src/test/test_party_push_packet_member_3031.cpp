#include "test_party_push_packet_member_3031.h"

#include "map/party_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party ShouldPushPartyPacketToMember 3031 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline CParty::PushPacket per-member filter for dual-wire cross-check
// (slice 3031) using ordered short-circuits:
//   !isPC → false
//   memberID == senderID → false
//   !notDisappear → false
//   inPrison → false
//   zoneIDFilter != 0 && memberZoneID != zoneIDFilter → false
//   else true
auto inlineShouldPushPartyPacketToMember(
    const bool   isPC,
    const uint32 memberID,
    const uint32 senderID,
    const bool   notDisappear,
    const bool   inPrison,
    const uint16 zoneIDFilter,
    const uint16 memberZoneID) -> bool
{
    if (!isPC)
    {
        return false;
    }
    if (memberID == senderID)
    {
        return false;
    }
    if (!notDisappear)
    {
        return false;
    }
    if (inPrison)
    {
        return false;
    }
    if (zoneIDFilter != 0 && memberZoneID != zoneIDFilter)
    {
        return false;
    }
    return true;
}

// Boolean-compose pin of the PushPacket per-member filter (slice 3031).
auto pinShouldPushPartyPacketToMember(
    const bool   isPC,
    const uint32 memberID,
    const uint32 senderID,
    const bool   notDisappear,
    const bool   inPrison,
    const uint16 zoneIDFilter,
    const uint16 memberZoneID) -> bool
{
    return isPC && memberID != senderID && notDisappear && !inPrison &&
           (zoneIDFilter == 0 || memberZoneID == zoneIDFilter);
}

} // namespace

// Pure dual-wire expansion for partyhelpers::ShouldPushPartyPacketToMember
// (CParty::PushPacket per-member filter; slice 3031).
auto runPartyPushPacketMember3031SelfTests() -> bool
{
    using partyhelpers::ShouldIncludeInGroupEffects;
    using partyhelpers::ShouldPushEffectsPacket;
    using partyhelpers::ShouldPushPartyPacketToMember;

    bool ok = true;

    // Residual 1335 pins still hold under dual-wire.
    ok = expect(ShouldPushPartyPacketToMember(true, 2, 1, true, false, 0, 230), "residual all zones") && ok;
    ok = expect(ShouldPushPartyPacketToMember(true, 2, 1, true, false, 230, 230), "residual zone match") && ok;
    ok = expect(!ShouldPushPartyPacketToMember(true, 2, 1, true, false, 230, 231), "residual zone miss") && ok;
    ok = expect(!ShouldPushPartyPacketToMember(true, 1, 1, true, false, 0, 230), "residual sender") && ok;
    ok = expect(!ShouldPushPartyPacketToMember(false, 2, 1, true, false, 0, 230), "residual non-pc") && ok;
    ok = expect(!ShouldPushPartyPacketToMember(true, 2, 1, false, false, 0, 230), "residual disappear") && ok;
    ok = expect(!ShouldPushPartyPacketToMember(true, 2, 1, true, true, 0, 230), "residual prison") && ok;

    const struct
    {
        bool        isPC;
        uint32      memberID;
        uint32      senderID;
        bool        notDisappear;
        bool        inPrison;
        uint16      zoneIDFilter;
        uint16      memberZoneID;
        bool        want;
        const char* label;
    } cases[] = {
        // Happy paths.
        { true, 2, 1, true, false, 0, 230, true, "PC not-sender visible free all-zones" },
        { true, 2, 1, true, false, 230, 230, true, "PC not-sender visible free zone-match" },
        { true, 99, 1, true, false, 0, 0, true, "member zone 0 with filter 0" },
        { true, 99, 1, true, false, 100, 100, true, "zone filter match non-zero" },

        // Ordered short-circuits.
        { false, 2, 1, true, false, 0, 230, false, "!isPC short-circuit" },
        { true, 1, 1, true, false, 0, 230, false, "memberID == senderID short-circuit" },
        { true, 2, 1, false, false, 0, 230, false, "!notDisappear short-circuit" },
        { true, 2, 1, true, true, 0, 230, false, "inPrison short-circuit" },
        { true, 2, 1, true, false, 230, 231, false, "zone filter miss short-circuit" },

        // Residual 1335 polarity pins.
        { true, 2, 1, true, false, 0, 230, true, "residual all zones table" },
        { true, 2, 1, true, false, 230, 230, true, "residual zone match table" },
        { true, 2, 1, true, false, 230, 231, false, "residual zone miss table" },
        { true, 1, 1, true, false, 0, 230, false, "residual sender table" },
        { false, 2, 1, true, false, 0, 230, false, "residual non-pc table" },
        { true, 2, 1, false, false, 0, 230, false, "residual disappear table" },
        { true, 2, 1, true, true, 0, 230, false, "residual prison table" },

        // Multi-gate fail still false.
        { false, 1, 1, false, true, 230, 231, false, "all-reject poles" },
        { true, 1, 1, false, true, 230, 231, false, "sender+disappear+prison+zone miss" },
        { true, 2, 1, true, true, 230, 230, false, "prison rejects even zone match" },
        { true, 2, 1, false, false, 230, 230, false, "disappear rejects even zone match" },
        { false, 2, 1, true, false, 230, 230, false, "non-PC rejects even zone match" },
        { true, 2, 2, true, false, 230, 230, false, "sender rejects even zone match" },

        // Zone filter 0 ignores member zone.
        { true, 2, 1, true, false, 0, 1, true, "filter 0 member zone 1" },
        { true, 2, 1, true, false, 0, 65535, true, "filter 0 member zone max" },
        { true, 2, 1, true, false, 1, 0, false, "filter 1 member zone 0 miss" },
    };

    for (const auto& c : cases)
    {
        const bool got = ShouldPushPartyPacketToMember(
            c.isPC, c.memberID, c.senderID, c.notDisappear, c.inPrison, c.zoneIDFilter, c.memberZoneID);
        const bool inlineF = inlineShouldPushPartyPacketToMember(
            c.isPC, c.memberID, c.senderID, c.notDisappear, c.inPrison, c.zoneIDFilter, c.memberZoneID);
        const bool wantPin = pinShouldPushPartyPacketToMember(
            c.isPC, c.memberID, c.senderID, c.notDisappear, c.inPrison, c.zoneIDFilter, c.memberZoneID);

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldPushPartyPacketToMember dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldPushPartyPacketToMember == pin boolean compose") && ok;
    }

    // Pin composition: every short-circuit is required independently.
    ok = expect(ShouldPushPartyPacketToMember(true, 2, 1, true, false, 0, 230), "happy all-zones must push") && ok;
    ok = expect(!ShouldPushPartyPacketToMember(false, 2, 1, true, false, 0, 230), "non-PC must skip") && ok;
    ok = expect(!ShouldPushPartyPacketToMember(true, 1, 1, true, false, 0, 230), "sender must skip") && ok;
    ok = expect(!ShouldPushPartyPacketToMember(true, 2, 1, false, false, 0, 230), "disappear must skip") && ok;
    ok = expect(!ShouldPushPartyPacketToMember(true, 2, 1, true, true, 0, 230), "prison must skip") && ok;
    ok = expect(!ShouldPushPartyPacketToMember(true, 2, 1, true, false, 230, 231), "zone miss must skip") && ok;
    ok = expect(ShouldPushPartyPacketToMember(true, 2, 1, true, false, 230, 230), "zone match must push") && ok;

    // Dense compose: bool poles × representative ID/zone samples.
    // free == inline ordered short-circuit == boolean pin compose.
    const uint32 memberIDs[]  = { 1, 2 };
    const uint32 senderIDs[]  = { 1, 2 };
    const uint16 zoneFilters[] = { 0, 230 };
    const uint16 memberZones[] = { 230, 231 };
    for (const bool isPC : { false, true })
    {
        for (const uint32 memberID : memberIDs)
        {
            for (const uint32 senderID : senderIDs)
            {
                for (const bool notDisappear : { false, true })
                {
                    for (const bool inPrison : { false, true })
                    {
                        for (const uint16 zoneIDFilter : zoneFilters)
                        {
                            for (const uint16 memberZoneID : memberZones)
                            {
                                const bool got = ShouldPushPartyPacketToMember(
                                    isPC, memberID, senderID, notDisappear, inPrison, zoneIDFilter, memberZoneID);
                                const bool want = pinShouldPushPartyPacketToMember(
                                    isPC, memberID, senderID, notDisappear, inPrison, zoneIDFilter, memberZoneID);
                                ok = expect(got == want, "compose free == pin formula") && ok;
                                ok = expect(got == inlineShouldPushPartyPacketToMember(
                                                       isPC, memberID, senderID, notDisappear, inPrison, zoneIDFilter, memberZoneID),
                                            "compose free == inline") &&
                                     ok;
                            }
                        }
                    }
                }
            }
        }
    }

    // --- Production CParty::PushPacket path semantics ---
    // Host injects:
    //   isPC         = i->objtype == TYPE_PC
    //   memberID     = member->id
    //   senderID     = PushPacket senderID arg
    //   notDisappear = member->status != DISAPPEAR
    //   inPrison     = jailutils::InPrison(member)
    //   zoneIDFilter = PushPacket ZoneID arg
    //   memberZoneID = member->getZone()
    // when true  → member->pushPacket(packet->copy())
    // when false → continue (skip member)
    // Host null-member continue sits outside this free function.
    ok = expect(ShouldPushPartyPacketToMember(true, 2, 1, true, false, 0, 230),
                "PushPacket PC other visible free all-zones → push path") &&
         ok;
    ok = expect(ShouldPushPartyPacketToMember(true, 2, 1, true, false, 230, 230),
                "PushPacket zone match → push path") &&
         ok;
    ok = expect(!ShouldPushPartyPacketToMember(true, 2, 1, true, false, 230, 231),
                "PushPacket zone miss → skip path") &&
         ok;
    ok = expect(!ShouldPushPartyPacketToMember(true, 1, 1, true, false, 0, 230),
                "PushPacket sender → skip path") &&
         ok;
    ok = expect(!ShouldPushPartyPacketToMember(false, 2, 1, true, false, 0, 230),
                "PushPacket non-PC → skip path") &&
         ok;
    ok = expect(!ShouldPushPartyPacketToMember(true, 2, 1, false, false, 0, 230),
                "PushPacket disappear → skip path") &&
         ok;
    ok = expect(!ShouldPushPartyPacketToMember(true, 2, 1, true, true, 0, 230),
                "PushPacket prison → skip path") &&
         ok;

    // Explicit dual-wire: free == ordered short-circuit == pin for host poles.
    const struct
    {
        bool        isPC;
        uint32      memberID;
        uint32      senderID;
        bool        notDisappear;
        bool        inPrison;
        uint16      zoneIDFilter;
        uint16      memberZoneID;
        const char* label;
    } hostPoles[] = {
        { true, 2, 1, true, false, 0, 230, "all zones push" },
        { true, 2, 1, true, false, 230, 230, "zone match push" },
        { true, 2, 1, true, false, 230, 231, "zone miss skip" },
        { true, 1, 1, true, false, 0, 230, "sender skip" },
        { false, 2, 1, true, false, 0, 230, "non-PC skip" },
        { true, 2, 1, false, false, 0, 230, "disappear skip" },
        { true, 2, 1, true, true, 0, 230, "prison skip" },
    };
    for (const auto& p : hostPoles)
    {
        const bool got = ShouldPushPartyPacketToMember(
            p.isPC, p.memberID, p.senderID, p.notDisappear, p.inPrison, p.zoneIDFilter, p.memberZoneID);
        const bool inlineF = inlineShouldPushPartyPacketToMember(
            p.isPC, p.memberID, p.senderID, p.notDisappear, p.inPrison, p.zoneIDFilter, p.memberZoneID);
        const bool want = pinShouldPushPartyPacketToMember(
            p.isPC, p.memberID, p.senderID, p.notDisappear, p.inPrison, p.zoneIDFilter, p.memberZoneID);
        ok = expect(got == want, p.label) && ok;
        ok = expect(got == inlineF, "host inject dual-wire free == inline") && ok;
    }

    // Residual sibling PushEffectsPacket / group-effects halves remain
    // independent of the PushPacket member filter (1335 / 1336 coexistence).
    ok = expect(ShouldPushEffectsPacket(true), "residual ShouldPushEffectsPacket true") && ok;
    ok = expect(!ShouldPushEffectsPacket(false), "residual ShouldPushEffectsPacket false") && ok;
    ok = expect(ShouldIncludeInGroupEffects(10, 10, 2, 1, true, true),
                "residual ShouldIncludeInGroupEffects include") &&
         ok;
    ok = expect(!ShouldIncludeInGroupEffects(11, 10, 2, 1, true, true),
                "residual ShouldIncludeInGroupEffects other-party") &&
         ok;
    ok = expect(!ShouldIncludeInGroupEffects(10, 10, 1, 1, true, true),
                "residual ShouldIncludeInGroupEffects self") &&
         ok;

    return ok;
}
