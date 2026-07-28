#include "test_instance_loader_finalize_7507.h"

#include "map/instance_loader_finalize.h"

#include <iostream>

auto runInstanceLoaderFinalize7507SelfTests() -> bool
{
    using namespace instanceloader;

    bool ok = true;
    if (MobFinalizationOrder != std::array{
                                    MobFinalizationAction::Initialize,
                                    MobFinalizationAction::FindParty,
                                    MobFinalizationAction::ApplyMixins,
                                    MobFinalizationAction::SaveModifiers,
                                    MobFinalizationAction::SaveMobModifiers,
                                    MobFinalizationAction::CacheScript })
    {
        std::cerr << "instance loader finalize 7507 self-test failed: mob order\n";
        ok = false;
    }
    if (NpcFinalizationOrder != std::array{
                                    NpcFinalizationAction::Spawn,
                                    NpcFinalizationAction::CacheScript })
    {
        std::cerr << "instance loader finalize 7507 self-test failed: npc order\n";
        ok = false;
    }
    if (InstanceFinalizationOrder != std::array{
                                         InstanceFinalizationAction::CacheScript,
                                         InstanceFinalizationAction::CreatedCallback,
                                         InstanceFinalizationAction::Created })
    {
        std::cerr << "instance loader finalize 7507 self-test failed: instance order\n";
        ok = false;
    }
    return ok;
}
