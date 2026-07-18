#pragma once
namespace targetfindmobownerhelpers
{
constexpr auto IsMobOwner(const bool casterMasterIsPlayer, const bool targetIsPlayer, const bool ownerMissing, const bool ownerMatchesMaster,
                          const bool targetIsMob, const bool nonExclusiveClaim, const bool ownerInAlliance) -> bool
{
    return !casterMasterIsPlayer || targetIsPlayer || ownerMissing || ownerMatchesMaster || (targetIsMob && nonExclusiveClaim) || ownerInAlliance;
}
} // namespace targetfindmobownerhelpers
