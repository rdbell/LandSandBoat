#include "test_salvage_claim_transport_3777.h"

#include "map/salvage_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "salvage claim transport 3777 self-test failed: " << label << '\n';
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

// Compact dual-wire pin matching Go pinCanClaimTransport3777:
//   !TransportUserBusy(transportUserID)
auto pinCanClaimTransport(const uint32 transportUserID) -> bool
{
    return !salvagehelpers::TransportUserBusy(transportUserID);
}

// Prior dedicated 3732 pin (same formula; residual suite identity).
auto pinCanClaimTransport3732(const uint32 transportUserID) -> bool
{
    return !salvagehelpers::TransportUserBusy(transportUserID);
}

} // namespace

// Pure dual-wire expansion for salvagehelpers::CanClaimTransport
// (Lua onTransportUpdate transportUser gate; OmegaXI internal/salvage;
// slice 3777 dedicated dual-wire expand residual 2871).
//
// Coverage:
//   - id == 0 → claim true (positive pin form)
//   - non-zero → claim false / busy true
//   - max uint32 busy
//   - free == inline == pin == pin3732 == !TransportUserBusy
//   - dense poles: free / busy residual + large / max
//   - residual 0977 / 1083 / 2871 / 3085 / 3259 / 3290 / 3318 / 3495 / 3552 / 3597 / 3642 / 3687 / 3732 pins still hold
//   - prior 3732 independence: free == pin3732 across poles
auto runSalvageClaimTransport3777SelfTests() -> bool
{
    using salvagehelpers::CanClaimTransport;
    using salvagehelpers::TransportUserBusy;

    bool ok = true;

    // Residual 0977 / 1083 / 2871 / 3085 / 3259 / 3290 / 3318 / 3495 / 3552 / 3597 / 3642 / 3687 / 3732 pins still hold under dual-wire.
    ok = expect(!TransportUserBusy(0), "residual: transportUser 0 free") && ok;
    ok = expect(TransportUserBusy(1), "residual: transportUser 1 busy") && ok;
    ok = expect(CanClaimTransport(0), "residual: transportUser 0 may claim") && ok;
    ok = expect(!CanClaimTransport(42), "residual: transportUser 42 cannot claim") && ok;
    ok = expect(CanClaimTransport(0), "residual 2871: free id 0 claim pin") && ok;
    ok = expect(!CanClaimTransport(0xFFFFFFFF), "residual 2871: max id busy cannot claim") && ok;
    ok = expect(CanClaimTransport(0) == pinCanClaimTransport(0),
                "residual prior: free == pin for id 0") &&
         ok;
    ok = expect(CanClaimTransport(0xFFFFFFFF) == pinCanClaimTransport(0xFFFFFFFF),
                "residual prior: free == pin for max busy") &&
         ok;
    ok = expect(CanClaimTransport(0) == pinCanClaimTransport3732(0),
                "residual 3732: free == pin for id 0") &&
         ok;
    ok = expect(CanClaimTransport(0xFFFFFFFF) == pinCanClaimTransport3732(0xFFFFFFFF),
                "residual 3732: free == pin for max busy") &&
         ok;

    // --- Eligible claim path (positive pin form: free id 0) ---
    ok = expect(CanClaimTransport(0), "eligible id 0 free should claim") && ok;
    ok = expect(pinCanClaimTransport(0), "eligible pin id 0 free should claim") && ok;
    ok = expect(pinCanClaimTransport3732(0), "eligible prior pin3732 id 0 free should claim") && ok;
    ok = expect(inlineCanClaimTransport(0), "eligible inline id 0 free should claim") && ok;
    ok = expect(!TransportUserBusy(0), "eligible id 0 must not be busy") && ok;
    ok = expect(CanClaimTransport(0) == inlineCanClaimTransport(0),
                "eligible free == inline id 0") &&
         ok;
    ok = expect(CanClaimTransport(0) == pinCanClaimTransport(0),
                "eligible free == pin id 0") &&
         ok;
    ok = expect(CanClaimTransport(0) == pinCanClaimTransport3732(0),
                "eligible free == pin3732 id 0") &&
         ok;

    // --- Blocked paths (busy residual poles) ---
    ok = expect(!CanClaimTransport(1), "id 1 busy should block claim") && ok;
    ok = expect(!CanClaimTransport(2), "id 2 busy should block claim") && ok;
    ok = expect(!CanClaimTransport(3), "id 3 busy should block claim") && ok;
    ok = expect(!CanClaimTransport(42), "id 42 busy should block claim") && ok;
    ok = expect(!CanClaimTransport(99), "id 99 busy should block claim") && ok;
    ok = expect(!CanClaimTransport(100), "id 100 busy should block claim") && ok;
    ok = expect(!CanClaimTransport(255), "id 255 busy should block claim") && ok;
    ok = expect(!CanClaimTransport(1000), "id 1000 busy should block claim") && ok;
    ok = expect(!CanClaimTransport(0x7FFFFFFF), "id int32-max busy should block claim") && ok;
    ok = expect(!CanClaimTransport(0x80000000), "id high-bit busy should block claim") && ok;
    ok = expect(!CanClaimTransport(0xFFFFFFFE), "id max-1 busy should block claim") && ok;
    ok = expect(!CanClaimTransport(0xFFFFFFFF), "id max busy should block claim") && ok;
    ok = expect(!pinCanClaimTransport(0xFFFFFFFF), "pin max busy should not claim") && ok;
    ok = expect(!pinCanClaimTransport3732(0xFFFFFFFF), "prior pin3732 max busy should not claim") && ok;
    ok = expect(!inlineCanClaimTransport(0xFFFFFFFF), "inline max busy should not claim") && ok;
    ok = expect(TransportUserBusy(0xFFFFFFFF), "id max is busy") && ok;

    // --- Composition table: free == inline == pin == pin3732 == !TransportUserBusy ---
    // Dense poles: free / busy residual + large / max.
    const struct
    {
        uint32      id;
        bool        wantClaim;
        bool        wantBusy;
        const char* label;
    } cases[] = {
        { 0, true, false, "table free id 0 claim" },
        { 1, false, true, "table busy id 1" },
        { 2, false, true, "table busy id 2" },
        { 3, false, true, "table busy id 3" },
        { 42, false, true, "table busy id 42" },
        { 99, false, true, "table busy id 99" },
        { 100, false, true, "table busy id 100" },
        { 255, false, true, "table busy id 255" },
        { 1000, false, true, "table busy id 1000" },
        { 0x7FFFFFFF, false, true, "table busy id int32-max" },
        { 0x80000000, false, true, "table busy id high-bit" },
        { 0xFFFFFFFE, false, true, "table busy id max-1" },
        { 0xFFFFFFFF, false, true, "table busy id max" },
    };

    for (const auto& c : cases)
    {
        const bool gotClaim   = CanClaimTransport(c.id);
        const bool gotBusy    = TransportUserBusy(c.id);
        const bool inlineC    = inlineCanClaimTransport(c.id);
        const bool inlineB    = inlineTransportUserBusy(c.id);
        const bool pinGot     = pinCanClaimTransport(c.id);
        const bool pinPrior   = pinCanClaimTransport3732(c.id);
        const bool invertBusy = !TransportUserBusy(c.id);
        const bool wantEq0    = c.id == 0;

        ok = expect(gotClaim == c.wantClaim, c.label) && ok;
        ok = expect(gotBusy == c.wantBusy, c.label) && ok;
        ok = expect(gotClaim == inlineC, "dual-wire free==inline") && ok;
        ok = expect(gotClaim == pinGot, "dual-wire free==pin") && ok;
        ok = expect(gotClaim == pinPrior, "dual-wire free==pin3732") && ok;
        ok = expect(gotClaim == invertBusy, "formula free==!TransportUserBusy") && ok;
        ok = expect(gotClaim == wantEq0, "formula free==id==0") && ok;
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
        0, 1, 2, 3, 42, 99, 100, 255, 1000, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFE, 0xFFFFFFFF,
    };
    for (const uint32 id : composeIds)
    {
        const bool got = CanClaimTransport(id);
        ok             = expect(got == (id == 0), "compose id==0") && ok;
        ok             = expect(got == !TransportUserBusy(id), "compose !busy") && ok;
        ok             = expect(got == inlineCanClaimTransport(id), "compose inline") && ok;
        ok             = expect(got == pinCanClaimTransport(id), "compose pin") && ok;
        ok             = expect(got == pinCanClaimTransport3732(id), "compose pin3732") && ok;
    }

    // Compose identity: free function is the gate; host injects scalar only.
    ok = expect(CanClaimTransport(0) == pinCanClaimTransport(0),
                "formula free == pin id 0") &&
         ok;
    ok = expect(CanClaimTransport(0) == pinCanClaimTransport3732(0),
                "formula free == pin3732 id 0") &&
         ok;
    ok = expect(CanClaimTransport(0) == inlineCanClaimTransport(0),
                "formula free == inline id 0") &&
         ok;
    ok = expect(CanClaimTransport(0) == !TransportUserBusy(0),
                "formula free == !TransportUserBusy(0)") &&
         ok;
    ok = expect(!CanClaimTransport(99), "formula busy path must block") && ok;
    ok = expect(CanClaimTransport(99) == pinCanClaimTransport(99),
                "formula busy free == pin") &&
         ok;
    ok = expect(CanClaimTransport(99) == pinCanClaimTransport3732(99),
                "formula busy free == pin3732") &&
         ok;
    ok = expect(CanClaimTransport(99) == inlineCanClaimTransport(99),
                "formula busy free == inline") &&
         ok;
    ok = expect(CanClaimTransport(99) == !TransportUserBusy(99),
                "formula free == !TransportUserBusy(99)") &&
         ok;

    return ok;
}
