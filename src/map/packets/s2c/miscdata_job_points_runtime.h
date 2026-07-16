#pragma once

#include "0x063_miscdata_job_points.h"

#include <iterator>

namespace miscdatajobpointshelpers
{

struct Facts
{
    bool                                                           access{};
    GP_SERV_COMMAND_MISCDATA::JOB_POINTS::PacketData::JobPointData jobs[24]{};
};

[[nodiscard]] inline auto PlanFor(const Facts& facts) -> GP_SERV_COMMAND_MISCDATA::JOB_POINTS::PacketData
{
    auto packet      = GP_SERV_COMMAND_MISCDATA::JOB_POINTS::PacketData{};
    packet.type      = GP_SERV_COMMAND_MISCDATA_TYPE::JobPoints;
    packet.unknown06 = sizeof(packet);
    packet.access    = facts.access;
    for (uint8 index = 1; index < std::size(packet.jobs); ++index)
    {
        packet.jobs[index] = facts.jobs[index];
    }
    return packet;
}

} // namespace miscdatajobpointshelpers
