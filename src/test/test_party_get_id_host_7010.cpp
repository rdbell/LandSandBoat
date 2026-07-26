#include "test_party_get_id_host_7010.h"

#include <iostream>

#include "map/party.h"

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party get ID host 7010 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CParty::GetPartyID characterization (slice 7010). It returns the
// current ID exactly, including a value assigned through SetPartyID.
auto runPartyGetIDHost7010SelfTests() -> bool
{
    CParty party(17);
    const bool initial = expect(party.GetPartyID() == 17, "returns constructor ID");

    party.SetPartyID(42);
    return initial && expect(party.GetPartyID() == 42, "returns assigned ID");
}
