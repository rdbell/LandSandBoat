/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

===========================================================================
*/

#include "test_c2s_switch_proposal_runtime.h"

#include <iostream>

#include "map/packets/c2s/0x0a0_switch_proposal.h"

auto runC2SSwitchProposalRuntimeSelfTests() -> bool
{
    if (switchproposalhelpers::SelectAction(true) != switchproposalhelpers::Action::SubmitProposal)
    {
        std::cerr << "c2s SWITCH_PROPOSAL runtime self-test failed: zoned character does not submit proposal\n";
        return false;
    }

    if (switchproposalhelpers::SelectAction(false) != switchproposalhelpers::Action::None)
    {
        std::cerr << "c2s SWITCH_PROPOSAL runtime self-test failed: zone-less character does not no-op\n";
        return false;
    }

    return true;
}
