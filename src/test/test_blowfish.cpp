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

#include "test_blowfish.h"

#include "common/blowfish.h"

#include <array>
#include <cstddef>
#include <iostream>
#include <string>

namespace
{

template <std::size_t Size>
auto expectWords(const uint32* actual, const std::array<uint32, Size>& expected, const std::string& label) -> bool
{
    for (std::size_t i = 0; i < expected.size(); ++i)
    {
        if (actual[i] != expected[i])
        {
            std::cerr << "blowfish self-test failed: " << label << " word " << i << " got "
                      << actual[i] << " expected " << expected[i] << '\n';
            return false;
        }
    }

    return true;
}

auto expectEqual(const uint32 actual, const uint32 expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "blowfish self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }

    return true;
}

auto initSequentialKey(uint32* P, uint32* S) -> void
{
    const int8 key[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    blowfish_init(key, 16, P, S);
}

} // namespace

auto runBlowfishSelfTests() -> bool
{
    bool ok = true;

    uint32 P[18]{};
    uint32 S[1024]{};
    initSequentialKey(P, S);

    ok = expectWords(P, std::array<uint32, 6>{ 0xECC666D5, 0x37D3C89B, 0x4E48F0BA, 0xC8852C4F, 0x59820312, 0x7E1C9EE0 }, "sequential key P") && ok;
    ok = expectWords(S, std::array<uint32, 6>{ 0x5F57A844, 0xA4AB8DCE, 0xFAA7BA1D, 0xF9D92041, 0xE4438204, 0x4B84C96A }, "sequential key S") && ok;

    {
        uint32 left  = 0x01234567;
        uint32 right = 0x89ABCDEF;
        blowfish_encipher(&left, &right, P, S);
        ok = expectEqual(left, 0x9E83B3EC, "scalar encipher left") && ok;
        ok = expectEqual(right, 0xE564651C, "scalar encipher right") && ok;

        blowfish_decipher(&left, &right, P, S);
        ok = expectEqual(left, 0x01234567, "scalar decipher left") && ok;
        ok = expectEqual(right, 0x89ABCDEF, "scalar decipher right") && ok;
    }

    {
        uint32 blocks[] = {
            0x00000000, 0x11111111,
            0x22222222, 0x33333333,
            0x44444444, 0x55555555,
            0x66666666, 0x77777777,
            0x88888888, 0x99999999,
        };

        blowfish_encipher_blocks(blocks, 5, P, S);
        ok = expectWords(blocks, std::array<uint32, 10>{
                                     0xC5E00A47, 0xBE9C88E4,
                                     0x7842A328, 0xE89894F7,
                                     0xA17540C1, 0x8726EFBB,
                                     0x9CFC4651, 0xB495157A,
                                     0xBA701166, 0xB1EAC103,
                                 },
                         "block encipher") &&
             ok;

        blowfish_decipher_blocks(blocks, 5, P, S);
        ok = expectWords(blocks, std::array<uint32, 10>{
                                     0x00000000, 0x11111111,
                                     0x22222222, 0x33333333,
                                     0x44444444, 0x55555555,
                                     0x66666666, 0x77777777,
                                     0x88888888, 0x99999999,
                                 },
                         "block decipher") &&
             ok;
    }

    {
        uint32      P2[18]{};
        uint32      S2[1024]{};
        const int8  key[] = { 'O', 'm', 'e', 'g', 'a', 'X', 'I' };
        blowfish_init(key, 7, P2, S2);

        ok = expectWords(P2, std::array<uint32, 6>{ 0x1986C39F, 0x922C3E85, 0x9F6D142B, 0xCA521058, 0xFF63470F, 0x9FAF9A00 }, "short key P") && ok;
        ok = expectWords(S2, std::array<uint32, 6>{ 0x7B146B4D, 0x423F50A0, 0x45B75A2C, 0x2D7B9797, 0x9E382B1C, 0x757ABBFC }, "short key S") && ok;

        uint32 left  = 0;
        uint32 right = 0;
        blowfish_encipher(&left, &right, P2, S2);
        ok = expectEqual(left, 0xDB376049, "short key encipher left") && ok;
        ok = expectEqual(right, 0x8106104D, "short key encipher right") && ok;
    }

    {
        uint32     P3[18]{};
        uint32     S3[1024]{};
        const int8 key[] = { static_cast<int8>(0x80), static_cast<int8>(0xFF), 0x00, 0x7F, 0x55 };
        blowfish_init(key, 5, P3, S3);

        ok = expectWords(P3, std::array<uint32, 6>{ 0x93CE0266, 0x7B1DD2EF, 0xD902DB62, 0x6D6EC1EE, 0x66091F4F, 0x50E34882 }, "high-bit key P") && ok;
        ok = expectWords(S3, std::array<uint32, 6>{ 0x9107F85A, 0x37FEA676, 0x6EFE9311, 0xD99558FF, 0x3CBEBC7A, 0xFB4BF63E }, "high-bit key S") && ok;

        uint32 left  = 0x01020304;
        uint32 right = 0xA0B0C0D0;
        blowfish_encipher(&left, &right, P3, S3);
        ok = expectEqual(left, 0x0D4439C5, "high-bit key encipher left") && ok;
        ok = expectEqual(right, 0xA9B25A05, "high-bit key encipher right") && ok;
    }

    return ok;
}
