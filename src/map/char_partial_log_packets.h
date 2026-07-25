#pragma once

#include "common/cbasetypes.h"

#include "enums/mission_log.h"
#include "enums/quest_log.h"

#include <array>

namespace partiallogpackethelpers
{

enum class Action : uint8
{
    Mission,
    QuestOffer,
    QuestComplete,
    MissionComplete,
};

struct Packet
{
    Action action;
    uint16  value;

    auto operator==(const Packet&) const -> bool = default;
};

struct Plan
{
    std::array<Packet, 2> packets{};
    uint8                 count{};

    auto operator==(const Plan&) const -> bool = default;
};

constexpr auto Mission() -> Plan
{
    Plan plan{};
    plan.packets[0] = { Action::Mission, 0 };
    plan.count      = 1;
    return plan;
}

constexpr auto Other(const Action action, const uint16 value) -> Plan
{
    Plan plan{};
    plan.packets[0] = { action, value };
    plan.count      = 1;
    return plan;
}

constexpr auto TwoMissionCompletions() -> Plan
{
    Plan plan{};
    plan.packets[0] = { Action::MissionComplete, static_cast<uint16>(MissionComplete::Campaign1) };
    plan.packets[1] = { Action::MissionComplete, static_cast<uint16>(MissionComplete::Campaign2) };
    plan.count      = 2;
    return plan;
}

constexpr auto BuildMissionPlan(const MissionLog log, const bool completed) -> Plan
{
    switch (log)
    {
        case MissionLog::Sandoria:
        case MissionLog::Bastok:
        case MissionLog::Windurst:
        case MissionLog::Zilart:
            return completed ? Other(Action::MissionComplete, static_cast<uint16>(MissionComplete::Nations)) : Mission();
        case MissionLog::ToAU:
        case MissionLog::WoTG:
            return completed ? Other(Action::MissionComplete, static_cast<uint16>(MissionComplete::ToAU_WoTG)) : Other(Action::QuestOffer, static_cast<uint16>(QuestOffer::AhtUrghan));
        case MissionLog::Assault:
            return completed ? Other(Action::QuestComplete, static_cast<uint16>(QuestComplete::AhtUrghan)) : Other(Action::QuestOffer, static_cast<uint16>(QuestOffer::AhtUrghan));
        case MissionLog::Campaign:
            return completed ? TwoMissionCompletions() : Other(Action::QuestOffer, static_cast<uint16>(QuestOffer::AhtUrghan));
        case MissionLog::CoP:
        case MissionLog::ACP:
        case MissionLog::AMK:
        case MissionLog::ASA:
        case MissionLog::SoA:
        case MissionLog::RoV:
            return Mission();
        default:
            return {};
    }
}

constexpr auto BuildQuestPlan(const QuestLog log, const bool completed) -> Plan
{
    const auto action = completed ? Action::QuestComplete : Action::QuestOffer;
    switch (log)
    {
        case QuestLog::Sandoria:
            return Other(action, completed ? static_cast<uint16>(QuestComplete::Sandoria) : static_cast<uint16>(QuestOffer::Sandoria));
        case QuestLog::Bastok:
            return Other(action, completed ? static_cast<uint16>(QuestComplete::Bastok) : static_cast<uint16>(QuestOffer::Bastok));
        case QuestLog::Windurst:
            return Other(action, completed ? static_cast<uint16>(QuestComplete::Windurst) : static_cast<uint16>(QuestOffer::Windurst));
        case QuestLog::Jeuno:
            return Other(action, completed ? static_cast<uint16>(QuestComplete::Jeuno) : static_cast<uint16>(QuestOffer::Jeuno));
        case QuestLog::OtherAreas:
            return Other(action, completed ? static_cast<uint16>(QuestComplete::OtherAreas) : static_cast<uint16>(QuestOffer::OtherAreas));
        case QuestLog::Outlands:
            return Other(action, completed ? static_cast<uint16>(QuestComplete::Outlands) : static_cast<uint16>(QuestOffer::Outlands));
        case QuestLog::AhtUrghan:
            return Other(action, completed ? static_cast<uint16>(QuestComplete::AhtUrghan) : static_cast<uint16>(QuestOffer::AhtUrghan));
        case QuestLog::CrystalWar:
            return Other(action, completed ? static_cast<uint16>(QuestComplete::CrystalWar) : static_cast<uint16>(QuestOffer::CrystalWar));
        case QuestLog::Abyssea:
            return Other(action, completed ? static_cast<uint16>(QuestComplete::Abyssea) : static_cast<uint16>(QuestOffer::Abyssea));
        case QuestLog::Adoulin:
            return Other(action, completed ? static_cast<uint16>(QuestComplete::Adoulin) : static_cast<uint16>(QuestOffer::Adoulin));
        case QuestLog::Coalition:
            return Other(action, completed ? static_cast<uint16>(QuestComplete::Coalition) : static_cast<uint16>(QuestOffer::Coalition));
        default:
            return {};
    }
}

} // namespace partiallogpackethelpers
