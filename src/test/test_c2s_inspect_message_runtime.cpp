/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_inspect_message_runtime.h"

#include <cstring>
#include <iostream>
#include <string>
#include <string_view>

#include "map/packets/c2s/inspect_message_runtime.h"

namespace
{

auto expect(const bool condition, const std::string_view label) -> bool
{
    if (!condition)
    {
        std::cerr << "c2s INSPECT_MESSAGE runtime self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto testBoundedMessageConversion() -> bool
{
    uint8_t exactly120[123] = {};
    std::memset(exactly120, 'a', 120);

    uint8_t overlong[123] = {};
    std::memset(overlong, 'b', sizeof(overlong));

    const uint8_t embeddedNul[123] = { 'o', 'm', 'e', 'g', 'a', '\0', 'x', 'i' };

    return expect(inspectmessage::BazaarMessageFrom(exactly120) == std::string(120, 'a'), "retains exactly 120 bytes") &&
           expect(inspectmessage::BazaarMessageFrom(overlong) == std::string(120, 'b'), "caps unterminated field at 120 bytes") &&
           expect(inspectmessage::BazaarMessageFrom(embeddedNul) == "omega", "stops at embedded NUL");
}

auto testSuccessGatedBazaarMessageUpdate() -> bool
{
    const uint8_t rawMessage[123] = { 'n', 'e', 'w', '\0', 'i', 'g', 'n', 'o', 'r', 'e', 'd' };
    const auto    failed          = inspectmessage::TransitionFor(rawMessage, "old", false);
    const auto    succeeded       = inspectmessage::TransitionFor(rawMessage, "old", true);

    return expect(failed.preparedMessage == "new", "failed update preserves prepared value") &&
           expect(failed.bazaarMessage == "old", "failed update retains in-memory message") &&
           expect(!failed.bazaarMessageUpdated, "failed update is not marked updated") &&
           expect(succeeded.preparedMessage == "new", "successful update preserves prepared value") &&
           expect(succeeded.bazaarMessage == "new", "successful update changes in-memory message") &&
           expect(succeeded.bazaarMessageUpdated, "successful update is marked updated");
}

} // namespace

auto runC2SInspectMessageRuntimeSelfTests() -> bool
{
    return testBoundedMessageConversion() && testSuccessGatedBazaarMessageUpdate();
}
