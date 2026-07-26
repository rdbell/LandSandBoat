#include "test_party_last_member_joined_6983.h"

#include "common/timer.h"

#define private public
#include "map/party.h"
#undef private

#include "map/entities/battle_entity.h"
#include "map/entities/char_entity.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party last member joined 6983 self-test failed: " << label << '\n';
    }
    return condition;
}

auto isNow(const timer::time_point value, const timer::time_point before, const timer::time_point after) -> bool
{
    return value >= before && value <= after;
}
} // namespace

// Direct CParty::GetTimeLastMemberJoined characterization (slice 6983). A
// character leader supplies its saved join time; missing or non-character
// leaders fall back to the current timer point.
auto runPartyLastMemberJoined6983SelfTests() -> bool
{
    CParty        party(1);
    CCharEntity   characterLeader;
    CBattleEntity nonCharacterLeader;

    const auto joined = timer::now() - std::chrono::seconds(2);
    characterLeader.m_LeaderCreatedPartyTime = joined;
    party.m_PLeader                          = &characterLeader;
    const bool characterLeaderTime = expect(party.GetTimeLastMemberJoined() == joined, "character leader join time");

    party.m_PLeader = nullptr;
    const auto noLeaderBefore = timer::now();
    const auto noLeaderTime   = party.GetTimeLastMemberJoined();
    const auto noLeaderAfter  = timer::now();
    const bool noLeaderNow    = expect(isNow(noLeaderTime, noLeaderBefore, noLeaderAfter), "missing leader uses now");

    party.m_PLeader = &nonCharacterLeader;
    const auto nonCharacterBefore = timer::now();
    const auto nonCharacterTime   = party.GetTimeLastMemberJoined();
    const auto nonCharacterAfter  = timer::now();
    const bool nonCharacterNow    = expect(isNow(nonCharacterTime, nonCharacterBefore, nonCharacterAfter), "non-character leader uses now");

    party.m_PLeader = nullptr;
    return characterLeaderTime && noLeaderNow && nonCharacterNow;
}
