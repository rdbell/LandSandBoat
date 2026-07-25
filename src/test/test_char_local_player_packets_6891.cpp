#include "test_char_local_player_packets_6891.h"

#include "map/char_local_player_packets.h"

#include <iostream>

auto runCharLocalPlayerPackets6891SelfTests() -> bool
{
    using Action = localplayerpackethelpers::Action;
    const bool ok = localplayerpackethelpers::BuildPlan() == std::array{
                                                               Action::GroupAttributes,
                                                               Action::CliStatus,
                                                               Action::CliStatus2,
                                                               Action::AbilityRecast,
                                                               Action::Merits,
                                                               Action::Monstrosity,
                                                               Action::JobPoints,
                                                           };
    if (!ok)
    {
        std::cerr << "local player packet refresh 6891 self-test failed\n";
    }
    return ok;
}
