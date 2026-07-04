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

#include "test_login_packet_hash.h"

#include "login/login_helpers.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>

namespace
{

constexpr std::size_t PacketSize       = 0x24;
constexpr std::size_t PacketHashOffset = 12;
constexpr std::size_t PacketHashLength = 16;

class AuthenticatedSessionsGuard
{
public:
    AuthenticatedSessionsGuard()
    : saved_(loginHelpers::getAuthenticatedSessions())
    {
    }

    ~AuthenticatedSessionsGuard()
    {
        loginHelpers::getAuthenticatedSessions() = saved_;
    }

private:
    std::unordered_map<std::string, std::map<std::string, session_t>> saved_;
};

auto expectEqual(const std::string& actual, const std::string& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "login packet hash self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto expectTrue(bool actual, const std::string& label) -> bool
{
    if (!actual)
    {
        std::cerr << "login packet hash self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto makePacket(const std::string& hash) -> std::array<uint8, PacketSize>
{
    auto packet = std::array<uint8, PacketSize>{};
    packet.fill(0xAA);
    std::memcpy(packet.data() + PacketHashOffset, hash.data(), std::min(hash.size(), PacketHashLength));
    return packet;
}

auto testAuthenticatedHashLookup() -> bool
{
    auto& sessions = loginHelpers::getAuthenticatedSessions();
    sessions.clear();

    const auto ip   = std::string("192.0.2.1");
    const auto hash = std::string("0123456789abcdef");
    sessions[ip][hash] = session_t{};

    auto packet = makePacket(hash);

    bool ok = true;
    ok = expectEqual(loginHelpers::getHashFromPacket(ip, packet.data()), hash, "authenticated hash") && ok;
    ok = expectEqual(loginHelpers::getHashFromPacket("198.51.100.7", packet.data()), "", "unknown ip") && ok;
    ok = expectTrue(sessions.find("198.51.100.7") != sessions.end(), "unknown ip inserted") && ok;

    auto unknownHashPacket = makePacket("fedcba9876543210");
    ok = expectEqual(loginHelpers::getHashFromPacket(ip, unknownHashPacket.data()), "", "unknown hash") && ok;

    return ok;
}

auto testBoundedStringSemantics() -> bool
{
    auto& sessions = loginHelpers::getAuthenticatedSessions();
    sessions.clear();

    const auto ip = std::string("192.0.2.1");

    sessions[ip]["Omega"] = session_t{};
    auto nulPacket = makePacket(std::string("Omega\0XI", 8));

    bool ok = true;
    ok = expectEqual(loginHelpers::getHashFromPacket(ip, nulPacket.data()), "Omega", "nul truncated hash") && ok;

    sessions.clear();
    auto highByteHash = std::string{
        static_cast<char>(0xFF),
        'A',
        static_cast<char>(0xFE),
        'Z',
        '1',
        '2',
        '3',
        '4',
        '5',
        '6',
        '7',
        '8',
        '9',
        '0',
        'a',
        'b',
    };
    sessions[ip][highByteHash] = session_t{};
    auto highBytePacket = makePacket(highByteHash);
    ok = expectEqual(loginHelpers::getHashFromPacket(ip, highBytePacket.data()), highByteHash, "high-byte hash") && ok;

    return ok;
}

} // namespace

auto runLoginPacketHashSelfTests() -> bool
{
    const AuthenticatedSessionsGuard guard;

    bool ok = true;
    ok = testAuthenticatedHashLookup() && ok;
    ok = testBoundedStringSemantics() && ok;

    return ok;
}
