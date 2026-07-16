/*
===========================================================================

  Copyright (c) 2025 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "autotranslate_helpers.h"

std::string autotranslate::replaceBytesWithLookup(const std::string& str, const Lookup& lookup)
{
    bool                inATBlock = false;
    std::string         outStr;
    std::vector<uint16> data;

    for (const auto ch : str)
    {
        const auto chAsInt = static_cast<uint16>(ch);
        if (chAsInt == 65533 && !inATBlock)
        {
            outStr += "{";
            inATBlock = true;
        }
        else if (chAsInt == 65533 && inATBlock)
        {
            outStr += lookup(str, data);
            data.clear();

            outStr += "}";
            inATBlock = false;
        }
        else if (!inATBlock)
        {
            outStr += ch;
        }
        else
        {
            data.emplace_back(chAsInt);
        }
    }

    return outStr;
}
