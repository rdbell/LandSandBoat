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

#include "test_modifier.h"

#include "map/modifier.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expectInt(std::int64_t actual, std::int64_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "modifier self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testPetModTypeValues() -> bool
{
    bool ok = true;
    ok      = expectInt(static_cast<int>(PetModType::All), 0, "PetModType::All") && ok;
    ok      = expectInt(static_cast<int>(PetModType::Avatar), 1, "PetModType::Avatar") && ok;
    ok      = expectInt(static_cast<int>(PetModType::Wyvern), 2, "PetModType::Wyvern") && ok;
    ok      = expectInt(static_cast<int>(PetModType::Automaton), 3, "PetModType::Automaton") && ok;
    ok      = expectInt(static_cast<int>(PetModType::Harlequin), 4, "PetModType::Harlequin") && ok;
    ok      = expectInt(static_cast<int>(PetModType::Valoredge), 5, "PetModType::Valoredge") && ok;
    ok      = expectInt(static_cast<int>(PetModType::Sharpshot), 6, "PetModType::Sharpshot") && ok;
    ok      = expectInt(static_cast<int>(PetModType::Stormwaker), 7, "PetModType::Stormwaker") && ok;
    ok      = expectInt(static_cast<int>(PetModType::Luopan), 8, "PetModType::Luopan") && ok;
    return ok;
}

auto testModifierAccessorsAndSetAmount() -> bool
{
    CModifier modifier(Mod::FASTCAST, -12);

    bool ok = true;
    ok      = expectInt(static_cast<int>(modifier.getModID()), static_cast<int>(Mod::FASTCAST), "modifier ID") && ok;
    ok      = expectInt(modifier.getModAmount(), -12, "modifier amount") && ok;

    modifier.setModAmount(23);
    ok = expectInt(modifier.getModAmount(), 23, "updated modifier amount") && ok;

    CModifier zeroAmount(Mod::FASTCAST);
    ok = expectInt(zeroAmount.getModAmount(), 0, "default modifier amount") && ok;
    return ok;
}

auto testPetModifierAccessorsAndSetAmount() -> bool
{
    CPetModifier modifier(Mod::STORETP, PetModType::Automaton, 15);

    bool ok = true;
    ok      = expectInt(static_cast<int>(modifier.getModID()), static_cast<int>(Mod::STORETP), "pet modifier ID") && ok;
    ok      = expectInt(modifier.getModAmount(), 15, "pet modifier amount") && ok;
    ok      = expectInt(static_cast<int>(modifier.getPetModType()), static_cast<int>(PetModType::Automaton), "pet modifier type") && ok;

    modifier.setModAmount(-3);
    ok = expectInt(modifier.getModAmount(), -3, "updated pet modifier amount") && ok;

    CPetModifier zeroAmount(Mod::STORETP, PetModType::Wyvern);
    ok = expectInt(zeroAmount.getModAmount(), 0, "default pet modifier amount") && ok;
    ok = expectInt(static_cast<int>(zeroAmount.getPetModType()), static_cast<int>(PetModType::Wyvern), "default pet modifier type") && ok;
    return ok;
}

} // namespace

auto runModifierSelfTests() -> bool
{
    bool ok = true;
    ok      = testPetModTypeValues() && ok;
    ok      = testModifierAccessorsAndSetAmount() && ok;
    ok      = testPetModifierAccessorsAndSetAmount() && ok;
    return ok;
}
