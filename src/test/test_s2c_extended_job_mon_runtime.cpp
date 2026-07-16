/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_s2c_extended_job_mon_runtime.h"

#include <iostream>

#include "map/packets/s2c/0x044_extended_job_mon.h"

namespace
{

auto expectPlan(const extendedjobmonhelpers::Plan actual, const extendedjobmonhelpers::Plan expected, const char* label) -> bool
{
    if (actual.job != expected.job || actual.species != expected.species || actual.equippedInstincts != expected.equippedInstincts)
    {
        std::cerr << "s2c EXTENDED_JOB MON runtime self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runS2CExtendedJobMonRuntimeSelfTests() -> bool
{
    using namespace extendedjobmonhelpers;

    bool ok = true;
    ok      = expectPlan(PlanFor({}), { .job = JobMon }, "no monstrosity keeps zero payload with JOB_MON") && ok;

    auto facts              = Facts{};
    facts.hasMonstrosity    = true;
    facts.species           = 0x1234;
    facts.equippedInstincts = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
    ok                      = expectPlan(PlanFor(facts), { .job = JobMon, .species = 0x1234, .equippedInstincts = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 } }, "monstrosity copies species and all instincts") && ok;
    return ok;
}
