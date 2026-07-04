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

#include "test_ipc_linkshell_payloads.h"

#include "common/ipc_structs.h"

#include <iostream>
#include <string>

namespace
{

auto expectEqualInt(int actual, int expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "IPC linkshell payload self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectEqualString(const std::string& actual, const std::string& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "IPC linkshell payload self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto testDefaultPayloads() -> bool
{
    bool ok = true;

    const ipc::LinkshellRankChange rankChange{};
    ok = expectEqualInt(rankChange.requesterId, 0, "rank requester id default") && ok;
    ok = expectEqualInt(rankChange.requesterRank, 0, "rank requester rank default") && ok;
    ok = expectEqualString(rankChange.memberName, "", "rank member name default") && ok;
    ok = expectEqualInt(rankChange.linkshellId, 0, "rank linkshell id default") && ok;
    ok = expectEqualInt(rankChange.newRank, 0, "rank new rank default") && ok;

    const ipc::LinkshellRemove remove{};
    ok = expectEqualInt(remove.requesterId, 0, "remove requester id default") && ok;
    ok = expectEqualInt(remove.requesterRank, 0, "remove requester rank default") && ok;
    ok = expectEqualString(remove.victimName, "", "remove victim name default") && ok;
    ok = expectEqualInt(remove.linkshellId, 0, "remove linkshell id default") && ok;

    const ipc::LinkshellSetMessage setMessage{};
    ok = expectEqualInt(setMessage.linkshellId, 0, "set message linkshell id default") && ok;
    ok = expectEqualString(setMessage.linkshellName, "", "set message linkshell name default") && ok;
    ok = expectEqualString(setMessage.poster, "", "set message poster default") && ok;
    ok = expectEqualString(setMessage.message, "", "set message body default") && ok;
    ok = expectEqualInt(setMessage.postTime, 0, "set message post time default") && ok;

    return ok;
}

auto testAssignedPayloads() -> bool
{
    bool ok = true;

    const ipc::LinkshellRankChange rankChange{ .requesterId = 1001, .requesterRank = 2, .memberName = "Pearlholder", .linkshellId = 3003, .newRank = 1 };
    ok = expectEqualInt(rankChange.requesterId, 1001, "assigned rank requester id") && ok;
    ok = expectEqualInt(rankChange.requesterRank, 2, "assigned rank requester rank") && ok;
    ok = expectEqualString(rankChange.memberName, "Pearlholder", "assigned rank member name") && ok;
    ok = expectEqualInt(rankChange.linkshellId, 3003, "assigned rank linkshell id") && ok;
    ok = expectEqualInt(rankChange.newRank, 1, "assigned rank new rank") && ok;

    const ipc::LinkshellRemove remove{ .requesterId = 1001, .requesterRank = 2, .victimName = "Retired", .linkshellId = 3003 };
    ok = expectEqualInt(remove.requesterId, 1001, "assigned remove requester id") && ok;
    ok = expectEqualInt(remove.requesterRank, 2, "assigned remove requester rank") && ok;
    ok = expectEqualString(remove.victimName, "Retired", "assigned remove victim name") && ok;
    ok = expectEqualInt(remove.linkshellId, 3003, "assigned remove linkshell id") && ok;

    const ipc::LinkshellSetMessage setMessage{ .linkshellId = 3003, .linkshellName = "OmegaShell", .poster = "Leader", .message = "Gather at 8", .postTime = 4004 };
    ok = expectEqualInt(setMessage.linkshellId, 3003, "assigned set message linkshell id") && ok;
    ok = expectEqualString(setMessage.linkshellName, "OmegaShell", "assigned set message linkshell name") && ok;
    ok = expectEqualString(setMessage.poster, "Leader", "assigned set message poster") && ok;
    ok = expectEqualString(setMessage.message, "Gather at 8", "assigned set message body") && ok;
    ok = expectEqualInt(setMessage.postTime, 4004, "assigned set message post time") && ok;

    return ok;
}

} // namespace

auto runIPCLinkshellPayloadSelfTests() -> bool
{
    return testDefaultPayloads() && testAssignedPayloads();
}
