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

#include "test_nominate_proposal_vote_plan_2794.h"

#include "map/nominate_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "nominate proposal/vote plan 2794 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runNominateProposalVotePlan2794SelfTests() -> bool
{
    using nominatehelpers::OnProposalDisposition;
    using nominatehelpers::OnVoteDisposition;
    using nominatehelpers::PlanOnProposal;
    using nominatehelpers::PlanOnVote;
    using nominatehelpers::PlanProposerScope;
    using nominatehelpers::ProposerScopeDisposition;
    using Kind = GP_CLI_COMMAND_SWITCH_PROPOSAL_KIND;

    bool ok = true;

    // --- PlanProposerScope ---
    ok = expect(PlanProposerScope(Kind::Party, true, false, false) == ProposerScopeDisposition::Allow,
                "scope party allow") &&
        ok;
    ok = expect(PlanProposerScope(Kind::Party, false, true, true) == ProposerScopeDisposition::FailNoPartyMembers,
                "scope party no members") &&
        ok;
    ok = expect(PlanProposerScope(Kind::Linkshell1, false, true, false) == ProposerScopeDisposition::Allow,
                "scope ls1 allow") &&
        ok;
    ok = expect(PlanProposerScope(Kind::Linkshell1, true, false, true) == ProposerScopeDisposition::FailNoLinkshellEquipped,
                "scope ls1 unequipped") &&
        ok;
    ok = expect(PlanProposerScope(Kind::Linkshell2, false, false, true) == ProposerScopeDisposition::Allow,
                "scope ls2 allow") &&
        ok;
    ok = expect(PlanProposerScope(Kind::Linkshell2, true, true, false) == ProposerScopeDisposition::FailNoLinkshellEquipped,
                "scope ls2 unequipped") &&
        ok;
    ok = expect(PlanProposerScope(Kind::Say, false, false, false) == ProposerScopeDisposition::Allow,
                "scope say allow") &&
        ok;
    ok = expect(PlanProposerScope(Kind::Shout, false, false, false) == ProposerScopeDisposition::Allow,
                "scope shout allow") &&
        ok;
    // Unknown kind (cast of unused value) → Fail, no message.
    ok = expect(PlanProposerScope(static_cast<Kind>(0x00), true, true, true) == ProposerScopeDisposition::Fail,
                "scope unknown fail") &&
        ok;
    ok = expect(PlanProposerScope(static_cast<Kind>(0x04), true, true, true) == ProposerScopeDisposition::Fail,
                "scope 0x04 fail") &&
        ok;

    // --- PlanOnProposal ---
    ok = expect(PlanOnProposal(true, false, false) == OnProposalDisposition::Ignore, "proposal null") && ok;
    ok = expect(PlanOnProposal(true, true, true) == OnProposalDisposition::Ignore, "proposal null first") && ok;
    ok = expect(PlanOnProposal(false, true, false) == OnProposalDisposition::CloseActive, "proposal close") && ok;
    ok = expect(PlanOnProposal(false, true, true) == OnProposalDisposition::CloseActive, "proposal close before cooldown") &&
        ok;
    ok = expect(PlanOnProposal(false, false, true) == OnProposalDisposition::CannotUseCommand, "proposal cooldown") &&
        ok;
    ok = expect(PlanOnProposal(false, false, false) == OnProposalDisposition::CreateNew, "proposal create") && ok;

    // --- PlanOnVote ---
    ok = expect(PlanOnVote(true, true, true, false, 2, 1) == OnVoteDisposition::Ignore, "vote null") && ok;
    ok = expect(PlanOnVote(true, false, false, false, 0, 0) == OnVoteDisposition::Ignore, "vote null first") && ok;
    ok = expect(PlanOnVote(false, false, false, false, 0, 1) == OnVoteDisposition::NotProposed, "vote no poll") && ok;
    ok = expect(PlanOnVote(false, true, false, false, 2, 1) == OnVoteDisposition::NotProposed, "vote out of scope") &&
        ok;
    // !pollFound || !inScope: poll missing wins even if inScope host flag is true.
    ok = expect(PlanOnVote(false, false, true, false, 2, 1) == OnVoteDisposition::NotProposed, "vote missing poll") &&
        ok;
    ok = expect(PlanOnVote(false, true, true, true, 2, 1) == OnVoteDisposition::AlreadyVoted, "vote already") && ok;
    // Already voted short-circuits before invalid index.
    ok = expect(PlanOnVote(false, true, true, true, 2, 0) == OnVoteDisposition::AlreadyVoted, "vote already before index") &&
        ok;
    ok = expect(PlanOnVote(false, true, true, false, 0, 1) == OnVoteDisposition::InvalidChoice, "vote zero choices") &&
        ok;
    ok = expect(PlanOnVote(false, true, true, false, 2, 0) == OnVoteDisposition::InvalidChoice, "vote index 0") && ok;
    ok = expect(PlanOnVote(false, true, true, false, 2, 3) == OnVoteDisposition::InvalidChoice, "vote index high") &&
        ok;
    ok = expect(PlanOnVote(false, true, true, false, 2, 1) == OnVoteDisposition::AcceptVote, "vote accept 1") && ok;
    ok = expect(PlanOnVote(false, true, true, false, 2, 2) == OnVoteDisposition::AcceptVote, "vote accept 2") && ok;
    ok = expect(PlanOnVote(false, true, true, false, 8, 8) == OnVoteDisposition::AcceptVote, "vote accept max") && ok;

    return ok;
}
