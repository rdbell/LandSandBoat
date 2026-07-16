/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_world_conquest_influence.h"

#include "world/conquest_system.h"

#include <array>
#include <iostream>
#include <string>

namespace
{

auto expectEqual(const std::array<int32, 4>& actual, const std::array<int32, 4>& expected, const std::string& label) -> bool
{
    if (actual == expected)
    {
        return true;
    }

    std::cerr << "World conquest influence self-test failed: " << label << '\n';
    return false;
}

auto testUnknownAndInvalidNation() -> bool
{
    std::array<int32, 4> influences{ 10, 20, 30, 40 };
    const auto           original = influences;
    bool                 ok       = !conquest::redistributeInfluencePoints(influences, 5, 0, REGION_TYPE::UNKNOWN, 100);
    ok                            = expectEqual(influences, original, "unknown region does not mutate") && ok;
    ok                            = !conquest::redistributeInfluencePoints(influences, 5, 4, REGION_TYPE::RONFAURE, 100) && ok;
    ok                            = expectEqual(influences, original, "invalid nation does not mutate") && ok;
    return ok;
}

auto testRoomAndCapClamp() -> bool
{
    std::array<int32, 4> under{ 10, 10, 10, 10 };
    std::array<int32, 4> exact{ 10, 10, 10, 10 };
    std::array<int32, 4> lowCap{};
    std::array<int32, 4> highCap{};

    bool ok = conquest::redistributeInfluencePoints(under, 20, 2, REGION_TYPE::RONFAURE, 100);
    ok      = expectEqual(under, { 10, 10, 30, 10 }, "under cap straight add") && ok;
    ok      = conquest::redistributeInfluencePoints(exact, 60, 1, REGION_TYPE::RONFAURE, 100) && ok;
    ok      = expectEqual(exact, { 10, 70, 10, 10 }, "exact cap straight add") && ok;
    ok      = conquest::redistributeInfluencePoints(lowCap, 1, 3, REGION_TYPE::RONFAURE, 0) && ok;
    ok      = expectEqual(lowCap, { 0, 0, 0, 1 }, "minimum cap clamp") && ok;
    ok      = conquest::redistributeInfluencePoints(highCap, 20000000, 0, REGION_TYPE::RONFAURE, 20000001) && ok;
    ok      = expectEqual(highCap, { 20000000, 0, 0, 0 }, "maximum cap clamp") && ok;
    return ok;
}

auto testRedistribution() -> bool
{
    std::array<int32, 4> proportional{ 20, 20, 20, 10 };
    std::array<int32, 4> targetAtCap{ 100, 0, 0, 0 };

    bool ok = conquest::redistributeInfluencePoints(proportional, 49, 0, REGION_TYPE::RONFAURE, 100);
    // Integer division leaves two overflow points unassigned: losses are 7, 7, and 3.
    ok      = expectEqual(proportional, { 67, 13, 13, 7 }, "overflow proportional losses round down") && ok;
    ok      = conquest::redistributeInfluencePoints(targetAtCap, 1, 0, REGION_TYPE::RONFAURE, 100) && ok;
    ok      = expectEqual(targetAtCap, { 100, 0, 0, 0 }, "target already at cap") && ok;
    return ok;
}

auto testNationIndices() -> bool
{
    bool ok = true;
    for (auto nation = 0u; nation < 4; ++nation)
    {
        std::array<int32, 4> influences{};
        ok = conquest::redistributeInfluencePoints(influences, 7, nation, REGION_TYPE::RONFAURE, 100) && ok;

        std::array<int32, 4> expected{};
        expected[nation] = 7;
        ok               = expectEqual(influences, expected, "nation index") && ok;
    }
    return ok;
}

} // namespace

auto runWorldConquestInfluenceSelfTests() -> bool
{
    return testUnknownAndInvalidNation() && testRoomAndCapClamp() && testRedistribution() && testNationIndices();
}
