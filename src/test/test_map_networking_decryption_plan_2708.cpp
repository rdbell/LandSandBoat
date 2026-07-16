#include "test_map_networking_decryption_plan_2708.h"

#include "map/map_networking_capacity.h"

#include <array>
#include <iostream>

auto runMapNetworkingDecryptionPlan2708SelfTests() -> bool
{
    using mapnetworkinghelpers::IncomingDecryptionPlan;
    struct Case
    {
        bool                   primaryDecrypted;
        bool                   pendingZone;
        bool                   previousDecrypted;
        IncomingDecryptionPlan expected;
    };
    constexpr std::array cases{
        Case{ true, false, false, IncomingDecryptionPlan::UsePrimary },
        Case{ true, true, true, IncomingDecryptionPlan::UsePrimary },
        Case{ false, true, true, IncomingDecryptionPlan::UsePrevious },
        Case{ false, true, false, IncomingDecryptionPlan::Reject },
        Case{ false, false, true, IncomingDecryptionPlan::Reject },
    };
    for (const auto& test : cases)
    {
        if (mapnetworkinghelpers::PlanIncomingDecryption(test.primaryDecrypted, test.pendingZone, test.previousDecrypted) != test.expected)
        {
            std::cerr << "map networking decryption plan 2708 self-test failed\n";
            return false;
        }
    }
    return true;
}
