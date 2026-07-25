#include "test_char_partial_log_packets_6892.h"

#include "map/char_partial_log_packets.h"

#include <iostream>

auto runCharPartialLogPackets6892SelfTests() -> bool
{
    using Action = partiallogpackethelpers::Action;
    using Packet = partiallogpackethelpers::Packet;
    using Plan   = partiallogpackethelpers::Plan;

    const bool ok =
        partiallogpackethelpers::BuildMissionPlan(MissionLog::Sandoria, false) == Plan{ { { Packet{ Action::Mission, 0 } } }, 1 } &&
        partiallogpackethelpers::BuildMissionPlan(MissionLog::Zilart, true) == Plan{ { { Packet{ Action::MissionComplete, static_cast<uint16>(MissionComplete::Nations) } } }, 1 } &&
        partiallogpackethelpers::BuildMissionPlan(MissionLog::ToAU, false) == Plan{ { { Packet{ Action::QuestOffer, static_cast<uint16>(QuestOffer::AhtUrghan) } } }, 1 } &&
        partiallogpackethelpers::BuildMissionPlan(MissionLog::WoTG, true) == Plan{ { { Packet{ Action::MissionComplete, static_cast<uint16>(MissionComplete::ToAU_WoTG) } } }, 1 } &&
        partiallogpackethelpers::BuildMissionPlan(MissionLog::Assault, true) == Plan{ { { Packet{ Action::QuestComplete, static_cast<uint16>(QuestComplete::AhtUrghan) } } }, 1 } &&
        partiallogpackethelpers::BuildMissionPlan(MissionLog::Campaign, true) == Plan{ { { Packet{ Action::MissionComplete, static_cast<uint16>(MissionComplete::Campaign1) }, Packet{ Action::MissionComplete, static_cast<uint16>(MissionComplete::Campaign2) } } }, 2 } &&
        partiallogpackethelpers::BuildMissionPlan(MissionLog::RoV, true) == Plan{ { { Packet{ Action::Mission, 0 } } }, 1 } &&
        partiallogpackethelpers::BuildMissionPlan(static_cast<MissionLog>(255), false) == Plan{} &&
        partiallogpackethelpers::BuildQuestPlan(QuestLog::Sandoria, false) == Plan{ { { Packet{ Action::QuestOffer, static_cast<uint16>(QuestOffer::Sandoria) } } }, 1 } &&
        partiallogpackethelpers::BuildQuestPlan(QuestLog::Coalition, true) == Plan{ { { Packet{ Action::QuestComplete, static_cast<uint16>(QuestComplete::Coalition) } } }, 1 } &&
        partiallogpackethelpers::BuildQuestPlan(static_cast<QuestLog>(255), false) == Plan{};

    if (!ok)
    {
        std::cerr << "partial mission/quest log packet 6892 self-test failed\n";
    }
    return ok;
}
