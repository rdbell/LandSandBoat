#include "test_instance_loader_create_7504.h"

#include "map/instance_loader_create.h"

#include <iostream>

auto runInstanceLoaderCreate7504SelfTests() -> bool
{
    bool ok = true;
    const struct { bool present; bool instanced; instanceloader::CreatePlan want; const char* label; } cases[] = {
        { false, false, { false, true }, "missing zone rejects" },
        { true, false, { false, true }, "ordinary zone rejects" },
        { true, true, { true, false }, "instanced zone creates" },
    };
    for (const auto& c : cases)
    {
        if (instanceloader::PlanCreate(c.present, c.instanced) != c.want)
        {
            std::cerr << "instance loader create 7504 self-test failed: " << c.label << '\n';
            ok = false;
        }
    }
    return ok;
}
