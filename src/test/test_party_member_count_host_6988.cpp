#include "test_party_member_count_host_6988.h"

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
        std::cerr << "party member count host 6988 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Direct CParty::MemberCount characterization (slice 6988). Characters add
// their trusts even when in another zone; non-character members only count in
// the requested zone.
auto runPartyMemberCountHost6988SelfTests() -> bool
{
    CParty        party(1);
    CCharEntity   inZoneCharacter;
    CCharEntity   offZoneCharacter;
    CBattleEntity inZoneBattle;
    CBattleEntity offZoneBattle;
    CParty        wrappedParty(2);
    CCharEntity   wrappedCharacter;

    inZoneCharacter.loc.destination  = 100;
    offZoneCharacter.loc.destination = 101;
    inZoneBattle.loc.destination     = 100;
    offZoneBattle.loc.destination    = 101;
    inZoneCharacter.PTrusts.resize(2);
    offZoneCharacter.PTrusts.resize(3);

    party.members = { &inZoneCharacter, &offZoneCharacter, &inZoneBattle, &offZoneBattle };

    const bool zone100 = expect(party.MemberCount(100) == 7, "zone member plus local/off-zone trusts counted");
    const bool zone101 = expect(party.MemberCount(101) == 7, "other-zone member plus all trusts counted");

    wrappedCharacter.loc.destination = 102;
    wrappedCharacter.PTrusts.resize(255);
    wrappedParty.members = { &wrappedCharacter };
    const bool wraps     = expect(wrappedParty.MemberCount(102) == 0, "uint8 total wraps after member plus 255 trusts");

    inZoneCharacter.PTrusts.clear();
    offZoneCharacter.PTrusts.clear();
    wrappedCharacter.PTrusts.clear();

    return zone100 && zone101 && wraps;
}
