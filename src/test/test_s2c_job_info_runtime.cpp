/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_s2c_job_info_runtime.h"

#include <iostream>

#include "map/packets/s2c/job_info_runtime.h"

auto runS2CJobInfoRuntimeSelfTests() -> bool
{
    auto facts = jobinfohelpers::Facts{
        .race              = 7,
        .mainJob           = 5,
        .subJob            = 6,
        .unlockedJobs      = 0xAABBCCDD,
        .maxHP             = 1234,
        .maxMP             = 567,
        .equipBlock        = 0x1122,
        .statsDebilitation = 0x3344,
        .canThumbsUpMentor = 1,
        .mentorRank        = 2,
        .masteryRank       = 3,
    };
    for (std::size_t i = 0; i < facts.jobLevels.size(); ++i)
    {
        facts.jobLevels[i] = static_cast<uint8>(i + 1);
    }
    for (std::size_t i = 0; i < facts.baseStats.size(); ++i)
    {
        facts.baseStats[i] = static_cast<uint16>(100 + i);
    }

    const auto plan = jobinfohelpers::PlanFor(facts);
    bool ok = plan.dancer.mon_no == 0 && plan.dancer.face_no == 7 && plan.dancer.mjob_no == static_cast<JOBTYPE>(5) && plan.dancer.hair_no == 0 && plan.dancer.size == 0 && plan.dancer.sjob_no == static_cast<JOBTYPE>(6) &&
              plan.dancer.get_job_flag == 0xAABBCCDD && plan.dancer.hpmax == 1234 && plan.dancer.mpmax == 567 && plan.dancer.sjobflg == 1 &&
              plan.encumbrance == 0x33441122 && plan.can_thumbs_up_mentor == 1 && plan.mentor_rank == 2 && plan.mastery_rank == 3 &&
              plan.dancer.unknown41[0] == 0 && plan.dancer.unknown41[1] == 0 && plan.dancer.unknown41[2] == 0 && plan.padding67 == 0 && plan.job_mastery_flags == 0;
    for (std::size_t i = 0; i < facts.jobLevels.size(); ++i)
    {
        ok = plan.dancer.job_lev2[i] == facts.jobLevels[i] && ok;
        if (i < std::size(plan.dancer.job_lev))
        {
            ok = plan.dancer.job_lev[i] == static_cast<int8>(facts.jobLevels[i]) && ok;
        }
    }
    for (std::size_t i = 0; i < facts.baseStats.size(); ++i)
    {
        ok = plan.dancer.bp_base[i] == facts.baseStats[i] && ok;
    }
    for (const auto adjustment : plan.dancer.bp_adj)
    {
        ok = adjustment == 0 && ok;
    }
    for (const auto level : plan.job_mastery_levels)
    {
        ok = level == 0 && ok;
    }

    facts.hasMonstrosity = true;
    const auto monstrosityPlan = jobinfohelpers::PlanFor(facts);
    ok = monstrosityPlan.dancer.mjob_no == static_cast<JOBTYPE>(jobinfohelpers::MonJob) &&
         monstrosityPlan.dancer.sjob_no == static_cast<JOBTYPE>(jobinfohelpers::MonJob) && ok;
    if (!ok)
    {
        std::cerr << "s2c JOB_INFO runtime self-test failed\n";
    }
    return ok;
}
