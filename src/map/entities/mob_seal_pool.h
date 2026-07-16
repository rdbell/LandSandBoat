#pragma once

#include "common/cbasetypes.h"
#include "items.h"

#include <vector>

namespace mobsealpoolhelpers
{

inline auto EligibleSeals(const uint8 level, const bool abysseaEnabled) -> std::vector<uint16>
{
    if (level >= 80 && abysseaEnabled)
    {
        return { BEASTMENS_SEAL, KINDREDS_SEAL, KINDREDS_CREST, HIGH_KINDREDS_CREST };
    }

    if (level >= 70 && abysseaEnabled)
    {
        return { BEASTMENS_SEAL, KINDREDS_SEAL, KINDREDS_CREST };
    }

    if (level >= 50)
    {
        return { BEASTMENS_SEAL, KINDREDS_SEAL };
    }

    return { BEASTMENS_SEAL };
}

} // namespace mobsealpoolhelpers
