#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure fishing area geometry + GetFishingArea selection from fishingutils.
// Parity: internal/fishingutils/area_geometry.go (+ area select; slice 1628).

namespace fishingareahelpers
{

// MAX_POINTS from fishingutils.cpp for poly ray end X.
constexpr float MaxAreaPoints = 10000.0f;

// Area types in GetFishingArea switch.
constexpr std::uint8_t AreaTypeWholeZone = 0;
constexpr std::uint8_t AreaTypeRadial    = 1;
constexpr std::uint8_t AreaTypePoly      = 2;

struct AreaVector
{
    float x = 0;
    float y = 0;
    float z = 0;
};

inline auto OnSegment(const AreaVector p, const AreaVector q, const AreaVector r) -> bool
{
    return q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) && q.z <= std::max(p.z, r.z) && q.z >= std::min(p.z, r.z);
}

inline auto Orientation(const AreaVector p, const AreaVector q, const AreaVector r) -> int
{
    const float val = std::round(q.z - p.z) * (r.x - q.x) - (q.x - p.x) * (r.z - q.z);
    if (val == 0)
    {
        return 0;
    }
    return (val > 0) ? 1 : 2;
}

inline auto DoIntersect(const AreaVector p1, const AreaVector q1, const AreaVector p2, const AreaVector q2) -> bool
{
    const int o1 = Orientation(p1, q1, p2);
    const int o2 = Orientation(p1, q1, q2);
    const int o3 = Orientation(p2, q2, p1);
    const int o4 = Orientation(p2, q2, q1);

    if (o1 != o2 && o3 != o4)
    {
        return true;
    }
    if (o1 == 0 && OnSegment(p1, p2, q1))
    {
        return true;
    }
    if (o2 == 0 && OnSegment(p1, q2, q1))
    {
        return true;
    }
    if (o3 == 0 && OnSegment(p2, p1, q2))
    {
        return true;
    }
    if (o4 == 0 && OnSegment(p2, q1, q2))
    {
        return true;
    }
    return false;
}

inline auto IsInsidePoly(const AreaVector* polygon, const int n, const AreaVector p, const float posy, const std::uint8_t height) -> bool
{
    if (p.y < (posy - (height / 2)) || p.y > (posy + (height / 2)))
    {
        return false;
    }
    if (n < 3)
    {
        return false;
    }

    const AreaVector extreme{ MaxAreaPoints, p.z, 0 };
    int              count = 0;
    int              i     = 0;
    do
    {
        const int next = (i + 1) % n;
        if (DoIntersect(polygon[i], polygon[next], p, extreme))
        {
            if (Orientation(polygon[i], p, polygon[next]) == 0)
            {
                return OnSegment(polygon[i], p, polygon[next]);
            }
            count++;
        }
        i = next;
    } while (i != 0);

    return (count & 1) != 0;
}

inline auto IsInsideCylinder(const AreaVector center, const AreaVector p, const std::uint16_t radius, const std::uint8_t height) -> bool
{
    if (p.y < (center.y - (height / 2)) || p.y > (center.y + (height / 2)))
    {
        return false;
    }

    const float dx = std::abs(p.x - center.x);
    if (dx > radius)
    {
        return false;
    }

    const float dz = std::abs(p.z - center.z);
    if (dz > radius)
    {
        return false;
    }

    if (dx + dz <= radius)
    {
        return true;
    }

    return (dx * dx + dz * dz <= static_cast<float>(radius) * static_cast<float>(radius));
}

// Pure GetFishingArea preflight: reject mog house.
inline auto RejectFishingAreaInMogHouse(const bool inMogHouse) -> bool
{
    return inMogHouse;
}

// Pure area-type match once geometry results are known (or not needed).
// Returns true if this area entry matches the player location for its type.
inline auto AreaTypeMatches(const std::uint8_t areaType, const bool insideCylinder, const bool insidePoly) -> bool
{
    switch (areaType)
    {
        case AreaTypeWholeZone:
            return true;
        case AreaTypeRadial:
            return insideCylinder;
        case AreaTypePoly:
            return insidePoly;
        default:
            return false;
    }
}

} // namespace fishingareahelpers
