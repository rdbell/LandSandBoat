#pragma once

#include <cstdint>

namespace zoneentitylookup
{

enum class TargetRange : std::uint8_t
{
    Static,
    Character,
    Dynamic,
    Invalid,
};

// ClassifyTargetID mirrors CZoneEntities::GetEntity's targid partitions.
constexpr auto ClassifyTargetID(const std::uint16_t targid) -> TargetRange
{
    if (targid < 0x400)
    {
        return TargetRange::Static;
    }
    if (targid < 0x700)
    {
        return TargetRange::Character;
    }
    if (targid < 0x1000)
    {
        return TargetRange::Dynamic;
    }
    return TargetRange::Invalid;
}

// RequestsType mirrors GetEntity's bitwise entity-type filter.
constexpr auto RequestsType(const std::uint8_t filter, const std::uint8_t entityType) -> bool
{
    return filter & entityType;
}

} // namespace zoneentitylookup
