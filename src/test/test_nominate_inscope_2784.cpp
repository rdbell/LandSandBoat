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

#include "test_nominate_inscope_2784.h"

#include "map/nominate_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "nominate inScope 2784 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runNominateInScope2784SelfTests() -> bool
{
    using nominatehelpers::InScope;
    using nominatehelpers::InScopeForLinkshell;
    using nominatehelpers::InScopeForParty;
    using nominatehelpers::InScopeForZoneWide;
    using Kind = GP_CLI_COMMAND_SWITCH_PROPOSAL_KIND;

    bool ok = true;

    // Party: no party rejects.
    ok = expect(!InScopeForParty(10, 20, false, 0, false, 0), "party: no party rejects") && ok;

    // Party: matching party id accepts regardless of alliance.
    ok = expect(InScopeForParty(10, 20, true, 10, false, 0), "party: matching party id") && ok;
    ok = expect(InScopeForParty(10, 0, true, 10, true, 99), "party: matching party ignores alliance") && ok;

    // Party: mismatched party falls through to alliance match.
    ok = expect(InScopeForParty(10, 20, true, 11, true, 20), "party: alliance match") && ok;
    ok = expect(!InScopeForParty(10, 20, true, 11, true, 21), "party: alliance mismatch rejects") && ok;
    ok = expect(!InScopeForParty(10, 20, true, 11, false, 20), "party: no alliance rejects") && ok;
    ok = expect(!InScopeForParty(10, 0, true, 11, true, 0), "party: zero proposal alliance rejects") && ok;

    // Linkshell: either slot matching proposal id accepts.
    ok = expect(InScopeForLinkshell(5, true, 5, false, 0), "ls: slot1 match") && ok;
    ok = expect(InScopeForLinkshell(5, false, 0, true, 5), "ls: slot2 match") && ok;
    ok = expect(InScopeForLinkshell(5, true, 5, true, 9), "ls: either slot") && ok;
    ok = expect(!InScopeForLinkshell(5, true, 4, true, 6), "ls: neither slot rejects") && ok;
    ok = expect(!InScopeForLinkshell(5, false, 0, false, 0), "ls: unequipped rejects") && ok;

    // Zone-wide: say/shout always true; other kinds false.
    ok = expect(InScopeForZoneWide(Kind::Say), "zone: say") && ok;
    ok = expect(InScopeForZoneWide(Kind::Shout), "zone: shout") && ok;
    ok = expect(!InScopeForZoneWide(Kind::Party), "zone: party false") && ok;
    ok = expect(!InScopeForZoneWide(Kind::Linkshell1), "zone: ls1 false") && ok;

    // Combined switch: party path.
    ok = expect(InScope(Kind::Party, 10, 20, 0, true, 10, false, 0, false, 0, false, 0),
                "InScope party match") &&
        ok;
    ok = expect(!InScope(Kind::Party, 10, 20, 0, false, 0, false, 0, false, 0, false, 0),
                "InScope party no membership") &&
        ok;

    // Combined switch: both linkshell kinds use linkshell membership.
    ok = expect(InScope(Kind::Linkshell1, 0, 0, 7, false, 0, false, 0, true, 7, false, 0),
                "InScope ls1") &&
        ok;
    ok = expect(InScope(Kind::Linkshell2, 0, 0, 7, false, 0, false, 0, false, 0, true, 7),
                "InScope ls2") &&
        ok;
    ok = expect(!InScope(Kind::Linkshell1, 0, 0, 7, true, 10, true, 20, false, 0, false, 0),
                "InScope ls ignores party membership") &&
        ok;

    // Combined switch: say/shout always in scope (ignore membership).
    ok = expect(InScope(Kind::Say, 0, 0, 0, false, 0, false, 0, false, 0, false, 0),
                "InScope say always") &&
        ok;
    ok = expect(InScope(Kind::Shout, 99, 99, 99, false, 0, false, 0, false, 0, false, 0),
                "InScope shout always") &&
        ok;

    return ok;
}
