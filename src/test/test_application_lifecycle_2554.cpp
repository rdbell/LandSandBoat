#include "test_application_lifecycle_2554.h"

#include <iostream>

#include "common/application.h"

auto runApplicationLifecycle2554SelfTests() -> bool
{
    const auto ok = applicationhelpers::IsRunning(false) && !applicationhelpers::IsRunning(true);
    if (!ok)
    {
        std::cerr << "application lifecycle 2554 self-test failed\n";
    }
    return ok;
}
