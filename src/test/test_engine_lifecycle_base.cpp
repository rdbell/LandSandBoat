/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

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

#include "test_engine_lifecycle_base.h"

#include "common/engine.h"

#include <iostream>
#include <string>

namespace
{

struct LifecycleState
{
    bool initialized{};
    bool shutdown{};
};

class ProbeEngine final : public Engine
{
public:
    explicit ProbeEngine(LifecycleState& state)
    : state_(state)
    {
    }

    void onInitialize() override
    {
        state_.initialized = true;
    }

    void onShutdown() override
    {
        state_.shutdown = true;
    }

private:
    LifecycleState& state_;
};

auto expectEqual(const bool actual, const bool expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "Engine lifecycle base self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto testVirtualHooksAndQualifiedDestructorShutdown() -> bool
{
    LifecycleState state;
    bool           ok = true;

    {
        ProbeEngine engine(state);
        engine.onInitialize();
        ok = expectEqual(state.initialized, true, "derived initialize hook") && ok;

        engine.onShutdown();
        ok = expectEqual(state.shutdown, true, "derived explicit shutdown hook") && ok;

        state.shutdown = false;
    }

    // Engine::~Engine explicitly invokes Engine::onShutdown(), so the derived
    // override above is not dispatched during destruction.
    ok = expectEqual(state.shutdown, false, "destructor uses base shutdown hook") && ok;
    return ok;
}

} // namespace

auto runEngineLifecycleBaseSelfTests() -> bool
{
    return testVirtualHooksAndQualifiedDestructorShutdown();
}
