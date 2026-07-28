#include "test_monstrosity_populate_7489.h"

#include "map/monstrosity.h"

#include <iostream>

auto runMonstrosityPopulate7489SelfTests() -> bool
{
    using monstrosity::PlanPopulateMonstrosityData;

    const auto disabled = PlanPopulateMonstrosityData(false, JOB_MON);
    if (disabled.readData || disabled.writeData)
    {
        std::cerr << "monstrosity populate: disabled plan should be empty\n";
        return false;
    }

    const auto wrongJob = PlanPopulateMonstrosityData(true, JOB_WAR);
    if (wrongJob.readData || wrongJob.writeData)
    {
        std::cerr << "monstrosity populate: non-MON plan should be empty\n";
        return false;
    }

    const auto active = PlanPopulateMonstrosityData(true, JOB_MON);
    if (!active.readData || !active.writeData)
    {
        std::cerr << "monstrosity populate: enabled MON plan should read then write\n";
        return false;
    }

    return true;
}
