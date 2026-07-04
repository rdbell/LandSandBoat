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

#include "test_xi_bitset.h"

#include "common/xi.h"

#include <array>
#include <cstddef>
#include <iostream>
#include <string>

namespace
{

template <typename T, typename U>
auto expectEqual(const T actual, const U expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "xi bitset self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

template <std::size_t N>
auto expectBytes(const xi::bitset<N>& actual, const std::array<uint8, xi::bitset<N>::storage_size>& expected, const std::string& label) -> bool
{
    if (actual.data != expected)
    {
        std::cerr << "xi bitset self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto testSetResetAndStorage() -> bool
{
    bool ok = true;

    xi::bitset<10> bits{};
    ok = expectEqual(bits.size(), std::size_t{ 10 }, "size") && ok;
    ok = expectEqual(xi::bitset<10>::storage_size, std::size_t{ 2 }, "storage size") && ok;
    ok = expectEqual(bits.none(), true, "new bitset none") && ok;

    bits.set(0);
    bits.set(9);
    bits.set(3);
    ok = expectEqual(bits.get(0), true, "get bit 0") && ok;
    ok = expectEqual(bits.test(3), true, "test bit 3") && ok;
    ok = expectEqual(bits[9], true, "operator[] read bit 9") && ok;
    ok = expectBytes(bits, std::array<uint8, 2>{ 0x09, 0x02 }, "set bytes") && ok;

    bits.set(3, false);
    ok = expectBytes(bits, std::array<uint8, 2>{ 0x01, 0x02 }, "set false bytes") && ok;

    bits.reset(9);
    ok = expectBytes(bits, std::array<uint8, 2>{ 0x01, 0x00 }, "reset pos bytes") && ok;

    bits[4] = true;
    ok      = expectEqual(static_cast<bool>(bits[4]), true, "operator[] write true") && ok;
    bits[4] = false;
    ok      = expectEqual(static_cast<bool>(bits[4]), false, "operator[] write false") && ok;

    bits.reset();
    ok = expectEqual(bits.none(), true, "reset none") && ok;
    ok = expectBytes(bits, std::array<uint8, 2>{ 0x00, 0x00 }, "reset bytes") && ok;

    return ok;
}

auto testFlipAndPadding() -> bool
{
    bool ok = true;

    xi::bitset<10> bits{};
    bits.set(1);
    bits.flip();
    ok = expectBytes(bits, std::array<uint8, 2>{ 0xFD, 0xFF }, "flip includes padding bytes") && ok;
    ok = expectEqual(bits.get(1), false, "flipped bit 1") && ok;
    ok = expectEqual(bits.get(9), true, "flipped bit 9") && ok;

    bits.flip(9);
    ok = expectBytes(bits, std::array<uint8, 2>{ 0xFD, 0xFD }, "flip pos bytes") && ok;

    const auto inverted = ~bits;
    ok                  = expectBytes(inverted, std::array<uint8, 2>{ 0x02, 0x02 }, "operator~ bytes") && ok;

    return ok;
}

auto testAndOperations() -> bool
{
    bool ok = true;

    xi::bitset<10> left{};
    xi::bitset<10> right{};
    left.data  = std::array<uint8, 2>{ 0xAA, 0xF0 };
    right.data = std::array<uint8, 2>{ 0x0F, 0xFF };

    const auto combined = left & right;
    ok                  = expectBytes(combined, std::array<uint8, 2>{ 0x0A, 0xF0 }, "operator& bytes") && ok;

    left &= right;
    ok = expectBytes(left, std::array<uint8, 2>{ 0x0A, 0xF0 }, "operator&= bytes") && ok;

    return ok;
}

auto testEmptyBitset() -> bool
{
    bool ok = true;

    xi::bitset<0> empty{};
    ok = expectEqual(empty.size(), std::size_t{ 0 }, "empty size") && ok;
    ok = expectEqual(xi::bitset<0>::storage_size, std::size_t{ 0 }, "empty storage size") && ok;
    ok = expectEqual(empty.none(), true, "empty none") && ok;
    empty.flip();
    ok = expectBytes(empty, std::array<uint8, 0>{}, "empty flip bytes") && ok;
    ok = expectBytes(~empty, std::array<uint8, 0>{}, "empty operator~ bytes") && ok;

    return ok;
}

} // namespace

auto runXiBitsetSelfTests() -> bool
{
    bool ok = true;

    ok = testSetResetAndStorage() && ok;
    ok = testFlipAndPadding() && ok;
    ok = testAndOperations() && ok;
    ok = testEmptyBitset() && ok;

    return ok;
}
