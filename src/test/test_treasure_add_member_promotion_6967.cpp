#include "test_treasure_add_member_promotion_6967.h"

#include "map/alliance.h"
#include "map/entities/char_entity.h"
#include "map/party.h"
#include "map/treasure_pool.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure addMember promotion 6967 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Direct CTreasurePool::addMember characterization (slice 6967). A member's
// party graph promotes a Solo pool to Party, and a Party pool to Alliance only
// when that party has an alliance.
auto runTreasureAddMemberPromotion6967SelfTests() -> bool
{
    CParty    party(1);
    CAlliance alliance(1);

    bool ok = true;
    {
        CTreasurePool pool(TreasurePoolType::Solo);
        CCharEntity   member;
        member.PTreasurePool = &pool;
        member.PParty        = &party;
        pool.addMember(&member);
        ok = expect(pool.getPoolType() == TreasurePoolType::Party, "party member promotes solo pool") && ok;
        member.PTreasurePool = nullptr;
        member.PParty        = nullptr;
    }

    {
        party.m_PAlliance = &alliance;
        CTreasurePool pool(TreasurePoolType::Party);
        CCharEntity   member;
        member.PTreasurePool = &pool;
        member.PParty        = &party;
        pool.addMember(&member);
        ok = expect(pool.getPoolType() == TreasurePoolType::Alliance, "alliance party promotes party pool") && ok;
        member.PTreasurePool = nullptr;
        member.PParty        = nullptr;
        party.m_PAlliance    = nullptr;
    }

    return ok;
}
