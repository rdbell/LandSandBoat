#include "test_guild_points_math_1358.h"

#include "map/guild_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "guild points math 1358 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEq(const auto actual, const auto expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "guild points math 1358 self-test failed: " << label << " got "
                  << static_cast<long long>(actual) << " expected " << static_cast<long long>(expected) << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runGuildPointsMath1358SelfTests() -> bool
{
    bool ok = true;

    ok = expectEq(guildhelpers::ClampSkillRank(0), static_cast<uint8>(3), "clamp low") && ok;
    ok = expectEq(guildhelpers::ClampSkillRank(5), static_cast<uint8>(5), "clamp mid") && ok;
    ok = expectEq(guildhelpers::ClampSkillRank(15), static_cast<uint8>(9), "clamp high") && ok;
    ok = expectEq(guildhelpers::SkillRankToTier(3), static_cast<uint8>(0), "tier 0") && ok;
    ok = expectEq(guildhelpers::SkillRankToTier(9), static_cast<uint8>(6), "tier 6") && ok;
    ok = expectEq(guildhelpers::SkillRankCharIndex(2), static_cast<uint8>(50), "rank index") && ok;

    ok = expect(guildhelpers::IsDailyPointsEligible(0), "eligible 0") && ok;
    ok = expect(!guildhelpers::IsDailyPointsEligible(1), "ineligible 1") && ok;
    ok = expect(guildhelpers::IsDailyPointsEligible(100), "eligible 100") && ok;

    ok = expectEq(guildhelpers::PatternRankFromElapsedDays(11, 0), static_cast<uint8>(3), "seed 0") && ok;
    ok = expectEq(guildhelpers::PatternRankAfterAdvance(3, 0), static_cast<uint8>(0), "advance wrap") && ok;

    // remaining: max 100, cur 30 → 70; cur 150 → 0
    ok = expectEq(guildhelpers::RemainingDailyCapacity(30, 100), static_cast<uint16>(70), "remain") && ok;
    ok = expectEq(guildhelpers::RemainingDailyCapacity(150, 100), static_cast<uint16>(0), "remain over") && ok;

    // quantity: remaining 70, points 10 → (70/10)+1=8; reserve 5 → 5; reserve 20 → 8
    ok = expectEq(guildhelpers::TradeQuantity(30, 100, 10, 5), static_cast<uint16>(5), "qty reserve") && ok;
    ok = expectEq(guildhelpers::TradeQuantity(30, 100, 10, 20), static_cast<uint16>(8), "qty cap") && ok;
    ok = expectEq(guildhelpers::TradeQuantity(30, 100, 0, 20), static_cast<uint16>(0), "qty zero pts") && ok;

    // points: cur 30 max 100 pts 10 qty 5 → 50; qty 20 would be 200 → clamp room 70
    ok = expectEq(guildhelpers::ClampPointsToAdd(30, 100, 10, 5), static_cast<uint16>(50), "pts mid") && ok;
    ok = expectEq(guildhelpers::ClampPointsToAdd(30, 100, 10, 20), static_cast<uint16>(70), "pts room") && ok;
    ok = expectEq(guildhelpers::ClampPointsToAdd(150, 100, 10, 5), static_cast<uint16>(0), "pts over") && ok;

    auto add = guildhelpers::ComputeAddGuildPoints(true, true, 0, 100, 10, 5);
    ok       = expect(add.quantity == 0 && add.pointsToAdd == 0, "add ineligible") && ok;
    add      = guildhelpers::ComputeAddGuildPoints(false, false, 0, 100, 10, 5);
    ok       = expect(add.quantity == 0 && add.pointsToAdd == 0, "add no match") && ok;
    add      = guildhelpers::ComputeAddGuildPoints(false, true, 30, 100, 10, 5);
    ok       = expect(add.quantity == 5 && add.pointsToAdd == 50, "add ok") && ok;

    auto daily = guildhelpers::ComputeDailyGPItem(true, 1234, 30, 100);
    ok         = expect(daily.itemId == 1234 && daily.remainingPoints == 0, "daily ineligible") && ok;
    daily      = guildhelpers::ComputeDailyGPItem(false, 1234, 30, 100);
    ok         = expect(daily.itemId == 1234 && daily.remainingPoints == 70, "daily remain") && ok;

    ok = expectEq(guildhelpers::NewDailyPointsTotal(30, 50), static_cast<uint16>(80), "new total") && ok;

    return ok;
}
