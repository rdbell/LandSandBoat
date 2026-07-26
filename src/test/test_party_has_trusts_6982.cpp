#include "test_party_has_trusts_6982.h"

#include "map/entities/battle_entity.h"
#include "map/entities/char_entity.h"
#include "map/party.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party has trusts 6982 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CParty::HasTrusts characterization (slice 6982). Only character
// members contribute, and a single active trust makes the party report true.
auto runPartyHasTrusts6982SelfTests() -> bool
{
    CParty        party(1);
    CBattleEntity nonCharacter;
    CCharEntity   character;

    party.members.emplace_back(&nonCharacter);
    const bool noCharacterTrusts = expect(!party.HasTrusts(), "non-character member has no trusts");

    party.members.emplace_back(&character);
    const bool emptyCharacterTrusts = expect(!party.HasTrusts(), "character without trusts has no trusts");

    character.PTrusts.emplace_back(nullptr);
    const bool activeTrust = expect(party.HasTrusts(), "one active trust is found");

    character.PTrusts.clear();
    return noCharacterTrusts && emptyCharacterTrusts && activeTrust;
}
