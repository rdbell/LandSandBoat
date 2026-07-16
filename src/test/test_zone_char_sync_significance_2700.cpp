/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_zone_char_sync_significance_2700.h"

#include "map/zone_char_sync_significance.h"

#include <iostream>

namespace
{

auto expect(const float got, const float want, const char* const label) -> bool
{
    if (got != want)
    {
        std::cerr << "zone character sync significance 2700 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runZoneCharSyncSignificance2700SelfTests() -> bool
{
    using zoneentityvisibility::CharacterSyncAllianceSignificance;
    using zoneentityvisibility::CharacterSyncPartySignificance;
    using zoneentityvisibility::CharacterSyncSignificance;

    bool ok = true;
    ok      = expect(CharacterSyncSignificance(false, false, false), 0.0f, "unrelated characters score zero") && ok;
    ok      = expect(CharacterSyncSignificance(false, false, true), CharacterSyncAllianceSignificance, "alliance gets alliance score") && ok;
    ok      = expect(CharacterSyncSignificance(false, true, true), CharacterSyncPartySignificance, "party outranks alliance") && ok;
    ok      = expect(CharacterSyncSignificance(true, true, true), CharacterSyncAllianceSignificance, "visible GM takes precedence") && ok;
    return ok;
}
