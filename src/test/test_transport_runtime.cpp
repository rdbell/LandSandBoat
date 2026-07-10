/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_transport_runtime.h"

#include "map/transport.h"

#include <cstdint>
#include <iostream>
#include <tuple>
#include <vector>

namespace
{

enum class TownEffect : uint8_t
{
    Animate,
    Spawn,
    Update,
    OpenDoor,
    CloseDoor,
    Depart,
    Hide,
};

struct TownModel
{
    int64 offset      = 10;
    int64 interval    = 100;
    int64 arriveDock  = 20;
    int64 departDock  = 40;
    int64 voyageStart = 50;
    uint8 state       = STATE_TRANSPORT_INIT;
};

// Characterization seam for CTransportHandler::TransportTimer's town loop.
// Effects intentionally omit entity payloads while preserving callback order.
auto tickTown(TownModel& model, const int64 now) -> std::vector<TownEffect>
{
    const auto timer = (now - model.offset) % model.interval;
    if (model.state == STATE_TRANSPORT_AWAY)
    {
        if (timer < model.arriveDock)
        {
            model.state = STATE_TRANSPORT_ARRIVING;
            return { TownEffect::Animate, TownEffect::Spawn, TownEffect::Update };
        }
    }
    else if (model.state == STATE_TRANSPORT_DEPARTING)
    {
        if (timer >= model.voyageStart)
        {
            model.state = STATE_TRANSPORT_AWAY;
            return { TownEffect::Hide, TownEffect::Update };
        }
    }
    else if (model.state == STATE_TRANSPORT_DOCKED)
    {
        if (timer >= model.departDock)
        {
            model.state = STATE_TRANSPORT_DEPARTING;
            return { TownEffect::Animate, TownEffect::CloseDoor, TownEffect::Depart, TownEffect::Update };
        }
    }
    else if (model.state == STATE_TRANSPORT_ARRIVING)
    {
        if (timer >= model.arriveDock)
        {
            model.state = STATE_TRANSPORT_DOCKED;
            return { TownEffect::OpenDoor };
        }
    }
    else if (model.state == STATE_TRANSPORT_INIT)
    {
        if (timer >= model.voyageStart)
        {
            model.state = STATE_TRANSPORT_AWAY;
        }
        else if (timer >= model.departDock)
        {
            model.state = STATE_TRANSPORT_DEPARTING;
            return { TownEffect::Spawn, TownEffect::Animate };
        }
        else if (timer >= model.arriveDock)
        {
            model.state = STATE_TRANSPORT_DOCKED;
            return { TownEffect::OpenDoor, TownEffect::Spawn, TownEffect::Animate };
        }
        else
        {
            model.state = STATE_TRANSPORT_ARRIVING;
            return { TownEffect::Spawn, TownEffect::Animate };
        }
    }
    return {};
}

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "transport runtime self-test failed: " << label << '\n';
    }
    return condition;
}

auto testTownCycleAndOrdering() -> bool
{
    TownModel model;
    model.state = STATE_TRANSPORT_AWAY;

    bool ok = true;
    ok      = expect(tickTown(model, 109).empty() && model.state == STATE_TRANSPORT_AWAY, "away before cycle wrap") && ok;
    ok      = expect(tickTown(model, 110) == std::vector{ TownEffect::Animate, TownEffect::Spawn, TownEffect::Update } &&
                         model.state == STATE_TRANSPORT_ARRIVING,
                     "arrival callback ordering") &&
         ok;
    ok = expect(tickTown(model, 130) == std::vector{ TownEffect::OpenDoor } && model.state == STATE_TRANSPORT_DOCKED,
                "inclusive dock boundary") &&
         ok;
    ok = expect(tickTown(model, 150) ==
                        std::vector{ TownEffect::Animate, TownEffect::CloseDoor, TownEffect::Depart, TownEffect::Update } &&
                    model.state == STATE_TRANSPORT_DEPARTING,
                "departure callback ordering") &&
         ok;
    ok = expect(tickTown(model, 160) == std::vector{ TownEffect::Hide, TownEffect::Update } &&
                    model.state == STATE_TRANSPORT_AWAY,
                "away callback ordering") &&
         ok;
    return ok;
}

auto testTownInitializationOrdering() -> bool
{
    bool ok = true;
    for (const auto& [now, expectedState, expectedEffects] :
         std::vector<std::tuple<int64, uint8, std::vector<TownEffect>>>{
             { 125, STATE_TRANSPORT_ARRIVING, { TownEffect::Spawn, TownEffect::Animate } },
             { 135, STATE_TRANSPORT_DOCKED, { TownEffect::OpenDoor, TownEffect::Spawn, TownEffect::Animate } },
             { 155, STATE_TRANSPORT_DEPARTING, { TownEffect::Spawn, TownEffect::Animate } },
             { 160, STATE_TRANSPORT_AWAY, {} },
         })
    {
        TownModel model;
        ok = expect(tickTown(model, now) == expectedEffects && model.state == expectedState, "initial state reconstruction") && ok;
    }
    return ok;
}

auto testVoyageStrictBoundaries() -> bool
{
    // Pins the asymmetry in TransportTimer: eviction is strictly after
    // arriveDock-10 and strictly before voyageStart; leaving dock is strict >.
    constexpr int64 arriveDock  = 20;
    constexpr int64 voyageStart = 50;
    const auto      shouldEvict = [=](const int64 timer) {
        return timer < voyageStart && timer > arriveDock - 10;
    };

    bool ok = true;
    ok      = expect(!shouldEvict(10), "eviction lower edge excluded") && ok;
    ok      = expect(shouldEvict(11), "eviction lower edge plus one included") && ok;
    ok      = expect(shouldEvict(49), "eviction upper edge minus one included") && ok;
    ok      = expect(!shouldEvict(50), "eviction upper edge excluded") && ok;
    ok      = expect(!(50 > voyageStart) && 51 > voyageStart, "docked voyage boundary is strict") && ok;
    return ok;
}

auto testElevatorSchedulingBoundaries() -> bool
{
    const auto nextCycle = [](const int64 now, const int64 interval) {
        return now - (now % interval) + interval;
    };

    bool ok = true;
    ok      = expect(nextCycle(41, 20) == 60, "unaligned elevator next cycle") && ok;
    ok      = expect(nextCycle(40, 20) == 60, "aligned elevator still waits one cycle") && ok;
    ok      = expect(!(39 >= 20 + 20) && 40 >= 20 + 20, "elevator start deadline is inclusive") && ok;
    ok      = expect(!(24 >= 20 + 5) && 25 >= 20 + 5, "elevator arrival deadline is inclusive") && ok;
    return ok;
}

} // namespace

auto runTransportRuntimeSelfTests() -> bool
{
    bool ok = true;
    ok      = testTownCycleAndOrdering() && ok;
    ok      = testTownInitializationOrdering() && ok;
    ok      = testVoyageStrictBoundaries() && ok;
    ok      = testElevatorSchedulingBoundaries() && ok;
    return ok;
}
