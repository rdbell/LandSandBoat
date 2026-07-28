#include "test_monstrosity_stored_data_write_7491.h"

#include "map/monstrosity.h"

#include <iostream>

auto runMonstrosityStoredDataWrite7491SelfTests() -> bool
{
    using monstrosity::MonstrosityData_t;
    using monstrosity::PlanMonstrosityDataWrite;

    if (PlanMonstrosityDataWrite(false, 1234, {}).write)
    {
        std::cerr << "monstrosity stored-data write: absent data should not persist\n";
        return false;
    }

    auto data                = MonstrosityData_t{};
    data.MonstrosityId       = 43;
    data.Species             = 99;
    data.NamePrefix1         = 12;
    data.NamePrefix2         = 34;
    data.CurrentExp          = 5678;
    data.EquippedInstincts[2] = 321;
    data.levels[43]          = 55;
    data.instincts[10]       = 0xAA;
    data.variants[4]         = 0x55;
    data.Belligerency        = true;
    data.EntryPos.x          = 1.5F;
    data.EntryPos.y          = -2.5F;
    data.EntryPos.z          = 3.5F;
    data.EntryPos.rotation   = 16;
    data.EntryZoneId         = 777;
    data.EntryMainJob        = 4;
    data.EntrySubJob         = 5;

    const auto plan = PlanMonstrosityDataWrite(true, 1234, data);
    const auto& out = plan.values;
    if (!plan.write || out.charId != 1234 || out.monstrosityId != 43 || out.species != 99 || out.namePrefix1 != 12 || out.namePrefix2 != 34 || out.currentExp != 5678 || out.equippedInstincts[2] != 321 || out.levels[43] != 55 || out.instincts[10] != 0xAA || out.variants[4] != 0x55 || out.belligerency != 1 || out.entryPos.x != 1.5F || out.entryPos.y != -2.5F || out.entryPos.z != 3.5F || out.entryPos.rotation != 16 || out.entryZoneId != 777 || out.entryMainJob != 4 || out.entrySubJob != 5)
    {
        std::cerr << "monstrosity stored-data write: persisted values differ from data\n";
        return false;
    }

    data.Belligerency = false;
    if (PlanMonstrosityDataWrite(true, 1234, data).values.belligerency != 0)
    {
        std::cerr << "monstrosity stored-data write: false belligerency should bind as zero\n";
        return false;
    }

    return true;
}
