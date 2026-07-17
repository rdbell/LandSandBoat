#include "test_salvage_claim_transport_3085.h"

#include "map/salvage_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "salvage claim transport 3085 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline Lua onTransportUpdate claim gate for dual-wire cross-check:
//   instance:getLocalVar('transportUser') == 0
auto inlineCanClaimTransport(const uint32 transportUserID) -> bool
{
    return transportUserID == 0;
}

// Inline busy half (else return):
//   transportUser != 0
auto inlineTransportUserBusy(const uint32 transportUserID) -> bool
{
    return transportUserID != 0;
}

// Compact dual-wire pin matching Go pinCanClaimTransport3085:
//   !TransportUserBusy(transportUserID)
auto pinCanClaimTransport(const uint32 transportUserID) -> bool
{
    return !salvagehelpers::TransportUserBusy(transportUserID);
}

} // namespace

// Pure dual-wire expansion for salvagehelpers::CanClaimTransport
// (Lua onTransportUpdate transportUser gate; OmegaXI internal/salvage;
// slice 3085).
//
// Coverage:
//   - id == 0 → claim true
//   - non-zero → claim false / busy true
//   - max uint32 busy
//   - free == inline == pin == !TransportUserBusy
//   - residual 0977 / 1083 / 2871 pins still hold
auto runSalvageClaimTransport3085SelfTests() -> bool
{
    using salvagehelpers::CanClaimTransport;
    using salvagehelpers::TransportUserBusy;

    bool ok = true;

    // Residual 0977 / 1083 / 2871 pins still hold under dual-wire.
    ok = expect(!TransportUserBusy(0), "residual: transportUser 0 free") && ok;
    ok = expect(TransportUserBusy(1), "residual: transportUser 1 busy") && ok;
    ok = expect(CanClaimTransport(0), "residual: transportUser 0 may claim") && ok;
    ok = expect(!CanClaimTransport(42), "residual: transportUser 42 cannot claim") && ok;

    // --- id == 0 → claim true ---
    ok = expect(CanClaimTransport(0), "id==0 free claim") && ok;
    ok = expect(!TransportUserBusy(0), "id==0 not busy") && ok;

    // --- non-zero → claim false ---
    ok = expect(!CanClaimTransport(1), "id 1 busy cannot claim") && ok;
    ok = expect(!CanClaimTransport(2), "id 2 busy cannot claim") && ok;
    ok = expect(!CanClaimTransport(42), "id 42 busy cannot claim") && ok;
    ok = expect(!CanClaimTransport(100), "id 100 busy cannot claim") && ok;

    // --- large / max values ---
    ok = expect(!CanClaimTransport(0x7FFFFFFF), "id int32-max busy") && ok;
    ok = expect(!CanClaimTransport(0x80000000), "id high-bit busy") && ok;
    ok = expect(!CanClaimTransport(0xFFFFFFFE), "id max-1 busy") && ok;
    ok = expect(!CanClaimTransport(0xFFFFFFFF), "id max busy") && ok;
    ok = expect(TransportUserBusy(0xFFFFFFFF), "id max is busy") && ok;

    // --- Composition table: free == inline == pin == !TransportUserBusy ---
    const struct
    {
        uint32      id;
        bool        wantClaim;
        bool        wantBusy;
        const char* label;
    } cases[] = {
        { 0, true, false, "table id 0 free claim" },
        { 1, false, true, "table id 1 busy" },
        { 2, false, true, "table id 2 busy" },
        { 42, false, true, "table id 42 busy" },
        { 100, false, true, "table id 100 busy" },
        { 0x7FFFFFFF, false, true, "table id int32-max busy" },
        { 0x80000000, false, true, "table id high-bit busy" },
        { 0xFFFFFFFE, false, true, "table id max-1 busy" },
        { 0xFFFFFFFF, false, true, "table id max busy" },
    };

    for (const auto& c : cases)
    {
        const bool gotClaim   = CanClaimTransport(c.id);
        const bool gotBusy    = TransportUserBusy(c.id);
        const bool inlineC    = inlineCanClaimTransport(c.id);
        const bool inlineB    = inlineTransportUserBusy(c.id);
        const bool pinGot     = pinCanClaimTransport(c.id);
        const bool invertBusy = !TransportUserBusy(c.id);

        ok = expect(gotClaim == c.wantClaim, c.label) && ok;
        ok = expect(gotBusy == c.wantBusy, c.label) && ok;
        ok = expect(gotClaim == inlineC, "dual-wire free==inline") && ok;
        ok = expect(gotClaim == pinGot, "dual-wire free==pin") && ok;
        ok = expect(gotClaim == invertBusy, "formula free==!TransportUserBusy") && ok;
        ok = expect(gotBusy == inlineB, "dual-wire busy free==inline") && ok;
        ok = expect(gotClaim != gotBusy, "claim and busy are inverse") && ok;
    }

    // --- Production onTransportUpdate path semantics ---
    // Free → may continue claim writeback / stageComplete / timer / deSpawn.
    // Busy → early return.
    ok = expect(CanClaimTransport(0), "onTransportUpdate free → continue path") && ok;
    ok = expect(!CanClaimTransport(1), "onTransportUpdate busy → early return") && ok;
    ok = expect(!CanClaimTransport(42), "onTransportUpdate busy 42 → early return") && ok;
    ok = expect(!CanClaimTransport(0xFFFFFFFF), "onTransportUpdate max busy → early return") && ok;

    // Dense compose identity over representative IDs.
    const uint32 composeIds[] = {
        0, 1, 2, 3, 42, 100, 255, 1000, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFE, 0xFFFFFFFF,
    };
    for (const uint32 id : composeIds)
    {
        const bool got = CanClaimTransport(id);
        ok             = expect(got == (id == 0), "compose id==0") && ok;
        ok             = expect(got == !TransportUserBusy(id), "compose !busy") && ok;
        ok             = expect(got == inlineCanClaimTransport(id), "compose inline") && ok;
        ok             = expect(got == pinCanClaimTransport(id), "compose pin") && ok;
    }

    return ok;
}
