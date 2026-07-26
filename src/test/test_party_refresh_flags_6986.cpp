#include "test_party_refresh_flags_6986.h"

#include <iostream>
#include <string>
#include <vector>

#define private public
#include "map/party.h"
#undef private

#include "map/alliance.h"
#include "map/entities/battle_entity.h"

struct CParty::partyInfo_t
{
    uint32      id         = {};
    uint32      partyid    = {};
    uint32      allianceid = {};
    std::string name       = {};
    uint16      flags      = {};
    uint16      zone       = {};
    uint16      prev_zone  = {};
};

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "party refresh flags 6986 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Direct CParty::RefreshFlags characterization (slice 6986). Matching rows
// rebuild local role pointers and alliance main-party state; unmatched role
// rows clear the affected pointers and main party.
auto runPartyRefreshFlags6986SelfTests() -> bool
{
    CParty        party(1);
    CAlliance     alliance(1);
    CBattleEntity leader;
    CBattleEntity quarterMaster;
    CBattleEntity syncTarget;

    leader.id        = 11;
    quarterMaster.id = 12;
    syncTarget.id    = 13;
    party.members.emplace_back(&leader);
    party.members.emplace_back(&quarterMaster);
    party.members.emplace_back(&syncTarget);
    party.m_PAlliance = &alliance;

    std::vector<CParty::partyInfo_t> info{
        { leader.id, 1, 1, "leader", static_cast<uint16>(PARTY_LEADER | ALLIANCE_LEADER), 0, 0 },
        { quarterMaster.id, 1, 1, "qm", PARTY_QM, 0, 0 },
        { syncTarget.id, 1, 1, "sync", PARTY_SYNC, 0, 0 },
    };
    party.RefreshFlags(info);

    const bool assigned = expect(party.GetLeader() == &leader, "leader assigned") &&
                          expect(party.GetQuaterMaster() == &quarterMaster, "quartermaster assigned") &&
                          expect(party.GetSyncTarget() == &syncTarget, "sync target assigned") &&
                          expect(alliance.getMainParty() == &party, "alliance main party assigned");

    info = {
        { 99, 1, 1, "missing", static_cast<uint16>(PARTY_LEADER | ALLIANCE_LEADER), 0, 0 },
    };
    party.RefreshFlags(info);

    const bool cleared = expect(party.GetLeader() == nullptr, "missing leader clears leader") &&
                         expect(party.GetQuaterMaster() == nullptr, "refresh clears quartermaster") &&
                         expect(party.GetSyncTarget() == nullptr, "refresh clears sync target") &&
                         expect(alliance.getMainParty() == nullptr, "missing alliance leader clears main party");

    party.m_PAlliance = nullptr;
    return assigned && cleared;
}
