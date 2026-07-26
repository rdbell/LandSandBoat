#include "test_party_member_lookup_host_6989.h"

#include <iostream>

#define private public
#include "map/party.h"
#undef private

#include "map/entities/battle_entity.h"

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party member lookup host 6989 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CParty::GetMemberByName characterization (slice 6989). PC parties
// search their local roster case-insensitively; empty names and mob parties
// are rejected before the scan.
auto runPartyMemberLookupHost6989SelfTests() -> bool
{
    CParty        party(1);
    CBattleEntity alice;
    CBattleEntity bob;

    alice.name = "Alice";
    bob.name   = "Bob";
    party.members = { &alice, &bob };

    const bool pcSearch = expect(party.GetMemberByName("alice") == &alice, "case-insensitive first member lookup") &&
                          expect(party.GetMemberByName("BOB") == &bob, "case-insensitive later member lookup") &&
                          expect(party.GetMemberByName("Carol") == nullptr, "unknown member rejected") &&
                          expect(party.GetMemberByName("") == nullptr, "empty name rejected");

    party.m_PartyType = PARTY_MOBS;
    const bool mobRejected = expect(party.GetMemberByName("Alice") == nullptr, "mob party lookup rejected");

    return pcSearch && mobRejected;
}
