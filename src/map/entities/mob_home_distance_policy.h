#pragma once
namespace mobhomedistancehelpers
{
inline auto IsFarFromHome(float distanceFromHome, float maxRoamDistance) -> bool { return distanceFromHome > maxRoamDistance; }
}
