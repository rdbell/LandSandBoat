#include "test_char_records_of_eminence_packets_6894.h"

#include "map/char_records_of_eminence_packets.h"

#include <iostream>

auto runCharRecordsOfEminencePackets6894SelfTests() -> bool
{
    using Action = eminencepackethelpers::Action;
    using Packet = eminencepackethelpers::Packet;
    using Plan   = eminencepackethelpers::Plan;

    const bool ok =
        eminencepackethelpers::BuildPlan(false, false, 0) == Plan{ { { Packet{ Action::Unity, 0 } } }, 1, false } &&
        eminencepackethelpers::BuildPlan(true, false, 0) == Plan{ { { Packet{ Action::Unity, 0 }, Packet{ Action::ActiveLog, 0 }, Packet{ Action::CompletionLog, 0 }, Packet{ Action::CompletionLog, 1 }, Packet{ Action::CompletionLog, 2 }, Packet{ Action::CompletionLog, 3 } } }, 6, false } &&
        eminencepackethelpers::BuildPlan(true, true, 987) == Plan{ { { Packet{ Action::Unity, 0 }, Packet{ Action::ActiveLog, 0 }, Packet{ Action::TimedRecordMessage, 987 }, Packet{ Action::CompletionLog, 0 }, Packet{ Action::CompletionLog, 1 }, Packet{ Action::CompletionLog, 2 }, Packet{ Action::CompletionLog, 3 } } }, 7, true };
    if (!ok)
    {
        std::cerr << "records of eminence packet 6894 self-test failed\n";
    }
    return ok;
}
