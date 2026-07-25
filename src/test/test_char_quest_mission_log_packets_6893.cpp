#include "test_char_quest_mission_log_packets_6893.h"

#include "map/char_mission_packet_plans.h"

#include <iostream>

auto runCharQuestMissionLogPackets6893SelfTests() -> bool
{
    using Action = missionpackethelpers::Action;
    using Packet = missionpackethelpers::Packet;

    const bool ok = missionpackethelpers::BuildQuestMissionLogPlan() == std::array{
                                                                Packet{ Action::QuestOffer, static_cast<uint16>(QuestOffer::Sandoria) },
                                                                Packet{ Action::QuestOffer, static_cast<uint16>(QuestOffer::Bastok) },
                                                                Packet{ Action::QuestOffer, static_cast<uint16>(QuestOffer::Windurst) },
                                                                Packet{ Action::QuestOffer, static_cast<uint16>(QuestOffer::Jeuno) },
                                                                Packet{ Action::QuestOffer, static_cast<uint16>(QuestOffer::OtherAreas) },
                                                                Packet{ Action::QuestOffer, static_cast<uint16>(QuestOffer::Outlands) },
                                                                Packet{ Action::QuestOffer, static_cast<uint16>(QuestOffer::AhtUrghan) },
                                                                Packet{ Action::QuestOffer, static_cast<uint16>(QuestOffer::CrystalWar) },
                                                                Packet{ Action::QuestComplete, static_cast<uint16>(QuestComplete::Sandoria) },
                                                                Packet{ Action::QuestComplete, static_cast<uint16>(QuestComplete::Bastok) },
                                                                Packet{ Action::QuestComplete, static_cast<uint16>(QuestComplete::Windurst) },
                                                                Packet{ Action::QuestComplete, static_cast<uint16>(QuestComplete::Jeuno) },
                                                                Packet{ Action::QuestComplete, static_cast<uint16>(QuestComplete::OtherAreas) },
                                                                Packet{ Action::QuestComplete, static_cast<uint16>(QuestComplete::Outlands) },
                                                                Packet{ Action::QuestComplete, static_cast<uint16>(QuestComplete::AhtUrghan) },
                                                                Packet{ Action::QuestComplete, static_cast<uint16>(QuestComplete::CrystalWar) },
                                                                Packet{ Action::MissionComplete, static_cast<uint16>(MissionComplete::Nations) },
                                                                Packet{ Action::MissionComplete, static_cast<uint16>(MissionComplete::ToAU_WoTG) },
                                                                Packet{ Action::MissionComplete, static_cast<uint16>(MissionComplete::Campaign1) },
                                                                Packet{ Action::MissionComplete, static_cast<uint16>(MissionComplete::Campaign2) },
                                                                Packet{ Action::QuestOffer, static_cast<uint16>(QuestOffer::Abyssea) },
                                                                Packet{ Action::QuestComplete, static_cast<uint16>(QuestComplete::Abyssea) },
                                                                Packet{ Action::QuestOffer, static_cast<uint16>(QuestOffer::Adoulin) },
                                                                Packet{ Action::QuestComplete, static_cast<uint16>(QuestComplete::Adoulin) },
                                                                Packet{ Action::QuestOffer, static_cast<uint16>(QuestOffer::Coalition) },
                                                                Packet{ Action::QuestComplete, static_cast<uint16>(QuestComplete::Coalition) },
                                                                Packet{ Action::Mission, 0 },
                                                                Packet{ Action::TVR, 0 },
                                                            };
    if (!ok)
    {
        std::cerr << "quest mission log packet 6893 self-test failed\n";
    }
    return ok;
}
