#pragma once

#include "common/cbasetypes.h"

namespace charnamelookuphelpers
{

struct IDs
{
    uint32 charId{};
    uint32 accountId{};

    auto operator==(const IDs&) const -> bool = default;
};

constexpr auto CharIDFromResult(const bool hasRow, const uint32 charId) -> uint32
{
    return hasRow ? charId : 0;
}

constexpr auto AccountIDFromResult(const bool hasRow, const uint32 accountId) -> uint32
{
    return hasRow ? accountId : 0;
}

constexpr auto IDsFromResult(const bool hasRow, const uint32 charId, const uint32 accountId) -> IDs
{
    return hasRow ? IDs{ charId, accountId } : IDs{};
}

} // namespace charnamelookuphelpers
