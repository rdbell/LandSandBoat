#include "test_salvage_claim_transport_2871.h"

#include "map/salvage_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "salvage claim transport 2871 self-test failed: " << label << '\n';
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

} // namespace

// Pure dual-wire expansion for salvagehelpers::CanClaimTransport /
// TransportUserBusy (Lua onTransportUpdate transportUser gate).
auto runSalvageClaimTransport2871SelfTests() -> bool
{
    using salvagehelpers::CanClaimTransport;
    using salvagehelpers::TransportUserBusy;

    bool ok = true;

    // Residual 0977 / 1083 pins.
    ok = expect(!TransportUserBusy(0), "transportUser 0 free") && ok;
    ok = expect(TransportUserBusy(1), "transportUser 1 busy") && ok;
    ok = expect(CanClaimTransport(0), "transportUser 0 may claim") && ok;
    ok = expect(!CanClaimTransport(42), "transportUser 42 cannot claim") && ok;

    // --- CanClaimTransport / TransportUserBusy table ---
    const struct
    {
        uint32      id;
        bool        wantClaim;
        bool        wantBusy;
        const char* label;
    } cases[] = {
        { 0, true, false, "id 0 free claim" },
        { 1, false, true, "id 1 busy" },
        { 42, false, true, "id 42 busy" },
        { 100, false, true, "id 100 busy" },
        { 0xFFFFFFFF, false, true, "id max busy" },
    };

    for (const auto& c : cases)
    {
        const bool gotClaim  = CanClaimTransport(c.id);
        const bool gotBusy   = TransportUserBusy(c.id);
        const bool inlineC   = inlineCanClaimTransport(c.id);
        const bool inlineB   = inlineTransportUserBusy(c.id);
        const bool invertBusy = !TransportUserBusy(c.id);

        ok = expect(gotClaim == c.wantClaim, c.label) && ok;
        ok = expect(gotBusy == c.wantBusy, c.label) && ok;
        ok = expect(gotClaim == inlineC, "dual-wire CanClaimTransport == inline Lua") && ok;
        ok = expect(gotBusy == inlineB, "dual-wire TransportUserBusy == inline Lua") && ok;
        ok = expect(gotClaim == invertBusy, "CanClaimTransport == !TransportUserBusy") && ok;
        ok = expect(gotClaim != gotBusy, "claim and busy are inverse") && ok;
    }

    return ok;
}
