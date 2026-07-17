#include "test_party_invite_alliance_join_2843.h"

#include "map/party_invite_response.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party invite alliance join 2843 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure dual-wire expansion for mapipc::CanJoinExistingAlliance.
// Truth table over alliance party counts 0..4 (MaxAllianceParties exclusive = 3).
auto runPartyInviteAllianceJoin2843SelfTests() -> bool
{
    using mapipc::CanJoinExistingAlliance;
    using mapipc::MaxAllianceParties;

    bool ok = true;

    ok = expect(MaxAllianceParties == 3, "MaxAllianceParties pin") && ok;

    // Truth table: join only when 0 < count < MaxAllianceParties.
    const struct
    {
        std::size_t count;
        bool        want;
        const char* label;
    } cases[] = {
        { 0, false, "0 parties empty" },
        { 1, true, "1 party joinable" },
        { 2, true, "2 parties joinable" },
        { 3, false, "3 parties full" },
        { 4, false, "4 parties over capacity" },
    };

    for (const auto& c : cases)
    {
        const bool got       = CanJoinExistingAlliance(c.count);
        const bool inlineGot = c.count > 0 && c.count < MaxAllianceParties;
        ok                   = expect(got == c.want, c.label) && ok;
        ok                   = expect(got == inlineGot, "dual-wire matches inline formula") && ok;
    }

    // Production HandlePartyInviteResponse dual-wires this pure gate on the
    // both-leaders + hasAlliance accept path (2 allows join; 3 rejects full).
    ok = expect(CanJoinExistingAlliance(2), "production join path allows 2") && ok;
    ok = expect(!CanJoinExistingAlliance(3), "production reject path blocks 3") && ok;

    return ok;
}
