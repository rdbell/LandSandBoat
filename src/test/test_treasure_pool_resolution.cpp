/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_treasure_pool_resolution.h"

#include "common/timer.h"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>

namespace
{

constexpr auto lifetime = 5min;

struct Member
{
    std::uint32_t id;
    std::uint8_t  freeSlots;
    bool          hasRare;
    bool          passed;
};

struct Lot
{
    std::uint32_t member;
    std::uint16_t value;
};

enum class Result
{
    Pending,
    Won,
    WinError,
    Lost,
};

struct Outcome
{
    Result        result{ Result::Pending };
    std::uint32_t winner{};
    std::uint16_t highest{};
    bool          random{};
    bool          removed{};
};

// Narrow characterization of CTreasurePool::checkTreasureItem. Keeping the
// model free of CCharEntity/item-database setup lets these boundary cases pin
// the source algorithm without requiring a running map service.
auto resolve(timer::time_point added, timer::time_point tick, const std::vector<Member>& members,
             const std::vector<Lot>& lots, bool rare, std::size_t selectedCandidate, bool insertSucceeds) -> Outcome
{
    Outcome out;
    const auto soloReady = members.size() == 1 && members.front().freeSlots != 0;
    if (!(tick - added > lifetime) && !soloReady && lots.size() != members.size())
    {
        return out;
    }

    const Member* highestMember = nullptr;
    for (const auto& lot : lots)
    {
        if (lot.value > out.highest)
        {
            out.highest = lot.value;
            highestMember = &*std::find_if(members.begin(), members.end(), [&](const Member& member) { return member.id == lot.member; });
        }
    }

    if (highestMember != nullptr && out.highest != 0)
    {
        out.removed = true;
        if (highestMember->freeSlots == 0)
        {
            out.result = Result::Lost;
            return out;
        }
        out.winner = highestMember->id;
        out.result = insertSucceeds ? Result::Won : Result::WinError;
        return out;
    }

    std::vector<const Member*> candidates;
    for (const auto& member : members)
    {
        if ((!rare || !member.hasRare) && member.freeSlots != 0 && !member.passed)
        {
            candidates.emplace_back(&member);
        }
    }
    out.removed = true;
    if (candidates.empty())
    {
        out.result = Result::Lost;
        return out;
    }
    out.random = true;
    out.winner = candidates.at(selectedCandidate)->id;
    out.result = insertSucceeds ? Result::Won : Result::WinError;
    return out;
}

auto expect(bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "treasure pool resolution self-test failed: " << label << '\n';
    }
    return condition;
}

auto testStrictTimeoutAndFirstTieWinner() -> bool
{
    const auto added = timer::time_point{} + 10min;
    const auto members = std::vector<Member>{ { 10, 1, false, false }, { 20, 1, false, false }, { 30, 1, false, false } };

    auto exact = resolve(added, added + lifetime, members, { { 10, 900 }, { 20, 900 } }, false, 0, true);
    auto late  = resolve(added, added + lifetime + 1ms, members, { { 10, 900 }, { 20, 900 } }, false, 0, true);

    bool ok = true;
    ok = expect(exact.result == Result::Pending && !exact.removed, "exact five-minute boundary remains pending") && ok;
    ok = expect(late.result == Result::Won && late.winner == 10 && late.highest == 900, "first equal high lot wins") && ok;
    ok = expect(late.removed && !late.random, "lot winner removes without random selection") && ok;
    return ok;
}

auto testWinnerFailuresAndCandidateFiltering() -> bool
{
    bool ok = true;
    auto full = resolve({}, {}, { { 7, 0, false, false }, { 8, 1, false, true } }, { { 7, 400 }, { 8, 0 } }, false, 0, true);
    ok = expect(full.result == Result::Lost && full.winner == 0 && full.removed, "full high lotter loses item") && ok;

    auto insertionError = resolve({}, {}, { { 7, 1, false, false }, { 8, 1, false, true } }, { { 7, 400 }, { 8, 0 } }, false, 0, false);
    ok = expect(insertionError.result == Result::WinError && insertionError.winner == 7, "failed high-lot insertion reports error") && ok;

    const auto candidates = std::vector<Member>{
        { 11, 1, false, true },
        { 22, 1, true, false },
        { 33, 1, false, false },
        { 44, 1, false, false },
    };
    auto random = resolve({}, timer::time_point{} + lifetime + 1ms, candidates, {}, true, 1, true);
    ok = expect(random.result == Result::Won && random.winner == 44 && random.random, "candidate order after pass rare filtering") && ok;

    auto none = resolve({}, timer::time_point{} + lifetime + 1ms, { { 11, 1, false, true }, { 22, 0, false, false } }, {}, false, 0, true);
    ok = expect(none.result == Result::Lost && none.removed, "no eligible candidates loses item") && ok;
    return ok;
}

} // namespace

auto runTreasurePoolResolutionSelfTests() -> bool
{
    return testStrictTimeoutAndFirstTieWinner() && testWinnerFailuresAndCandidateFiltering();
}
