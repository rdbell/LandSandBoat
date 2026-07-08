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

#include "test_ximesh_null_mesh.h"

#include "map/ximesh/iximesh.h"

#include <cstdint>
#include <iostream>

namespace
{

auto expectBool(bool actual, bool expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "ximesh null mesh self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectUInt(std::uint64_t actual, std::uint64_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "ximesh null mesh self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testNullMeshQueries() -> bool
{
    const NullXiMesh mesh;
    bool             ok = true;

    ok = expectBool(mesh.query(1.0f, 2.0f, 3.0f).has_value(), false, "query empty") && ok;
    ok = expectUInt(static_cast<std::uint8_t>(mesh.getTerrainAt(1.0f, 2.0f, 3.0f)),
                    static_cast<std::uint8_t>(TerrainType::None),
                    "terrain none") &&
         ok;
    ok = expectUInt(mesh.getFloorId(1.0f, 2.0f, 3.0f), 0, "floor id zero") && ok;
    ok = expectBool(mesh.rayIntersect(Vector3{ 0.0f, 1.0f, 2.0f }, Vector3{ 3.0f, 4.0f, 5.0f }, IgnoreTransparentBarriers::Yes), false, "rayIntersect yes") && ok;
    ok = expectBool(mesh.rayIntersect(Vector3{ 0.0f, 1.0f, 2.0f }, Vector3{ 3.0f, 4.0f, 5.0f }, IgnoreTransparentBarriers::No), false, "rayIntersect no") && ok;
    ok = expectBool(mesh.getPositionInfo(Vector3{ 1.0f, 2.0f, 3.0f }, YOffsets{ 4.0f, -8.0f }, IgnoreTransparentBarriers::Yes).has_value(), false, "position info empty") && ok;
    return ok;
}

auto testNullMeshStorageViews() -> bool
{
    const NullXiMesh mesh;
    bool             ok = true;

    ok = expectBool(mesh.blocks().empty(), true, "blocks empty") && ok;
    ok = expectBool(mesh.placements().empty(), true, "placements empty") && ok;
    ok = expectBool(mesh.entries().empty(), true, "entries empty") && ok;
    ok = expectBool(mesh.cells().empty(), true, "cells empty") && ok;
    ok = expectUInt(mesh.gridWidth(), 0, "grid width zero") && ok;
    ok = expectUInt(mesh.gridHeight(), 0, "grid height zero") && ok;
    return ok;
}

} // namespace

auto runXimeshNullMeshSelfTests() -> bool
{
    bool ok = true;
    ok      = testNullMeshQueries() && ok;
    ok      = testNullMeshStorageViews() && ok;
    return ok;
}
