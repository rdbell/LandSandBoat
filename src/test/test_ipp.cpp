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

#include "test_ipp.h"

#include "common/ipp.h"

#include <iostream>
#include <string>

namespace
{

template <typename T, typename U>
auto expectEqual(const T actual, const U expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "IPP self-test failed: " << label << " expected " << expected << ", got " << actual << '\n';
        return false;
    }

    return true;
}

auto expectIPP(const std::string& label,
               const std::string& ipString,
               const uint16       port,
               const uint32       expectedIP,
               const uint64       expectedRaw,
               const std::string& expectedText) -> bool
{
    bool ok = true;

    const auto ip = str2ip(ipString);
    ok            = expectEqual(ip, expectedIP, label + " str2ip") && ok;
    ok            = expectEqual(ip2str(ip), ipString, label + " ip2str") && ok;

    const auto pair = IPP(ip, port);
    ok              = expectEqual(pair.getIP(), expectedIP, label + " getIP") && ok;
    ok              = expectEqual(pair.getIPString(), ipString, label + " getIPString") && ok;
    ok              = expectEqual(pair.getPort(), port, label + " getPort") && ok;
    ok              = expectEqual(pair.getRawIPP(), expectedRaw, label + " getRawIPP") && ok;
    ok              = expectEqual(pair.toString(), expectedText, label + " toString") && ok;

    const auto unpacked = IPP(expectedRaw);
    if (!(unpacked == pair))
    {
        std::cerr << "IPP self-test failed: " << label << " unpacked equality\n";
        ok = false;
    }

    return ok;
}

} // namespace

auto runIPPSelfTests() -> bool
{
    bool ok = true;

    ok = expectIPP("loopback", "127.0.0.1", 54001, 16777343U, 231932545728639ULL, "127.0.0.1:54001") && ok;
    ok = expectIPP("any", "0.0.0.0", 0, 0U, 0ULL, "0.0.0.0:0") && ok;
    ok = expectIPP("broadcast",
                   "255.255.255.255",
                   65535,
                   4294967295U,
                   281474976710655ULL,
                   "255.255.255.255:65535") &&
         ok;
    ok = expectIPP("zone", "192.168.1.45", 54230, 755083456U, 232916831545536ULL, "192.168.1.45:54230") && ok;
    ok = expectIPP("public", "10.20.30.40", 12345, 673059850U, 53022044328970ULL, "10.20.30.40:12345") && ok;

    // inet_pton acceptance of noncanonical IPv4 text with leading zeroes
    // differs between supported platforms, so only canonical forms are pinned.

    const auto invalidIP = str2ip("not-an-ip");
    ok                   = expectEqual(invalidIP, 0U, "invalid str2ip") && ok;
    ok                   = expectEqual(ip2str(invalidIP), "0.0.0.0", "invalid ip2str") && ok;

    const auto fromRaw = IPP(2329165952ULL);
    ok                 = expectEqual(fromRaw.getIP(), 2329165952U, "from raw getIP") && ok;
    ok                 = expectEqual(fromRaw.getPort(), 0U, "from raw getPort") && ok;
    ok                 = expectEqual(fromRaw.toString(), "128.64.212.138:0", "from raw toString") && ok;

    const auto a = IPP(str2ip("10.0.0.1"), 1000);
    const auto b = IPP(str2ip("10.0.0.1"), 1001);
    const auto c = IPP(str2ip("10.0.0.2"), 1);

    if (!(a < b))
    {
        std::cerr << "IPP self-test failed: lower port should sort first for same IP\n";
        ok = false;
    }

    if (!(b < c))
    {
        std::cerr << "IPP self-test failed: lower stored IP should sort first\n";
        ok = false;
    }

    if (c < b)
    {
        std::cerr << "IPP self-test failed: higher stored IP sorted first\n";
        ok = false;
    }

    if (!(a == IPP(a.getIP(), a.getPort())))
    {
        std::cerr << "IPP self-test failed: equality\n";
        ok = false;
    }

    return ok;
}
