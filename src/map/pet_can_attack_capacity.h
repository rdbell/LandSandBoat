#pragma once

namespace petcanattackhelpers
{

template <typename HasPCMaster, typename MasterOwnsTarget, typename BuildAlreadyClaimed,
          typename Disengage, typename BaseCanAttack>
inline auto Apply(
    const bool hasMaster,
    HasPCMaster&& hasPCMaster,
    MasterOwnsTarget&& masterOwnsTarget,
    BuildAlreadyClaimed&& buildAlreadyClaimed,
    Disengage&& disengage,
    BaseCanAttack&& baseCanAttack) -> bool
{
    if (hasMaster && hasPCMaster() && !masterOwnsTarget())
    {
        buildAlreadyClaimed();
        disengage();
        return false;
    }
    return baseCanAttack();
}

} // namespace petcanattackhelpers
