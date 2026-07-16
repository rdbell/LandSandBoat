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

#include <algorithm>
#include <array>

#include "0x01b_job_info.h"

// JOB_INFO's constructor projects selected character state into the packet.
// Entity, A.M.A.N., and Monstrosity ownership remain in the packet handler.
namespace jobinfohelpers
{

constexpr auto JobCount = 24;
constexpr auto StatCount = 7;
constexpr uint8 MonJob = 23; // JOB_MON

struct Facts
{
    uint16                    race{};
    uint8                     mainJob{};
    uint8                     subJob{};
    uint32                    unlockedJobs{};
    std::array<uint8, JobCount> jobLevels{};
    std::array<uint16, StatCount> baseStats{};
    int32                     maxHP{};
    int32                     maxMP{};
    uint16                    equipBlock{};
    uint16                    statsDebilitation{};
    uint8                     canThumbsUpMentor{};
    uint8                     mentorRank{};
    uint8                     masteryRank{};
    bool                      hasMonstrosity{};
};

[[nodiscard]] inline auto PlanFor(const Facts& facts) -> GP_SERV_COMMAND_JOB_INFO::PacketData
{
    auto plan = GP_SERV_COMMAND_JOB_INFO::PacketData{};

    plan.dancer.face_no      = facts.race;
    plan.dancer.mjob_no      = static_cast<JOBTYPE>(facts.hasMonstrosity ? MonJob : facts.mainJob);
    plan.dancer.sjob_no      = static_cast<JOBTYPE>(facts.hasMonstrosity ? MonJob : facts.subJob);
    plan.dancer.get_job_flag = facts.unlockedJobs;
    std::copy_n(facts.jobLevels.begin(), std::size(plan.dancer.job_lev), plan.dancer.job_lev);
    std::copy(facts.baseStats.begin(), facts.baseStats.end(), plan.dancer.bp_base);
    std::copy(facts.jobLevels.begin(), facts.jobLevels.end(), plan.dancer.job_lev2);
    plan.dancer.hpmax   = facts.maxHP;
    plan.dancer.mpmax   = facts.maxMP;
    plan.dancer.sjobflg = static_cast<uint8>(facts.unlockedJobs & 1);

    plan.encumbrance          = facts.equipBlock | (static_cast<uint32>(facts.statsDebilitation) << 16);
    plan.can_thumbs_up_mentor = facts.canThumbsUpMentor;
    plan.mentor_rank          = facts.mentorRank;
    plan.mastery_rank         = facts.masteryRank;
    return plan;
}

} // namespace jobinfohelpers
