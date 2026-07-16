/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_guild_buy_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x0aa_guild_buy.h"

auto runC2SGuildBuyRuntimeSelfTests() -> bool
{
    using guildbuyhelpers::MakeResultFields;
    using guildbuyhelpers::PreScriptAction;
    using guildbuyhelpers::SelectPreScriptAction;
    using guildbuyhelpers::ShouldSendScriptResult;

    const auto fields = MakeResultFields(0x1234, 0x56, -1);
    const auto ok     =
        SelectPreScriptAction(false, 1, 12) == PreScriptAction::None &&
        SelectPreScriptAction(true, 13, 12) == PreScriptAction::RejectStackLimit &&
        SelectPreScriptAction(true, 12, 12) == PreScriptAction::CallScript &&
        !ShouldSendScriptResult(false, true) &&
        !ShouldSendScriptResult(true, false) &&
        ShouldSendScriptResult(true, true) &&
        fields.itemNo == 0x1234 && fields.count == 0x56 && fields.trade == 0xFF;
    if (!ok)
    {
        std::cerr << "c2s GUILD_BUY runtime self-test failed\n";
    }
    return ok;
}
