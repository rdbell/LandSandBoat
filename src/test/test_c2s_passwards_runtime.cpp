/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_passwards_runtime.h"

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "map/packets/c2s/passwards_runtime.h"

namespace
{

auto expect(const bool condition, const std::string_view label) -> bool
{
    if (!condition)
    {
        std::cerr << "c2s PASSWARDS runtime self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto testFixedWidthStringConversion() -> bool
{
    const uint8_t embeddedNul[16] = { 'o', 'm', 'e', 'g', 'a', '\0', 'x', 'i', 0xFF, 'x', 'x', 'x', 'x', 'x', 'x', 'x' };
    const uint8_t highBytes[16]   = { 0xFF, 'A', '\0' };
    const uint8_t unterminated[16] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
    };

    auto expectedHighBytes = std::string{};
    expectedHighBytes.push_back(static_cast<char>(0xFF));
    expectedHighBytes.push_back('A');

    return expect(passwards::UpdateStringFrom(embeddedNul) == "omega", "stops at embedded NUL") &&
           expect(passwards::UpdateStringFrom(highBytes) == expectedHighBytes, "preserves non-ASCII bytes") &&
           expect(passwards::UpdateStringFrom(unterminated) == "0123456789ABCDEF", "caps unterminated field at 16 bytes");
}

auto testCallbackAndResponseOrdering() -> bool
{
    const uint8_t rawString[16] = { 'p', 'a', 's', 's', '\0', 'i', 'g', 'n', 'o', 'r', 'e', 'd' };
    auto           calls        = std::vector<std::string>{};

    passwards::Run(rawString,
                   {
                       .onEventUpdate = [&calls](const std::string& value) { calls.emplace_back("update:" + value); },
                       .sendEventUCOff = [&calls](const GP_SERV_COMMAND_EVENTUCOFF_MODE mode) {
                           calls.emplace_back("eventucoff:" + std::to_string(static_cast<uint32_t>(mode)));
                       },
                   });

    return expect(calls == std::vector<std::string>{ "update:pass", "eventucoff:1", "eventucoff:3" }, "OnEventUpdate then EventRecvPending then CancelInput");
}

} // namespace

auto runC2SPasswardsRuntimeSelfTests() -> bool
{
    return testFixedWidthStringConversion() && testCallbackAndResponseOrdering();
}
