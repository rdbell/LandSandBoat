#include "test_instance_loader_load_7506.h"

#include "map/instance_loader_load.h"

#include <iostream>

auto runInstanceLoaderLoad7506SelfTests() -> bool
{
    bool ok = true;
    if (instanceloader::PlanLoad(false) != instanceloader::LoadPlan{ true, true })
    {
        std::cerr << "instance loader load 7506 self-test failed: healthy instance loads and finalizes\n";
        ok = false;
    }
    if (instanceloader::PlanLoad(true) != instanceloader::LoadPlan{})
    {
        std::cerr << "instance loader load 7506 self-test failed: failed instance skips all work\n";
        ok = false;
    }
    return ok;
}
