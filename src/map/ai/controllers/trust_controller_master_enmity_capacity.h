#pragma once

#include <cstdint>

namespace trustcontrollermasterenmity
{
struct Record
{
    bool    active;
    int32_t cumulative;
    int32_t volatileEnmity;
};

template <typename EnmityList>
auto Resolve(const EnmityList& enmityList, const uint32_t masterID) -> Record
{
    if (const auto it = enmityList.find(masterID); it != enmityList.end())
    {
        const auto& entry = it->second;
        return { entry.active, entry.CE, entry.VE };
    }
    return {};
}
} // namespace trustcontrollermasterenmity
