/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#pragma once

#include <string>
#include <string_view>

namespace zonemeshhelpers
{

// Resolves the exceptional zone-settings name spellings used by ximesh files.
inline auto ResolveXiMeshName(std::string meshName) -> std::string
{
    if (meshName.size() >= 2 && meshName.substr(meshName.size() - 2) == "_U")
    {
        meshName.replace(meshName.size() - 2, 2, "_[U]");
    }

    if (meshName.starts_with("Escha_"))
    {
        meshName.replace(5, 1, "-");
    }

    if (meshName.starts_with("Desuetia_"))
    {
        meshName.replace(8, 1, "-");
    }

    if (meshName == "Ship_bound_for_Selbina_Pirates")
    {
        meshName = "Ship_bound_for_Selbina_ID-227";
    }
    else if (meshName == "Ship_bound_for_Mhaura_Pirates")
    {
        meshName = "Ship_bound_for_Mhaura_ID-228";
    }

    if (meshName.starts_with("Maquette_Abdhaljs-Legion_"))
    {
        meshName.erase(meshName.size() - 2, 1);
    }

    return meshName;
}

} // namespace zonemeshhelpers
