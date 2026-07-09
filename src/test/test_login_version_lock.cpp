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

#include "test_login_version_lock.h"

#include "login/version_lock.h"

#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace
{

using Direction = login::version_lock::MismatchDirection;

struct Case
{
    std::string               label;
    std::vector<std::uint8_t> client;
    std::string               expected;
    std::uint8_t              lockMode;
    std::string               normalizedClient;
    std::string               normalizedExpected;
    bool                      mismatch;
    bool                      fatal;
    Direction                 direction;
};

auto bytes(const std::string_view value) -> std::vector<std::uint8_t>
{
    return { value.begin(), value.end() };
}

auto expectCase(const Case& test) -> bool
{
    const auto actual = login::version_lock::evaluate(test.client, test.expected, test.lockMode);
    if (actual.clientVersion != test.normalizedClient ||
        actual.expectedVersion != test.normalizedExpected ||
        actual.mismatch != test.mismatch ||
        actual.fatal != test.fatal ||
        actual.direction != test.direction)
    {
        std::cerr << "login version lock self-test failed: " << test.label
                  << " got client=" << actual.clientVersion
                  << " expected=" << actual.expectedVersion
                  << " mismatch=" << actual.mismatch
                  << " fatal=" << actual.fatal
                  << " direction=" << static_cast<int>(actual.direction) << '\n';
        return false;
    }

    return true;
}

auto expectFlow(const std::string_view label,
                const std::vector<std::uint8_t>& client,
                const std::string_view expected,
                const std::uint8_t lockMode,
                const int expectedReads,
                const bool expectedFatal,
                const login::version_lock::ResponseLength expectedResponse) -> bool
{
    int reads = 0;
    const auto actual = login::version_lock::evaluateFlow(
        client,
        expected,
        [&]()
        {
            ++reads;
            return lockMode;
        });

    if (reads != expectedReads ||
        actual.version.fatal != expectedFatal ||
        actual.responseLength != expectedResponse ||
        (expectedReads != 0 && actual.lockMode != lockMode))
    {
        std::cerr << "login version flow self-test failed: " << label
                  << " reads=" << reads
                  << " fatal=" << actual.version.fatal
                  << " lockMode=" << static_cast<int>(actual.lockMode)
                  << " responseLength=" << static_cast<std::size_t>(actual.responseLength) << '\n';
        return false;
    }

    return true;
}

} // namespace

auto runLoginVersionLockSelfTests() -> bool
{
    const std::vector<Case> cases{
        { "exact disabled ignores suffix", bytes("302606E_0"), "30260604_0", login::version_lock::Disabled, "302606xx_x", "302606xx_x", false, false, Direction::None },
        { "exact strict", bytes("302606"), "302606", login::version_lock::Strict, "302606xx_x", "302606xx_x", false, false, Direction::None },
        { "exact greater or equal", bytes("302606"), "302606", login::version_lock::AllowNewer, "302606xx_x", "302606xx_x", false, false, Direction::None },
        { "exact unknown lock", bytes("302606"), "302606", 99, "302606xx_x", "302606xx_x", false, false, Direction::None },
        { "strict newer client", bytes("302607"), "30260604_0", login::version_lock::Strict, "302607xx_x", "302606xx_x", true, true, Direction::ServerTooOld },
        { "strict older client", bytes("302605"), "30260604_0", login::version_lock::Strict, "302605xx_x", "302606xx_x", true, true, Direction::ClientTooOld },
        { "greater or equal newer client", bytes("302607"), "30260604_0", login::version_lock::AllowNewer, "302607xx_x", "302606xx_x", true, false, Direction::ServerTooOld },
        { "greater or equal older client", bytes("302605"), "30260604_0", login::version_lock::AllowNewer, "302605xx_x", "302606xx_x", true, true, Direction::ClientTooOld },
        { "disabled older client", bytes("302605"), "30260604_0", login::version_lock::Disabled, "302605xx_x", "302606xx_x", true, false, Direction::ClientTooOld },
        { "unknown newer client", bytes("302607"), "30260604_0", 255, "302607xx_x", "302606xx_x", true, false, Direction::ServerTooOld },
        { "nul terminates client prefix", { '3', '0', '2', 0, '9', '9' }, "302", login::version_lock::Strict, "302xx_x", "302xx_x", false, false, Direction::None },
        { "nul is preserved in expected prefix", bytes("30"), std::string("30\0" "699", 6), login::version_lock::Disabled, "30xx_x", std::string("30\0" "699xx_x", 10), true, false, Direction::ServerTooOld },
        { "empty client", {}, "1", login::version_lock::AllowNewer, "xx_x", "1xx_x", true, false, Direction::ServerTooOld },
        { "short client and expected", bytes("30"), "31", login::version_lock::AllowNewer, "30xx_x", "31xx_x", true, true, Direction::ClientTooOld },
        { "lexicographic not numeric", bytes("30269"), "302610", login::version_lock::AllowNewer, "30269xx_x", "302610xx_x", true, false, Direction::ServerTooOld },
    };

    bool ok = true;
    for (const auto& test : cases)
    {
        ok = expectCase(test) && ok;
    }

    ok = expectFlow("exact skips lock read", bytes("302606"), "302606", login::version_lock::Strict, 0, false, login::version_lock::ResponseLength::KeyPacket) && ok;
    ok = expectFlow("fatal mismatch selects error packet", bytes("302605"), "302606", login::version_lock::Strict, 1, true, login::version_lock::ResponseLength::VersionError) && ok;
    ok = expectFlow("nonfatal mismatch selects key packet", bytes("302605"), "302606", login::version_lock::Disabled, 1, false, login::version_lock::ResponseLength::KeyPacket) && ok;
    return ok;
}
