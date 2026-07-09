/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
  Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_trait.h"

#include "map/blue_trait.h"
#include "map/entities/battle_entity.h"
#include "map/trait.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expectInt(std::int64_t actual, std::int64_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "trait self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testTraitDefaultsAndSetters() -> bool
{
    CTrait trait(12);

    bool ok = true;
    ok      = expectInt(trait.getID(), 12, "trait ID") && ok;
    ok      = expectInt(trait.getJob(), 0, "default job") && ok;
    ok      = expectInt(trait.getLevel(), 0, "default level") && ok;
    ok      = expectInt(static_cast<int>(trait.getMod()), static_cast<int>(Mod::NONE), "default mod") && ok;
    ok      = expectInt(trait.getValue(), 0, "default value") && ok;
    ok      = expectInt(trait.getRank(), 0, "default rank") && ok;
    ok      = expectInt(trait.getMeritID(), 0, "default merit ID") && ok;

    trait.setJob(16);
    trait.setLevel(45);
    trait.setMod(Mod::FASTCAST);
    trait.setValue(-7);
    trait.setRank(3);
    trait.setMeritId(1024);

    ok = expectInt(trait.getJob(), 16, "updated job") && ok;
    ok = expectInt(trait.getLevel(), 45, "updated level") && ok;
    ok = expectInt(static_cast<int>(trait.getMod()), static_cast<int>(Mod::FASTCAST), "updated mod") && ok;
    ok = expectInt(trait.getValue(), -7, "updated value") && ok;
    ok = expectInt(trait.getRank(), 3, "updated rank") && ok;
    ok = expectInt(trait.getMeritID(), 1024, "updated merit ID") && ok;

    trait.setJob(-1);
    ok = expectInt(trait.getJob(), 255, "signed job setter stores byte") && ok;
    return ok;
}

auto testBlueTraitDefaultsAndSetters() -> bool
{
    CBlueTrait trait(4, 12, true);

    bool ok = true;
    ok      = expectInt(trait.getID(), 12, "blue trait ID") && ok;
    ok      = expectInt(trait.getCategory(), 4, "blue trait category") && ok;
    ok      = expectInt(trait.getPoints(), 0, "default blue trait points") && ok;
    ok      = expectInt(trait.getJobPointsOnly(), 1, "blue trait job-points-only") && ok;

    trait.setPoints(8);
    trait.setMod(Mod::STORETP);
    trait.setValue(20);

    ok = expectInt(trait.getPoints(), 8, "updated blue trait points") && ok;
    ok = expectInt(static_cast<int>(trait.getMod()), static_cast<int>(Mod::STORETP), "updated blue trait mod") && ok;
    ok = expectInt(trait.getValue(), 20, "updated blue trait value") && ok;

    CBlueTrait withoutJobPoints(5, 13, false);
    ok = expectInt(withoutJobPoints.getJobPointsOnly(), 0, "blue trait not job-points-only") && ok;
    return ok;
}

auto testTraitRegistryJobLookup() -> bool
{
    auto* noneTraits = traits::GetTraits(JOB_NON);
    auto* warTraits  = traits::GetTraits(JOB_WAR);
    auto* lastTraits = traits::GetTraits(MAX_JOBTYPE - 1);

    bool ok = true;
    ok      = expectInt(noneTraits != nullptr, 1, "job non trait list") && ok;
    ok      = expectInt(warTraits != nullptr, 1, "valid job trait list") && ok;
    ok      = expectInt(lastTraits != nullptr, 1, "last valid job trait list") && ok;
    ok      = expectInt(traits::GetTraits(MAX_JOBTYPE) == nullptr, 1, "invalid job trait list") && ok;

    if (warTraits != nullptr)
    {
        const auto originalSize = warTraits->size();
        CTrait     trait(77);
        trait.setJob(JOB_WAR);
        warTraits->push_back(&trait);

        ok = expectInt(traits::GetTraits(JOB_WAR) == warTraits, 1, "same job trait list pointer") && ok;
        ok = expectInt(warTraits->size(), originalSize + 1, "inserted trait list size") && ok;
        ok = expectInt(warTraits->back()->getID(), 77, "inserted trait ID") && ok;

        warTraits->pop_back();
        ok = expectInt(warTraits->size(), originalSize, "restored trait list size") && ok;
    }

    return ok;
}

} // namespace

auto runTraitSelfTests() -> bool
{
    bool ok = true;
    ok      = testTraitDefaultsAndSetters() && ok;
    ok      = testBlueTraitDefaultsAndSetters() && ok;
    ok      = testTraitRegistryJobLookup() && ok;
    return ok;
}
