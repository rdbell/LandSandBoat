/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_zone_mesh_name_2691.h"

#include "map/zone_mesh_name.h"

#include <iostream>
#include <string_view>

namespace
{

auto expectEqual(const std::string_view input, const std::string_view want) -> bool
{
    const auto got = zonemeshhelpers::ResolveXiMeshName(std::string{ input });
    if (got != want)
    {
        std::cerr << "zone mesh name 2691 self-test failed: " << input << " resolved to " << got << " instead of " << want << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runZoneMeshName2691SelfTests() -> bool
{
    bool ok = true;
    ok      = expectEqual("Rala_Waterways_U", "Rala_Waterways_[U]") && ok;
    ok      = expectEqual("Escha_ZiTah", "Escha-ZiTah") && ok;
    ok      = expectEqual("Desuetia_Empyreal_Paradox", "Desuetia-Empyreal_Paradox") && ok;
    ok      = expectEqual("Ship_bound_for_Selbina_Pirates", "Ship_bound_for_Selbina_ID-227") && ok;
    ok      = expectEqual("Ship_bound_for_Mhaura_Pirates", "Ship_bound_for_Mhaura_ID-228") && ok;
    ok      = expectEqual("Maquette_Abdhaljs-Legion_A", "Maquette_Abdhaljs-LegionA") && ok;
    ok      = expectEqual("Ordinary_Zone", "Ordinary_Zone") && ok;
    return ok;
}
