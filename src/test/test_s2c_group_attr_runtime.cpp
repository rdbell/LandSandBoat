/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_s2c_group_attr_runtime.h"

#include <iostream>

#include "map/packets/s2c/0x0df_group_attr.h"

namespace
{

auto expectPlan(const groupattrhelpers::PacketPlan actual, const groupattrhelpers::PacketPlan expected, const char* label) -> bool
{
    if (actual.uniqueNo != expected.uniqueNo || actual.hp != expected.hp || actual.mp != expected.mp || actual.tp != expected.tp || actual.actIndex != expected.actIndex || actual.hpp != expected.hpp || actual.mpp != expected.mpp || actual.monstrosityNameId != expected.monstrosityNameId || actual.mjobNo != expected.mjobNo || actual.mjobLv != expected.mjobLv || actual.sjobNo != expected.sjobNo || actual.sjobLv != expected.sjobLv)
    {
        std::cerr << "s2c GROUP_ATTR runtime self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runS2CGroupAttrRuntimeSelfTests() -> bool
{
    using namespace groupattrhelpers;

    const auto common = CommonFacts{ .uniqueNo = 0x11223344, .hp = 1000, .mp = 200, .tp = 3000, .actIndex = 0x1234, .hpp = 75, .mpp = 50, .mjobNo = 6, .mjobLv = 99, .sjobNo = 3, .sjobLv = 49 };
    bool       ok     = true;

    auto character              = CharacterFacts{};
    character.common            = common;
    character.hasMonstrosity    = true;
    character.monstrosityNameId = 0xBEEF;
    ok                          = expectPlan(CharacterPlanFor(character), { .uniqueNo = 0x11223344, .hp = 1000, .mp = 200, .tp = 3000, .actIndex = 0x1234, .hpp = 75, .mpp = 50, .monstrosityNameId = 0xBEEF, .mjobNo = 6, .mjobLv = 99, .sjobNo = 3, .sjobLv = 49 }, "non-anon character preserves jobs and monstrosity") && ok;

    character.anonymous = true;
    ok                  = expectPlan(CharacterPlanFor(character), { .uniqueNo = 0x11223344, .hp = 1000, .mp = 200, .tp = 3000, .actIndex = 0x1234, .hpp = 75, .mpp = 50, .monstrosityNameId = 0xBEEF }, "anonymous character suppresses jobs") && ok;

    ok = expectPlan(TrustPlanFor(common), { .uniqueNo = 0x11223344, .hp = 1000, .mp = 200, .tp = 3000, .actIndex = 0x1234, .hpp = 75, .mpp = 50, .mjobNo = 6, .mjobLv = 99, .sjobNo = 3, .sjobLv = 49 }, "trust preserves jobs and common fields") && ok;
    return ok;
}
