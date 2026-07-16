/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_tracking_transitions.h"

#include <cstdint>
#include <iostream>
#include <optional>
#include <string_view>

#include "map/packets/c2s/tracking_transitions.h"

namespace
{

auto expect(bool condition, std::string_view label) -> bool
{
    if (!condition)
    {
        std::cerr << "c2s tracking transition self-test failed: " << label << '\n';
    }
    return condition;
}

auto target(uint32_t id, uint16_t targid) -> tracking::TargetIdentity
{
    return { .id = id, .targid = targid };
}

auto testListRange() -> bool
{
    return expect(tracking::WideScanRangeFor(0) == 0, "zero list range") &&
           expect(tracking::WideScanRangeFor(75) == 75, "list range preserved") &&
           expect(tracking::WideScanRangeFor(UINT16_MAX) == UINT16_MAX, "maximum list range preserved");
}

auto testStartTarget() -> bool
{
    const auto existing = target(0x01020304, 0x0123);
    const auto requested = target(0xAABBCCDD, 0x0456);

    bool ok = true;
    ok      = expect(!tracking::StartTargetFor(existing, std::nullopt, false, false), "missing target clears existing target") && ok;
    ok      = expect(tracking::StartTargetFor(existing, requested, false, true) == existing, "unscannable target keeps existing target") && ok;
    ok      = expect(tracking::StartTargetFor(existing, requested, true, false) == existing, "out-of-range target keeps existing target") && ok;
    ok      = expect(tracking::StartTargetFor(std::nullopt, requested, true, true) == requested, "eligible target sets identity") && ok;
    ok      = expect(tracking::StartTargetFor(existing, requested, true, true) == requested, "eligible target replaces identity") && ok;
    return ok;
}

auto testEndTarget() -> bool
{
    return expect(!tracking::EndTargetFor(), "end clears target");
}

} // namespace

auto runC2STrackingTransitionSelfTests() -> bool
{
    return testListRange() &&
           testStartTarget() &&
           testEndTarget();
}
