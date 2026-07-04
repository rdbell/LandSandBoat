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

#include "test_hash_utils.h"

#include "common/md52.h"
#include "common/utils.h"

#include <array>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace
{

auto expectBytes(const std::array<uint8, 16>& actual, const std::array<uint8, 16>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "hash utils self-test failed: " << label << " byte mismatch\n";
        return false;
    }

    return true;
}

template <typename T, typename U>
auto expectEqual(const T actual, const U expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "hash utils self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto md5Bytes(std::vector<uint8> input) -> std::array<uint8, 16>
{
    std::array<uint8, 16> output{};
    md5(input.data(), output.data(), static_cast<int32>(input.size()));
    return output;
}

auto md5String(const std::string& input) -> std::array<uint8, 16>
{
    std::vector<uint8> bytes(input.begin(), input.end());
    return md5Bytes(bytes);
}

auto hexBytes(std::vector<uint8> input) -> std::string
{
    std::string output(input.size() * 2, '\0');
    output.push_back('\0');
    bin2hex(output.data(), input.data(), input.size());
    output.resize(input.size() * 2);
    return output;
}

} // namespace

auto runHashUtilsSelfTests() -> bool
{
    bool ok = true;

    ok = expectBytes(md5Bytes({}), std::array<uint8, 16>{
                                      0xD4, 0x1D, 0x8C, 0xD9, 0x8F, 0x00, 0xB2, 0x04,
                                      0xE9, 0x80, 0x09, 0x98, 0xEC, 0xF8, 0x42, 0x7E,
                                  },
                     "md5 empty") &&
         ok;

    ok = expectBytes(md5String("abc"), std::array<uint8, 16>{
                                           0x90, 0x01, 0x50, 0x98, 0x3C, 0xD2, 0x4F, 0xB0,
                                           0xD6, 0x96, 0x3F, 0x7D, 0x28, 0xE1, 0x7F, 0x72,
                                       },
                     "md5 abc") &&
         ok;

    ok = expectBytes(md5Bytes({ 0x00, 0xFF, 0x10, 0x20, 0x7F, 0x80 }), std::array<uint8, 16>{
                                                                                 0xFC, 0x51, 0x5A, 0x7E, 0x70, 0xC5, 0xD1, 0x54,
                                                                                 0x87, 0xB6, 0x9D, 0x91, 0x01, 0xA2, 0x5D, 0xF1,
                                                                             },
                     "md5 binary") &&
         ok;

    auto goodHash = md5String("OmegaXI");
    std::vector<uint8> omegaBytes{ 'O', 'm', 'e', 'g', 'a', 'X', 'I' };
    ok = expectEqual(checksum(omegaBytes.data(), static_cast<uint32>(omegaBytes.size()), reinterpret_cast<char*>(goodHash.data())), 0, "checksum match") && ok;

    std::array<uint8, 18> goodHashWithExtra{};
    std::memcpy(goodHashWithExtra.data(), goodHash.data(), goodHash.size());
    goodHashWithExtra[16] = 0xAA;
    goodHashWithExtra[17] = 0xBB;
    ok = expectEqual(checksum(omegaBytes.data(), static_cast<uint32>(omegaBytes.size()), reinterpret_cast<char*>(goodHashWithExtra.data())), 0, "checksum match ignores trailing bytes") && ok;

    auto badHash = goodHash;
    badHash[0] ^= 0xFF;
    ok = expectEqual(checksum(omegaBytes.data(), static_cast<uint32>(omegaBytes.size()), reinterpret_cast<char*>(badHash.data())), -1, "checksum mismatch") && ok;

    ok = expectEqual(hexBytes({}), std::string(""), "bin2hex empty") && ok;
    ok = expectEqual(hexBytes({ 'a', 'b', 'c' }), std::string("616263"), "bin2hex abc") && ok;
    ok = expectEqual(hexBytes({ 0x00, 0x0F, 0x10, 0xAB, 0xCD, 0xEF }), std::string("000f10abcdef"), "bin2hex binary") && ok;

    return ok;
}
