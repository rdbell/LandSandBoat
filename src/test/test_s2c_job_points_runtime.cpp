/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_s2c_job_points_runtime.h"

#include <algorithm>
#include <cstring>
#include <iostream>

#include "map/packets/s2c/job_points_runtime.h"

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "s2c JOB_POINTS runtime self-test failed: " << label << '\n';
    }
    return condition;
}

auto isZero(const GP_SERV_COMMAND_JOB_POINTS::PacketData& packet) -> bool
{
    return std::all_of(std::begin(packet.points), std::end(packet.points), [](const jobpoint_t point) {
        return point.index == 0 && point.job_no == 0 && point.next == 0 && point.level == 0;
    });
}

} // namespace

auto runS2CJobPointsRuntimeSelfTests() -> bool
{
    using namespace jobpointshelpers;

    auto jobs         = JobsFacts{};
    jobs[0][0]        = { .id = 0xFFFF, .value = 9 };
    jobs[1][0]        = { .id = 0, .value = 1 };
    jobs[1][1]        = { .id = 0x23, .value = 20 };
    jobs[2][0]        = { .id = 0x44, .value = 1 };
    jobs[3][0]        = { .id = 0x65, .value = 2 };
    const auto plan   = FullPlanFor(jobs);

    bool ok = true;
    ok      = expect(plan.packets.size() == FullPacketCount, "all odd-job boundaries emit a packet") && ok;
    const auto first = plan.packets[0].points[0];
    ok = expect(first.index == 3 && first.job_no == 1 && first.next == 0 && first.level == 20, "id zero filtering and cost wrap on first odd job") && ok;
    ok = expect(plan.packets[0].points[1].index == 0 && plan.packets[0].points[1].job_no == 0 && plan.packets[0].points[1].next == 0 && plan.packets[0].points[1].level == 0, "first packet tail is zero") && ok;
    const auto secondFirst  = plan.packets[1].points[0];
    const auto secondSecond = plan.packets[1].points[1];
    ok = expect(secondFirst.index == 4 && secondFirst.job_no == 2 && secondFirst.next == 2 && secondFirst.level == 1 && secondSecond.index == 5 && secondSecond.job_no == 3 && secondSecond.next == 3 && secondSecond.level == 2, "even job carries into following odd-job packet") && ok;
    ok = expect(plan.packets[1].points[2].index == 0 && isZero(plan.packets[2]), "packet reset and untouched tails are zero") && ok;

    const auto packed = EntryFor({ .id = 0xFFFF, .value = 20 });
    uint32 packedBytes{};
    std::memcpy(&packedBytes, &packed, sizeof(packedBytes));
    ok = expect(packed.index == 0x1F && packed.job_no == 0x7FF && packed.next == 0 && packed.level == 20 && packedBytes == 0x5000FFFF, "entry bit packing and cost wrap") && ok;

    const auto single = SinglePlanFor({ .id = 0x23, .value = 20 });
    ok = expect(single.points[0].index == 3 && single.points[0].job_no == 1 && single.points[0].next == 0 && single.points[0].level == 20 && single.points[1].index == 0 && sizeof(single) == sizeof(GP_SERV_COMMAND_JOB_POINTS::PacketData), "single update stays full sized with zero tail") && ok;
    return ok;
}
