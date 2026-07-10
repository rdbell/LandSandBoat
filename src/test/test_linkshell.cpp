/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_linkshell.h"

#include "map/entities/char_entity.h"
#include "map/linkshell.h"
#include "map/packets/c2s/0x0e2_set_lsmsg.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "linkshell self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

auto testValueState() -> bool
{
    CLinkshell shell(0x11223344);

    bool ok = true;
    ok      = expect(shell.getID() == 0x11223344, "constructor id") && ok;
    ok      = expect(shell.getColor() == 0, "constructor color") && ok;
    ok      = expect(shell.getPostRights() == GP_CLI_COMMAND_SET_LSMSG_WRITELEVEL::Linkshell, "constructor post rights") && ok;
    ok      = expect(shell.getName().empty(), "constructor name") && ok;

    shell.setColor(0xABCD);
    shell.setName("OmegaXI");
    ok = expect(shell.getColor() == 0xABCD, "set color") && ok;
    ok = expect(shell.getName() == "OmegaXI", "set name") && ok;
    return ok;
}

auto testMembershipOrderingAndReturnValue() -> bool
{
    CLinkshell shell(7);
    CCharEntity first;
    CCharEntity middle;
    CCharEntity last;
    CCharEntity missing;

    // Populate the public online roster directly. This keeps the fixture free
    // of AddMember's database/session side effects while exercising the exact
    // production deletion loop.
    shell.members = { &first, &middle, &last };

    bool ok = true;
    ok      = expect(shell.DelMember(&middle), "middle removal reports members remaining") && ok;
    ok      = expect(shell.members.size() == 2 && shell.members[0] == &first && shell.members[1] == &last,
                     "middle removal preserves order") && ok;
    ok = expect(shell.DelMember(&missing), "missing removal reports non-empty roster") && ok;
    ok = expect(shell.members.size() == 2, "missing removal does not mutate roster") && ok;
    ok = expect(shell.DelMember(&first), "penultimate removal reports member remaining") && ok;
    ok = expect(!shell.DelMember(&last), "last removal reports empty roster") && ok;
    ok = expect(shell.members.empty(), "last removal empties roster") && ok;
    ok = expect(!shell.DelMember(&missing), "missing removal from empty roster reports empty") && ok;
    return ok;
}

auto testRejectedAddsDoNotMutate() -> bool
{
    CLinkshell shell(9);
    CCharEntity member;
    shell.members = { &member };

    shell.AddMember(nullptr, 1, 1);
    shell.AddMember(&member, 3, 2);
    return expect(shell.members.size() == 1 && shell.members[0] == &member, "null and duplicate adds preserve roster");
}

} // namespace

auto runLinkshellSelfTests() -> bool
{
    bool ok = true;
    ok      = testValueState() && ok;
    ok      = testMembershipOrderingAndReturnValue() && ok;
    ok      = testRejectedAddsDoNotMutate() && ok;
    return ok;
}
