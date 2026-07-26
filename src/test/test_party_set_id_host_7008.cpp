#include "test_party_set_id_host_7008.h"

#include <iostream>

#include "map/party.h"

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party set ID host 7008 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CParty::SetPartyID characterization (slice 7008). It assigns the
// supplied value directly, including zero.
auto runPartySetIDHost7008SelfTests() -> bool
{
    CParty party(17);

    party.SetPartyID(42);
    const bool assigned = expect(party.GetPartyID() == 42, "sets nonzero ID");

    party.SetPartyID(0);
    return assigned && expect(party.GetPartyID() == 0, "sets zero ID");
}
