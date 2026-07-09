/*
===========================================================================

  Copyright (c) 2026 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
  Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "test_map_session_blowfish.h"

#include "common/blowfish.h"
#include "map/map_session_blowfish.h"

#include <array>
#include <cstdint>
#include <iostream>

namespace
{

auto expectBool(bool actual, bool expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "map session blowfish self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

auto expectUInt(std::uint64_t actual, std::uint64_t expected, const char* label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "map session blowfish self-test failed: " << label << " got "
                  << actual << " expected " << expected << '\n';
        return false;
    }
    return true;
}

template <typename Actual, typename Expected>
auto expectRange(const Actual& actual, const Expected& expected, const char* label) -> bool
{
    for (std::size_t index = 0; index < expected.size(); ++index)
    {
        if (actual[index] != expected[index])
        {
            std::cerr << "map session blowfish self-test failed: " << label << '[' << index << "] got "
                      << actual[index] << " expected " << expected[index] << '\n';
            return false;
        }
    }
    return true;
}

auto expectBlowfish(const blowfish_t& actual, const blowfish_t& expected, const char* label) -> bool
{
    bool ok = true;
    for (std::size_t index = 0; index < std::size(actual.key); ++index)
    {
        if (actual.key[index] != expected.key[index])
        {
            std::cerr << "map session blowfish self-test failed: " << label << " key[" << index << "] mismatch\n";
            ok = false;
        }
    }
    for (std::size_t index = 0; index < std::size(actual.hash); ++index)
    {
        if (actual.hash[index] != expected.hash[index])
        {
            std::cerr << "map session blowfish self-test failed: " << label << " hash[" << index << "] mismatch\n";
            ok = false;
        }
    }
    for (std::size_t index = 0; index < std::size(actual.P); ++index)
    {
        if (actual.P[index] != expected.P[index])
        {
            std::cerr << "map session blowfish self-test failed: " << label << " P[" << index << "] mismatch\n";
            ok = false;
        }
    }
    for (std::size_t box = 0; box < std::size(actual.S); ++box)
    {
        for (std::size_t index = 0; index < std::size(actual.S[box]); ++index)
        {
            if (actual.S[box][index] != expected.S[box][index])
            {
                std::cerr << "map session blowfish self-test failed: " << label << " S[" << box << "][" << index << "] mismatch\n";
                ok = false;
            }
        }
    }
    if (actual.status != expected.status)
    {
        std::cerr << "map session blowfish self-test failed: " << label << " status mismatch\n";
        ok = false;
    }
    return ok;
}

auto testTwentyByteLittleEndianKeyInitialization() -> bool
{
    blowfish_t blowfish{};
    blowfish.key[0] = 0x03020100;
    blowfish.key[1] = 0x07060504;
    blowfish.key[2] = 0x0B0A0908;
    blowfish.key[3] = 0x0F0E0D0C;
    blowfish.key[4] = 0x13121110;
    blowfish.status = BLOWFISH_ACCEPTED;

    map_session_blowfish::initialize(blowfish);

    constexpr std::array<uint8, 16> expectedHash{
        0x15, 0x49, 0xD1, 0xAA, 0xE2, 0x02, 0x14, 0xE0,
        0x65, 0xAB, 0x4B, 0x76, 0xAA, 0xAC, 0x89, 0xA8,
    };
    constexpr std::array<uint32, 6> expectedP{
        0xE18D9F12, 0xC4F88600, 0x9940C1D8,
        0xA8A3D0C2, 0x0E35B2E6, 0xCF95E1CE,
    };
    constexpr std::array<uint32, 6> expectedS{
        0xBB332D60, 0xB02723AD, 0x03BAE7EE,
        0x65396F9E, 0x6502D5A5, 0x0A5DAFFD,
    };

    bool ok = true;
    ok      = expectRange(blowfish.hash, expectedHash, "little-endian hash") && ok;
    ok      = expectRange(blowfish.P, expectedP, "initialized P") && ok;
    ok      = expectRange(blowfish.S[0], expectedS, "initialized S") && ok;
    ok      = expectUInt(blowfish.key[0], 0x03020100, "key word zero retained") && ok;
    ok      = expectUInt(blowfish.key[4], 0x13121110, "key word four retained") && ok;
    ok      = expectUInt(blowfish.status, BLOWFISH_ACCEPTED, "status retained") && ok;

    uint32 left  = 0x01234567;
    uint32 right = 0x89ABCDEF;
    blowfish_encipher(&left, &right, blowfish.P, blowfish.S[0]);
    ok = expectUInt(left, 0x405B7A60, "initialized encipher left") && ok;
    ok = expectUInt(right, 0xF5165827, "initialized encipher right") && ok;
    return ok;
}

auto testFirstNullDigestSuffixIsZeroed() -> bool
{
    blowfish_t blowfish{};
    blowfish.key[0] = 0x11223344;
    blowfish.key[1] = 0x55667788;
    blowfish.key[2] = 0x99AABBCC;
    blowfish.key[3] = 0xDDEEFF00;
    blowfish.key[4] = 18;
    blowfish.status = BLOWFISH_PENDING_ZONE;

    map_session_blowfish::initialize(blowfish);

    // Raw MD5: bea3c924e13ae2bdedd9006826a85a89. The first zero is
    // retained as the terminator and its entire suffix is cleared.
    constexpr std::array<uint8, 16> expectedHash{
        0xBE, 0xA3, 0xC9, 0x24, 0xE1, 0x3A, 0xE2, 0xBD,
        0xED, 0xD9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    constexpr std::array<uint32, 6> expectedP{
        0xAAD2F5D6, 0x402DA62A, 0xC9B87A0C,
        0xACEEA0D8, 0x420EB15B, 0xCE92EE1F,
    };
    constexpr std::array<uint32, 6> expectedS{
        0x678291C0, 0x2F786591, 0x14B34CEF,
        0x5394C8D2, 0xA552229B, 0x94E09214,
    };

    bool ok = true;
    ok      = expectRange(blowfish.hash, expectedHash, "NUL-truncated hash") && ok;
    ok      = expectRange(blowfish.P, expectedP, "NUL initialized P") && ok;
    ok      = expectRange(blowfish.S[0], expectedS, "NUL initialized S") && ok;
    ok      = expectUInt(blowfish.status, BLOWFISH_PENDING_ZONE, "NUL status retained") && ok;

    uint32 left  = 0x01234567;
    uint32 right = 0x89ABCDEF;
    blowfish_encipher(&left, &right, blowfish.P, blowfish.S[0]);
    ok = expectUInt(left, 0x02EC7F50, "NUL encipher left") && ok;
    ok = expectUInt(right, 0x8BC9BE28, "NUL encipher right") && ok;
    return ok;
}

auto testIncrementSnapshotsAndResets() -> bool
{
    blowfish_t blowfish{};
    blowfish.key[0] = 1;
    blowfish.key[1] = 2;
    blowfish.key[2] = 3;
    blowfish.key[3] = 4;
    blowfish.key[4] = 0x10203040;
    blowfish.status = BLOWFISH_PENDING_ZONE;
    map_session_blowfish::initialize(blowfish);

    const auto expectedPrevious = blowfish;
    blowfish_t previous{};
    bool       hasDecryptedPacket = true;
    map_session_blowfish::increment(blowfish, previous, hasDecryptedPacket);

    bool ok = true;
    ok      = expectBool(hasDecryptedPacket, false, "increment resets decrypted flag") && ok;
    ok      = expectBlowfish(previous, expectedPrevious, "previous snapshot") && ok;
    ok      = expectUInt(blowfish.key[0], 1, "increment keeps key word zero") && ok;
    ok      = expectUInt(blowfish.key[4], 0x10203042, "increment adds two") && ok;
    ok      = expectUInt(blowfish.status, BLOWFISH_PENDING_ZONE, "increment retains status") && ok;
    ok      = expectBool(blowfish.P[0] != expectedPrevious.P[0], true, "increment initializes new P") && ok;
    ok      = expectBool(blowfish.S[0][0] != expectedPrevious.S[0][0], true, "increment initializes new S") && ok;

    blowfish.key[0] ^= 0xFFFFFFFF;
    blowfish.hash[0] ^= 0xFF;
    blowfish.P[0] ^= 0xFFFFFFFF;
    blowfish.S[0][0] ^= 0xFFFFFFFF;
    ok = expectBlowfish(previous, expectedPrevious, "previous deep copy") && ok;
    return ok;
}

auto testIncrementWrapsWordFour() -> bool
{
    blowfish_t blowfish{};
    blowfish.key[0] = 10;
    blowfish.key[1] = 20;
    blowfish.key[2] = 30;
    blowfish.key[3] = 40;
    blowfish.key[4] = 0xFFFFFFFF;
    blowfish.status = BLOWFISH_SENT;
    map_session_blowfish::initialize(blowfish);

    const auto expectedPrevious = blowfish;
    blowfish_t previous{};
    bool       hasDecryptedPacket = true;
    map_session_blowfish::increment(blowfish, previous, hasDecryptedPacket);

    bool ok = true;
    ok      = expectUInt(blowfish.key[4], 1, "wrapped key word four") && ok;
    ok      = expectUInt(blowfish.status, BLOWFISH_SENT, "wrapped status retained") && ok;
    ok      = expectUInt(previous.key[4], 0xFFFFFFFF, "wrapped previous key") && ok;
    ok      = expectBlowfish(previous, expectedPrevious, "wrapped previous snapshot") && ok;
    ok      = expectBool(hasDecryptedPacket, false, "wrapped increment resets decrypted flag") && ok;
    return ok;
}

} // namespace

auto runMapSessionBlowfishSelfTests() -> bool
{
    bool ok = true;
    ok      = testTwentyByteLittleEndianKeyInitialization() && ok;
    ok      = testFirstNullDigestSuffixIsZeroed() && ok;
    ok      = testIncrementSnapshotsAndResets() && ok;
    ok      = testIncrementWrapsWordFour() && ok;
    return ok;
}
