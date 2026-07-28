#include "test_monstrosity_full_update_7495.h"

#include "map/monstrosity.h"

#include <iostream>
#include <vector>

auto runMonstrosityFullUpdate7495SelfTests() -> bool
{
    using monstrosity::FullMonstrosityUpdateAction;
    using monstrosity::PlanFullMonstrosityUpdate;

    const auto absent = PlanFullMonstrosityUpdate(false, 0x1234);
    if (absent.look != 0 || !absent.actions.empty())
    {
        std::cerr << "monstrosity full update: missing data should be a no-op\n";
        return false;
    }

    const auto got = PlanFullMonstrosityUpdate(true, 0x4321);
    const auto want = std::vector<FullMonstrosityUpdateAction>{
        FullMonstrosityUpdateAction::RefreshLook,
        FullMonstrosityUpdateAction::BuildTraits,
        FullMonstrosityUpdateAction::NotifyLua,
        FullMonstrosityUpdateAction::SendMonstrosity1,
        FullMonstrosityUpdateAction::SendMonstrosity2,
        FullMonstrosityUpdateAction::SendJobInfo,
        FullMonstrosityUpdateAction::SendExtendedJobPackets,
        FullMonstrosityUpdateAction::SendGrapList,
        FullMonstrosityUpdateAction::SendCLIStatus,
        FullMonstrosityUpdateAction::SendCommandData,
        FullMonstrosityUpdateAction::MarkLookUpdate,
    };
    if (got.look != 0x4321 || got.actions != want)
    {
        std::cerr << "monstrosity full update: sequence should refresh data then send all client updates\n";
        return false;
    }

    return true;
}
