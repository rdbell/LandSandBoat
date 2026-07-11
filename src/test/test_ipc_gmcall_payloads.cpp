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

#include "test_ipc_gmcall_payloads.h"

#include "common/ipc.h"

#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace
{

auto expectEqualInt(int actual, int expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "IPC GM call payload self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualFloat(float actual, float expected, const std::string& label) -> bool
{
    if (std::fabs(actual - expected) > 0.00001f)
    {
        std::cerr << "IPC GM call payload self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(const std::string& actual, const std::string& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "IPC GM call payload self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualBytes(const std::vector<uint8>& actual, const std::vector<uint8>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "IPC GM call payload self-test failed: " << label << " got";
        for (const auto byte : actual)
        {
            std::cerr << " " << static_cast<int>(byte);
        }
        std::cerr << " expected";
        for (const auto byte : expected)
        {
            std::cerr << " " << static_cast<int>(byte);
        }
        std::cerr << '\n';
        return false;
    }
    return true;
}

auto testDefaultPayloads() -> bool
{
    bool ok = true;

    const ipc::GMCallRequest request{};
    ok = expectEqualInt(request.callId, 0, "request call id default") && ok;
    ok = expectEqualInt(request.charId, 0, "request char id default") && ok;
    ok = expectEqualString(request.charName, "", "request char name default") && ok;
    ok = expectEqualInt(request.accId, 0, "request account id default") && ok;
    ok = expectEqualInt(request.zoneId, 0, "request zone id default") && ok;
    ok = expectEqualFloat(request.posX, 0.0f, "request pos x default") && ok;
    ok = expectEqualFloat(request.posY, 0.0f, "request pos y default") && ok;
    ok = expectEqualFloat(request.posZ, 0.0f, "request pos z default") && ok;
    ok = expectEqualString(request.message, "", "request message default") && ok;
    ok = expectEqualInt(static_cast<int>(request.parameters.size()), 0, "request parameter count default") && ok;

    const ipc::GMCallResponse response{};
    ok = expectEqualInt(response.callId, 0, "response call id default") && ok;
    ok = expectEqualInt(response.charId, 0, "response char id default") && ok;
    ok = expectEqualString(response.message, "", "response message default") && ok;

    return ok;
}

auto testAssignedPayloads() -> bool
{
    bool ok = true;

    const ipc::GMCallRequest request{
        .callId     = 1001,
        .charId     = 2002,
        .charName   = "Omega",
        .accId      = 3003,
        .zoneId     = 230,
        .posX       = 1.25f,
        .posY       = -2.5f,
        .posZ       = 3.75f,
        .message    = "Need help",
        .parameters = { { "category", "stuck" }, { "area", "Mog House" } },
    };
    ok = expectEqualInt(request.callId, 1001, "assigned request call id") && ok;
    ok = expectEqualInt(request.charId, 2002, "assigned request char id") && ok;
    ok = expectEqualString(request.charName, "Omega", "assigned request char name") && ok;
    ok = expectEqualInt(request.accId, 3003, "assigned request account id") && ok;
    ok = expectEqualInt(request.zoneId, 230, "assigned request zone id") && ok;
    ok = expectEqualFloat(request.posX, 1.25f, "assigned request pos x") && ok;
    ok = expectEqualFloat(request.posY, -2.5f, "assigned request pos y") && ok;
    ok = expectEqualFloat(request.posZ, 3.75f, "assigned request pos z") && ok;
    ok = expectEqualString(request.message, "Need help", "assigned request message") && ok;
    ok = expectEqualString(request.parameters.at("category"), "stuck", "assigned request category") && ok;
    ok = expectEqualString(request.parameters.at("area"), "Mog House", "assigned request area") && ok;

    auto copiedRequest = request;
    copiedRequest.parameters["category"] = "changed";
    copiedRequest.parameters["extra"]    = "ignored";
    ok = expectEqualString(request.parameters.at("category"), "stuck", "copied request owns parameters") && ok;
    ok = expectEqualInt(static_cast<int>(request.parameters.size()), 2, "copied request original parameter count") && ok;

    const ipc::GMCallResponse response{ .callId = 1001, .charId = 2002, .message = "On the way" };
    ok = expectEqualInt(response.callId, 1001, "assigned response call id") && ok;
    ok = expectEqualInt(response.charId, 2002, "assigned response char id") && ok;
    ok = expectEqualString(response.message, "On the way", "assigned response message") && ok;

    return ok;
}

auto testWireEncoding() -> bool
{
    bool ok = true;

    const ipc::GMCallRequest request{
        .callId     = 1001,
        .charId     = 2002,
        .charName   = "Omega",
        .accId      = 3003,
        .zoneId     = 230,
        .posX       = 1.25f,
        .posY       = -2.5f,
        .posZ       = 3.75f,
        .message    = "Need help",
        .parameters = { { "category", "stuck" }, { "area", "Mog House" } },
    };
    const std::vector<uint8> expectedRequest{
        36,
        0xE9, 0x07,
        0xD2, 0x0F,
        0x05, 'O', 'm', 'e', 'g', 'a',
        0xBB, 0x17,
        0xE6, 0x00,
        0x00, 0x00, 0xA0, 0x3F,
        0x00, 0x00, 0x20, 0xC0,
        0x00, 0x00, 0x70, 0x40,
        0x09, 'N', 'e', 'e', 'd', ' ', 'h', 'e', 'l', 'p',
        0x02,
        0x04, 'a', 'r', 'e', 'a',
        0x09, 'M', 'o', 'g', ' ', 'H', 'o', 'u', 's', 'e',
        0x08, 'c', 'a', 't', 'e', 'g', 'o', 'r', 'y',
        0x05, 's', 't', 'u', 'c', 'k',
    };
    ok = expectEqualBytes(ipc::toBytesWithHeader(request), expectedRequest, "request wire bytes") && ok;

    const ipc::GMCallResponse response{ .callId = 1001, .charId = 2002, .message = "On the way" };
    const std::vector<uint8> expectedResponse{
        37,
        0xE9, 0x07,
        0xD2, 0x0F,
        0x0A, 'O', 'n', ' ', 't', 'h', 'e', ' ', 'w', 'a', 'y',
    };
    ok = expectEqualBytes(ipc::toBytesWithHeader(response), expectedResponse, "response wire bytes") && ok;

    return ok;
}
} // namespace

auto runIPCGMCallPayloadSelfTests() -> bool
{
    return testDefaultPayloads() && testAssignedPayloads() && testWireEncoding();
}
