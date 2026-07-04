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

#include "test_ipc_session_payloads.h"

#include "common/ipc_structs.h"

#include <cmath>
#include <iostream>
#include <string>

namespace
{

auto expectEqualInt(int actual, int expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "IPC session payload self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualUInt(uint32_t actual, uint32_t expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "IPC session payload self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualFloat(float actual, float expected, const std::string& label) -> bool
{
    if (std::fabs(actual - expected) > 0.00001f)
    {
        std::cerr << "IPC session payload self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(const std::string& actual, const std::string& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "IPC session payload self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualBool(bool actual, bool expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "IPC session payload self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testDefaultPayloads() -> bool
{
    bool ok = true;

    ok = expectEqualUInt(ipc::AccountLogin{}.accountId, 0, "account id default") && ok;
    ok = expectEqualUInt(ipc::CharZone{}.charId, 0, "char zone char id default") && ok;
    ok = expectEqualInt(ipc::CharZone{}.destinationZoneId, 0, "destination zone default") && ok;

    const ipc::CharVarUpdate varUpdate{};
    ok = expectEqualUInt(varUpdate.charId, 0, "char var char id default") && ok;
    ok = expectEqualInt(varUpdate.value, 0, "char var value default") && ok;
    ok = expectEqualUInt(varUpdate.expiry, 0, "char var expiry default") && ok;
    ok = expectEqualString(varUpdate.varName, "", "char var name default") && ok;

    const ipc::LuaFunction luaFunction{};
    ok = expectEqualInt(luaFunction.requesterZoneId, 0, "lua requester zone default") && ok;
    ok = expectEqualInt(luaFunction.executorZoneId, 0, "lua executor zone default") && ok;
    ok = expectEqualString(luaFunction.funcString, "", "lua function string default") && ok;
    ok = expectEqualUInt(ipc::KillSession{}.victimId, 0, "kill session victim default") && ok;

    const ipc::EntityInformationRequest request{};
    ok = expectEqualUInt(request.requesterId, 0, "entity request requester default") && ok;
    ok = expectEqualUInt(request.targetId, 0, "entity request target default") && ok;
    ok = expectEqualInt(request.entityType, 0, "entity request type default") && ok;
    ok = expectEqualBool(request.warp, false, "entity request warp default") && ok;
    ok = expectEqualBool(request.spawnedOnly, false, "entity request spawned only default") && ok;

    const ipc::EntityInformationResponse response{};
    ok = expectEqualUInt(response.requesterId, 0, "entity response requester default") && ok;
    ok = expectEqualUInt(response.targetId, 0, "entity response target default") && ok;
    ok = expectEqualInt(response.entityType, 0, "entity response type default") && ok;
    ok = expectEqualBool(response.warp, false, "entity response warp default") && ok;
    ok = expectEqualInt(response.zoneId, 0, "entity response zone default") && ok;
    ok = expectEqualFloat(response.x, 0.0f, "entity response x default") && ok;
    ok = expectEqualFloat(response.y, 0.0f, "entity response y default") && ok;
    ok = expectEqualFloat(response.z, 0.0f, "entity response z default") && ok;
    ok = expectEqualInt(response.rot, 0, "entity response rot default") && ok;
    ok = expectEqualUInt(response.moghouseId, 0, "entity response moghouse default") && ok;

    const ipc::SendPlayerToLocation location{};
    ok = expectEqualUInt(location.targetId, 0, "send location target default") && ok;
    ok = expectEqualInt(location.zoneId, 0, "send location zone default") && ok;
    ok = expectEqualFloat(location.x, 0.0f, "send location x default") && ok;
    ok = expectEqualFloat(location.y, 0.0f, "send location y default") && ok;
    ok = expectEqualFloat(location.z, 0.0f, "send location z default") && ok;
    ok = expectEqualInt(location.rot, 0, "send location rot default") && ok;
    ok = expectEqualUInt(location.moghouseId, 0, "send location moghouse default") && ok;

    const ipc::AssistChannelEvent assist{};
    ok = expectEqualUInt(assist.senderId, 0, "assist sender default") && ok;
    ok = expectEqualUInt(assist.receiverId, 0, "assist receiver default") && ok;
    ok = expectEqualInt(assist.action, 0, "assist action default") && ok;

    return ok;
}

auto testAssignedPayloads() -> bool
{
    bool ok = true;

    ok = expectEqualUInt(ipc::AccountLogin{ .accountId = 1001 }.accountId, 1001, "assigned account id") && ok;
    ok = expectEqualInt(ipc::CharZone{ .charId = 2002, .destinationZoneId = 230 }.destinationZoneId, 230, "assigned char zone") && ok;

    const ipc::CharVarUpdate varUpdate{ .charId = 3003, .value = -42, .expiry = 4004, .varName = "QuestFlag" };
    ok = expectEqualUInt(varUpdate.charId, 3003, "assigned char var char id") && ok;
    ok = expectEqualInt(varUpdate.value, -42, "assigned char var value") && ok;
    ok = expectEqualUInt(varUpdate.expiry, 4004, "assigned char var expiry") && ok;
    ok = expectEqualString(varUpdate.varName, "QuestFlag", "assigned char var name") && ok;

    const ipc::LuaFunction luaFunction{ .requesterZoneId = 101, .executorZoneId = 102, .funcString = "xi.test()" };
    ok = expectEqualInt(luaFunction.requesterZoneId, 101, "assigned lua requester") && ok;
    ok = expectEqualInt(luaFunction.executorZoneId, 102, "assigned lua executor") && ok;
    ok = expectEqualString(luaFunction.funcString, "xi.test()", "assigned lua function") && ok;
    ok = expectEqualUInt(ipc::KillSession{ .victimId = 5005 }.victimId, 5005, "assigned kill session") && ok;

    const ipc::EntityInformationRequest request{ .requesterId = 6006, .targetId = 7007, .entityType = 2, .warp = true, .spawnedOnly = true };
    ok = expectEqualUInt(request.requesterId, 6006, "assigned entity request requester") && ok;
    ok = expectEqualUInt(request.targetId, 7007, "assigned entity request target") && ok;
    ok = expectEqualInt(request.entityType, 2, "assigned entity request type") && ok;
    ok = expectEqualBool(request.warp, true, "assigned entity request warp") && ok;
    ok = expectEqualBool(request.spawnedOnly, true, "assigned entity request spawned only") && ok;

    const ipc::EntityInformationResponse response{ .requesterId = 6006, .targetId = 7007, .entityType = 2, .warp = true, .zoneId = 230, .x = 1.5f, .y = -2.25f, .z = 3.75f, .rot = 128, .moghouseId = 9009 };
    ok = expectEqualInt(response.zoneId, 230, "assigned entity response zone") && ok;
    ok = expectEqualFloat(response.x, 1.5f, "assigned entity response x") && ok;
    ok = expectEqualFloat(response.y, -2.25f, "assigned entity response y") && ok;
    ok = expectEqualFloat(response.z, 3.75f, "assigned entity response z") && ok;
    ok = expectEqualInt(response.rot, 128, "assigned entity response rot") && ok;
    ok = expectEqualUInt(response.moghouseId, 9009, "assigned entity response moghouse") && ok;

    const ipc::SendPlayerToLocation location{ .targetId = 1111, .zoneId = 241, .x = -10.5f, .y = 0.25f, .z = 99.75f, .rot = 64, .moghouseId = 2222 };
    ok = expectEqualUInt(location.targetId, 1111, "assigned send location target") && ok;
    ok = expectEqualInt(location.zoneId, 241, "assigned send location zone") && ok;
    ok = expectEqualFloat(location.x, -10.5f, "assigned send location x") && ok;
    ok = expectEqualFloat(location.y, 0.25f, "assigned send location y") && ok;
    ok = expectEqualFloat(location.z, 99.75f, "assigned send location z") && ok;
    ok = expectEqualInt(location.rot, 64, "assigned send location rot") && ok;
    ok = expectEqualUInt(location.moghouseId, 2222, "assigned send location moghouse") && ok;

    const ipc::AssistChannelEvent assist{ .senderId = 123, .receiverId = 456, .action = 7 };
    ok = expectEqualUInt(assist.senderId, 123, "assigned assist sender") && ok;
    ok = expectEqualUInt(assist.receiverId, 456, "assigned assist receiver") && ok;
    ok = expectEqualInt(assist.action, 7, "assigned assist action") && ok;

    return ok;
}

} // namespace

auto runIPCSessionPayloadSelfTests() -> bool
{
    return testDefaultPayloads() && testAssignedPayloads();
}
