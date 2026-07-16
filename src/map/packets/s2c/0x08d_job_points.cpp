/*
===========================================================================

  Copyright (c) 2025 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "0x08d_job_points.h"
#include "job_points_runtime.h"

#include "entities/char_entity.h"
#include "job_points.h"

// Constructor for full job point details
GP_SERV_COMMAND_JOB_POINTS::GP_SERV_COMMAND_JOB_POINTS(CCharEntity* PChar)
{
    const JobPoints_t* PJobPoints = PChar->PJobPoints->GetAllJobPoints();
    if (!PJobPoints)
    {
        return;
    }

    auto jobs = jobpointshelpers::JobsFacts{};
    for (uint8 job = 1; job < MAX_JOBTYPE; ++job)
    {
        for (uint8 type = 0; type < JOBPOINTS_JPTYPE_PER_CATEGORY; ++type)
        {
            const auto currentType = PJobPoints[job].job_point_types[type];
            jobs[job][type]        = { .id = currentType.id, .value = currentType.value };
        }
    }

    const auto plan = jobpointshelpers::FullPlanFor(jobs);
    for (const auto& packet : plan.packets)
    {
        this->data() = packet;
        PChar->pushPacket(this->copy());
    }

    this->data() = {};
}

// Constructor for single job point update
GP_SERV_COMMAND_JOB_POINTS::GP_SERV_COMMAND_JOB_POINTS(const CCharEntity* PChar, const JOBPOINT_TYPE jpType)
{
    const JobPointType_t* PJobPoint = PChar->PJobPoints->GetJobPointType(jpType);
    this->data() = jobpointshelpers::SinglePlanFor({ .id = PJobPoint->id, .value = PJobPoint->value });
}
