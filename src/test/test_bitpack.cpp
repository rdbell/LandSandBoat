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

#include "test_bitpack.h"

#include "common/utils.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>

namespace
{

template <typename T, typename U>
auto expectEqual(const T actual, const U expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "bitpack self-test failed: " << label << " expected " << expected << ", got " << actual << '\n';
        return false;
    }

    return true;
}

template <std::size_t N>
auto expectBytes(const std::array<uint8, N>& actual, const std::array<uint8, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "bitpack self-test failed: " << label << " byte mismatch\n";
        return false;
    }

    return true;
}

} // namespace

auto runBitpackSelfTests() -> bool
{
    bool ok = true;

    {
        std::array<uint8, 2> bits{ 0x00, 0x80 };
        ok = expectEqual(hasBit(15, bits.data(), bits.size()), 128, "hasBit 15") && ok;
        ok = expectEqual(hasBit(0, bits.data(), bits.size()), 0, "hasBit 0") && ok;
        ok = expectEqual(addBit(3, bits.data(), bits.size()), 1, "addBit 3") && ok;
        ok = expectBytes(bits, std::array<uint8, 2>{ 0x08, 0x80 }, "after addBit 3") && ok;
        ok = expectEqual(addBit(3, bits.data(), bits.size()), 0, "addBit 3 again") && ok;
        ok = expectEqual(delBit(15, bits.data(), bits.size()), 1, "delBit 15") && ok;
        ok = expectBytes(bits, std::array<uint8, 2>{ 0x08, 0x00 }, "after delBit 15") && ok;
        ok = expectEqual(hasBit(16, bits.data(), bits.size()), 0, "hasBit out of range") && ok;
        ok = expectEqual(addBit(16, bits.data(), bits.size()), 0, "addBit out of range") && ok;
        ok = expectEqual(delBit(16, bits.data(), bits.size()), 0, "delBit out of range") && ok;
    }

    {
        std::array<uint8, 4> buf{};
        ok = expectEqual(packBitsBE(buf.data(), 0xAB, 0, 8), 8U, "packBitsBE one byte offset") && ok;
        ok = expectBytes(buf, std::array<uint8, 4>{ 0xAB, 0x00, 0x00, 0x00 }, "packBitsBE one byte bytes") && ok;
        ok = expectEqual(unpackBitsBE(buf.data(), 0, 8), 171ULL, "unpackBitsBE one byte") && ok;
    }

    {
        std::array<uint8, 5> buf{ 0x11, 0x22, 0x33, 0x44, 0x55 };
        ok = expectEqual(packBitsBE(buf.data(), 0xAB, 4, 8), 12U, "packBitsBE cross-byte offset") && ok;
        ok = expectBytes(buf, std::array<uint8, 5>{ 0xB1, 0x2A, 0x33, 0x44, 0x55 }, "packBitsBE cross-byte bytes") && ok;
        ok = expectEqual(unpackBitsBE(buf.data(), 4, 8), 171ULL, "unpackBitsBE cross-byte") && ok;
    }

    {
        std::array<uint8, 4> buf{ 0xFF, 0x00, 0xAA, 0x55 };
        ok = expectEqual(packBitsBE(buf.data(), 0x5, 1, 3, 5), 16U, "packBitsBE byte offset return") && ok;
        ok = expectBytes(buf, std::array<uint8, 4>{ 0xFF, 0x28, 0xAA, 0x55 }, "packBitsBE byte offset bytes") && ok;
        ok = expectEqual(unpackBitsBE(buf.data(), 1, 3, 5), 5ULL, "unpackBitsBE byte offset") && ok;
    }

    {
        std::array<uint8, 8> buf{};
        uint32               offset = 0;
        offset                       = packBitsBE(buf.data(), 0x12345678, offset, 32);
        offset                       = packBitsBE(buf.data(), 0x2A, offset, 6);
        offset                       = packBitsBE(buf.data(), 0xD, offset, 4);
        ok = expectEqual(offset, 42U, "packBitsBE sequential offset") && ok;
        ok = expectBytes(buf, std::array<uint8, 8>{ 0x78, 0x56, 0x34, 0x12, 0x6A, 0x03, 0x00, 0x00 }, "packBitsBE sequential bytes") && ok;
        ok = expectEqual(unpackBitsBE(buf.data(), 0, 32), 305419896ULL, "unpackBitsBE sequential first") && ok;
        ok = expectEqual(unpackBitsBE(buf.data(), 32, 6), 42ULL, "unpackBitsBE sequential second") && ok;
        ok = expectEqual(unpackBitsBE(buf.data(), 38, 4), 13ULL, "unpackBitsBE sequential third") && ok;
    }

    {
        std::array<uint8, 9> buf{ 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90 };
        ok = expectEqual(packBitsBE(buf.data(), 0x123456789ULL, 5, 36), 41U, "packBitsBE wide offset") && ok;
        ok = expectBytes(buf, std::array<uint8, 9>{ 0x30, 0xF1, 0xAC, 0x68, 0x24, 0x60, 0x70, 0x80, 0x90 }, "packBitsBE wide bytes") && ok;
        ok = expectEqual(unpackBitsBE(buf.data(), 5, 36), 4886718345ULL, "unpackBitsBE wide") && ok;
    }

    {
        std::array<uint8, 4> buf{};
        ok = expectEqual(packBitsLE(buf.data(), 0xAB, 0, 8), 8U, "packBitsLE one byte offset") && ok;
        ok = expectBytes(buf, std::array<uint8, 4>{ 0xAB, 0x00, 0x00, 0x00 }, "packBitsLE one byte bytes") && ok;
        ok = expectEqual(unpackBitsLE(buf.data(), 0, 8), 171ULL, "unpackBitsLE one byte") && ok;
    }

    {
        std::array<uint8, 5> buf{ 0x11, 0x22, 0x33, 0x44, 0x55 };
        ok = expectEqual(packBitsLE(buf.data(), 0xAB, 4, 8), 12U, "packBitsLE cross-byte offset") && ok;
        ok = expectBytes(buf, std::array<uint8, 5>{ 0x1A, 0xB2, 0x33, 0x44, 0x55 }, "packBitsLE cross-byte bytes") && ok;
        ok = expectEqual(unpackBitsLE(buf.data(), 4, 8), 171ULL, "unpackBitsLE cross-byte") && ok;
    }

    {
        std::array<uint8, 4> buf{ 0xFF, 0x00, 0xAA, 0x55 };
        ok = expectEqual(packBitsLE(buf.data(), 0x5, 1, 3, 5), 16U, "packBitsLE byte offset return") && ok;
        ok = expectBytes(buf, std::array<uint8, 4>{ 0xFF, 0x05, 0xAA, 0x55 }, "packBitsLE byte offset bytes") && ok;
        ok = expectEqual(unpackBitsLE(buf.data(), 1, 3, 5), 5ULL, "unpackBitsLE byte offset") && ok;
    }

    {
        std::array<uint8, 6> buf{};
        uint32               offset = 0;
        offset                       = packBitsLE(buf.data(), 0x12, offset, 5);
        offset                       = packBitsLE(buf.data(), 0x7, offset, 4);
        offset                       = packBitsLE(buf.data(), 'A', offset, 7);
        ok = expectEqual(offset, 16U, "packBitsLE sequential offset") && ok;
        ok = expectBytes(buf, std::array<uint8, 6>{ 0x93, 0xC1, 0x00, 0x00, 0x00, 0x00 }, "packBitsLE sequential bytes") && ok;
        ok = expectEqual(unpackBitsLE(buf.data(), 0, 5), 18ULL, "unpackBitsLE sequential first") && ok;
        ok = expectEqual(unpackBitsLE(buf.data(), 5, 4), 7ULL, "unpackBitsLE sequential second") && ok;
        ok = expectEqual(unpackBitsLE(buf.data(), 9, 7), 65ULL, "unpackBitsLE sequential third") && ok;
    }

    {
        std::array<uint8, 9> buf{ 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90 };
        ok = expectEqual(packBitsLE(buf.data(), 0x123456789ULL, 5, 36), 41U, "packBitsLE wide offset") && ok;
        ok = expectBytes(buf, std::array<uint8, 9>{ 0x10, 0x91, 0xA2, 0xB3, 0xC4, 0xE0, 0x70, 0x80, 0x90 }, "packBitsLE wide bytes") && ok;
        ok = expectEqual(unpackBitsLE(buf.data(), 5, 36), 4886718345ULL, "unpackBitsLE wide") && ok;
    }

    {
        std::array<uint8, 2> buf{ 0xB6, 0x00 };
        ok = expectEqual(unpackBitsLE(buf.data(), 1, 3), 3ULL, "unpackBitsLE single byte branch") && ok;
    }

    {
        std::array<uint8, 10> buf{ 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11, 0x22, 0x33, 0x44 };
        const auto            before = buf;
        ok                           = expectEqual(packBitsLE(buf.data(), 1, 0, 1, 64), 0U, "packBitsLE over 64 offset") && ok;
        ok                           = expectBytes(buf, before, "packBitsLE over 64 unchanged") && ok;
        ok                           = expectEqual(unpackBitsLE(buf.data(), 0, 1, 64), 0ULL, "unpackBitsLE over 64") && ok;
    }

    return ok;
}
