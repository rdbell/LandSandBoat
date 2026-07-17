#pragma once

#include "common/cbasetypes.h"

#include <cstddef>
#include <vector>

namespace fishingcontest
{

// RankEntryAt applies GetFishRankEntry's real-result/placeholder fallback
// policy without depending on the process-global contest vectors.
template <typename Entry>
auto RankEntryAt(std::vector<Entry>& entries, std::vector<Entry>& placeholders, const uint8 position) -> Entry*
{
    if (position < entries.size())
    {
        return &entries[position];
    }

    if (!placeholders.empty())
    {
        return &placeholders[(position - entries.size()) % placeholders.size()];
    }

    return nullptr;
}

} // namespace fishingcontest
