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

#include "test_typed_flags.h"

#include "common/types/flag.h"

#include <iostream>
#include <string>
#include <type_traits>

namespace
{

struct LocalFlagTag;
using LocalFlag = xi::Flag<LocalFlagTag>;

struct OtherFlagTag;
using OtherFlag = xi::Flag<OtherFlagTag>;

static_assert(!std::is_same_v<LocalFlag, OtherFlag>);
static_assert(std::is_constructible_v<LocalFlag, bool>);
static_assert(!std::is_convertible_v<bool, LocalFlag>);
static_assert(std::is_convertible_v<LocalFlag, bool>);
static_assert(std::is_same_v<SendPacket, xi::Flag<SendPacketTag>>);
static_assert(std::is_same_v<IsRecycleBin, xi::Flag<IsRecycleBinTag>>);

template <typename T, typename U>
auto expectEqual(const T actual, const U expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "typed flags self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto testLocalFlags() -> bool
{
    bool ok = true;

    const auto yes = LocalFlag::Yes;
    const auto no  = LocalFlag::No;
    ok             = expectEqual(static_cast<bool>(yes), true, "Yes bool") && ok;
    ok             = expectEqual(static_cast<bool>(no), false, "No bool") && ok;
    ok             = expectEqual(yes == no, false, "Yes != No") && ok;
    ok             = expectEqual(yes == LocalFlag(true), true, "Yes equals true construction") && ok;
    ok             = expectEqual(no == LocalFlag(false), true, "No equals false construction") && ok;
    ok             = expectEqual(LocalFlag(true) != LocalFlag(false), true, "operator!=") && ok;

    return ok;
}

auto testCommonAliases() -> bool
{
    bool ok = true;

    const SendPacket send       = SendPacket::Yes;
    const IsRecycleBin recycle = IsRecycleBin::No;

    ok = expectEqual(static_cast<bool>(send), true, "SendPacket yes") && ok;
    ok = expectEqual(static_cast<bool>(recycle), false, "IsRecycleBin no") && ok;
    ok = expectEqual(send == SendPacket(true), true, "SendPacket construction") && ok;
    ok = expectEqual(recycle == IsRecycleBin(false), true, "IsRecycleBin construction") && ok;

    return ok;
}

} // namespace

auto runTypedFlagsSelfTests() -> bool
{
    bool ok = true;

    ok = testLocalFlags() && ok;
    ok = testCommonAliases() && ok;

    return ok;
}
