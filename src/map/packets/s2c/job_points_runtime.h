/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#pragma once

#include <array>

#include "0x08d_job_points.h"

namespace jobpointshelpers
{

constexpr auto JobSlots          = 24;
constexpr auto TypesPerJob       = 10;
constexpr auto FullPacketCount   = 12;
constexpr auto PacketPointLimit  = 64;

struct TypeFacts
{
    uint16 id{};
    uint8  value{};
};

using JobsFacts = std::array<std::array<TypeFacts, TypesPerJob>, JobSlots>;

struct FullPlan
{
    std::array<GP_SERV_COMMAND_JOB_POINTS::PacketData, FullPacketCount> packets{};
};

[[nodiscard]] inline auto EntryFor(const TypeFacts facts) -> jobpoint_t
{
    return {
        .index  = static_cast<uint16>(facts.id & 0x1F),
        .job_no = static_cast<uint16>(facts.id >> 5),
        .next   = static_cast<uint16>((facts.value + 1) % 21),
        .level  = static_cast<uint16>(facts.value),
    };
}

[[nodiscard]] inline auto FullPlanFor(const JobsFacts& jobs) -> FullPlan
{
    auto plan       = FullPlan{};
    auto packet     = GP_SERV_COMMAND_JOB_POINTS::PacketData{};
    uint8 pointIndex = 0;
    uint8 packetIndex = 0;

    for (uint8 job = 1; job < JobSlots; ++job)
    {
        for (const auto type : jobs[job])
        {
            if (type.id != 0 && pointIndex < PacketPointLimit)
            {
                packet.points[pointIndex++] = EntryFor(type);
            }
        }

        if (job % 2 == 1)
        {
            plan.packets[packetIndex++] = packet;
            packet                      = {};
            pointIndex                  = 0;
        }
    }
    return plan;
}

[[nodiscard]] inline auto SinglePlanFor(const TypeFacts facts) -> GP_SERV_COMMAND_JOB_POINTS::PacketData
{
    auto packet      = GP_SERV_COMMAND_JOB_POINTS::PacketData{};
    packet.points[0] = EntryFor(facts);
    return packet;
}

} // namespace jobpointshelpers
