#include "test_monstrosity_stored_data_7490.h"

#include "map/monstrosity.h"

#include <iostream>

auto runMonstrosityStoredData7490SelfTests() -> bool
{
    using monstrosity::BuildMonstrosityData;
    using monstrosity::MonstrosityDataRow;
    using monstrosity::SpeciesRuntimeData;

    const auto defaults = BuildMonstrosityData(false, {}, false, {});
    if (defaults.MonstrosityId != 1 || defaults.Species != 1 || defaults.Look != 0x010C || defaults.MainJob != JOB_WAR || defaults.levels[18] != 1 || defaults.instincts[20] != 0x1F)
    {
        std::cerr << "monstrosity stored data: missing row should retain constructor defaults\n";
        return false;
    }
    if (defaults.Flags != 0x0B44)
    {
        std::cerr << "monstrosity stored data: missing row did not preserve default flags\n";
        return false;
    }

    auto row = MonstrosityDataRow{
        .monstrosityId = 43,
        .species       = 99,
        .namePrefix1   = 12,
        .namePrefix2   = 34,
        .currentExp    = 5678,
        .belligerency  = true,
        .entryZoneId   = 777,
        .entryMainJob  = 4,
        .entrySubJob   = 5,
    };
    row.entryPos.x        = 1.5F;
    row.entryPos.y        = -2.5F;
    row.entryPos.z        = 3.5F;
    row.entryPos.rotation = 16;
    row.equippedInstincts[2] = 321;
    row.levels[43]           = 55;
    row.instincts[10]        = 0xAA;
    row.variants[4]          = 0x55;

    const auto species = SpeciesRuntimeData{ .look = 0x4321, .mainJob = JOB_WAR, .subJob = static_cast<JOBTYPE>(2), .size = 2 };
    const auto loaded  = BuildMonstrosityData(true, row, true, species);
    if (loaded.MonstrosityId != 43 || loaded.Species != 99 || loaded.Look != 0x4321 || loaded.MainJob != JOB_WAR || loaded.SubJob != 2 || loaded.Size != 2 || loaded.NamePrefix1 != 12 || loaded.NamePrefix2 != 34 || loaded.CurrentExp != 5678 || loaded.EquippedInstincts[2] != 321 || loaded.levels[43] != 55 || loaded.instincts[10] != 0xAA || loaded.variants[4] != 0x55 || !loaded.Belligerency || loaded.EntryPos.x != 1.5F || loaded.EntryPos.y != -2.5F || loaded.EntryPos.z != 3.5F || loaded.EntryPos.rotation != 16 || loaded.EntryZoneId != 777 || loaded.EntryMainJob != 4 || loaded.EntrySubJob != 5)
    {
        std::cerr << "monstrosity stored data: row decode or species enrichment failed\n";
        return false;
    }
    if (loaded.Flags != 0x0B44)
    {
        std::cerr << "monstrosity stored data: row did not preserve default flags\n";
        return false;
    }

    const auto missingSpecies = BuildMonstrosityData(true, row, false, {});
    if (missingSpecies.Look != 0 || missingSpecies.MainJob != JOB_NON || missingSpecies.SubJob != JOB_NON || missingSpecies.Size != 0 || missingSpecies.MonstrosityId != 43 || missingSpecies.Species != 99)
    {
        std::cerr << "monstrosity stored data: missing species should use zero-value metadata\n";
        return false;
    }

    return true;
}
