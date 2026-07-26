#include "test_party_add_member_host_7003.h"

#include <iostream>

#define private public
#include "map/party.h"
#undef private

#include "map/entities/battle_entity.h"
#include "map/entities/char_entity.h"
#include "map/treasure_pool.h"

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party add member host 7003 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CParty::AddMember characterization (slice 7003). A valid entity is
// attached and appended even when its default entity ID is zero; the next call
// rejects the same in-list entity before a second append. The PC path also
// reloads its pool and increments party-join history after admission.
auto runPartyAddMemberHost7003SelfTests() -> bool
{
    CParty        party(1);
    CBattleEntity member;
    party.m_PartyType = PARTY_MOBS;

    party.AddMember(&member);
    const bool firstAdd = expect(member.PParty == &party, "member attached") &&
                          expect(party.members.size() == 1, "member appended") &&
                          expect(party.members[0] == &member, "member retained");

    party.AddMember(&member);

    CTreasurePool pcPool(TreasurePoolType::Solo);
    CParty        pcParty(2);
    CCharEntity   pcMember;
    pcMember.PTreasurePool = &pcPool;
    pcPool.addMember(&pcMember);

    pcParty.AddMember(&pcMember);
    const bool pcPostProcess = expect(pcMember.PParty == &pcParty, "PC member attached") &&
                               expect(pcParty.members.size() == 1 && pcParty.members.front() == &pcMember, "PC member appended") &&
                               expect(pcMember.m_charHistory.joinedParties == 1, "PC party join history increments") &&
                               expect(pcMember.PTreasurePool == &pcPool && pcPool.isMember(&pcMember), "PC treasure pool remains valid");

    pcMember.PTreasurePool = nullptr;
    pcMember.PParty        = nullptr;
    pcParty.members.clear();
    return firstAdd && expect(party.members.size() == 1, "duplicate member rejected") && pcPostProcess;
}
