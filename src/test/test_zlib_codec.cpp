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

#include "test_zlib_codec.h"

#include "common/zlib.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace
{

template <typename T, typename U>
auto expectEqual(const T actual, const U expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "zlib codec self-test failed: " << label << " got " << actual << " expected " << expected << '\n';
        return false;
    }

    return true;
}

template <typename T, std::size_t Size>
auto expectBytes(const T* actual, const std::array<uint8, Size>& expected, const std::string& label) -> bool
{
    if (std::memcmp(actual, expected.data(), expected.size()) != 0)
    {
        std::cerr << "zlib codec self-test failed: " << label << " got";
        for (std::size_t i = 0; i < expected.size(); ++i)
        {
            std::cerr << ' ' << static_cast<unsigned>(static_cast<uint8>(actual[i]));
        }
        std::cerr << " expected";
        for (const auto byte : expected)
        {
            std::cerr << ' ' << static_cast<unsigned>(byte);
        }
        std::cerr << '\n';
        return false;
    }

    return true;
}

auto expectVectorPrefix(const std::vector<int8>& actual, const std::vector<uint8>& expected, const std::string& label) -> bool
{
    if (actual.size() < expected.size() || std::memcmp(actual.data(), expected.data(), expected.size()) != 0)
    {
        std::cerr << "zlib codec self-test failed: " << label << '\n';
        return false;
    }

    return true;
}

auto byteRange(const std::size_t size) -> std::vector<uint8>
{
    std::vector<uint8> output(size);
    for (std::size_t i = 0; i < output.size(); ++i)
    {
        output[i] = static_cast<uint8>(i);
    }
    return output;
}

auto compressBytes(const std::vector<uint8>& input, std::vector<int8>& output) -> int32
{
    return zlib_compress(reinterpret_cast<const int8*>(input.data()), static_cast<uint32>(input.size()), output.data(), static_cast<uint32>(output.size()));
}

auto decompressBytes(const std::vector<int8>& input, const int32 bitSize, std::vector<int8>& output) -> int32
{
    return zlib_decompress(input.data(), static_cast<uint32>(bitSize), output.data(), static_cast<uint32>(output.size()));
}

} // namespace

auto runZlibCodecSelfTests() -> bool
{
    bool ok = true;

    ok = expectEqual(zlib_init(), 0, "zlib_init") && ok;

    {
        const auto        input = std::vector<uint8>{ 'O', 'm', 'e', 'g', 'a', 'X', 'I' };
        std::vector<int8> compressed(64);
        std::vector<int8> decompressed(32);

        const auto bits = compressBytes(input, compressed);
        ok              = expectEqual(bits, 83, "OmegaXI compressed bit size") && ok;
        ok              = expectBytes(compressed.data(), std::array<uint8, 11>{ 0x01, 0xC6, 0x73, 0x18, 0x32, 0x44, 0x20, 0x40, 0x7F, 0x0E, 0x07 }, "OmegaXI compressed bytes") && ok;

        const auto bytes = decompressBytes(compressed, bits - 8, decompressed);
        ok               = expectEqual(bytes, 7, "OmegaXI decompressed byte size") && ok;
        ok               = expectVectorPrefix(decompressed, input, "OmegaXI decompressed bytes") && ok;
    }

    {
        const auto        input = byteRange(16);
        std::vector<int8> compressed(128);
        std::vector<int8> decompressed(32);

        const auto bits = compressBytes(input, compressed);
        ok              = expectEqual(bits, 128, "byte range compressed bit size") && ok;
        ok              = expectBytes(compressed.data(), std::array<uint8, 16>{ 0x01, 0x95, 0xEB, 0x34, 0x01, 0xF7, 0x3A, 0x22, 0xA1, 0x35, 0x1B, 0x05, 0x4C, 0x7E, 0x83, 0x47 }, "byte range compressed bytes") && ok;

        const auto bytes = decompressBytes(compressed, bits - 8, decompressed);
        ok               = expectEqual(bytes, 16, "byte range decompressed byte size") && ok;
        ok               = expectVectorPrefix(decompressed, input, "byte range decompressed bytes") && ok;
    }

    {
        const auto        input = std::vector<uint8>{ 0x00, 0xFF, 0x10, 0x80, 0x7F, 0x20 };
        std::vector<int8> compressed(64);
        std::vector<int8> decompressed(32);

        const auto bits = compressBytes(input, compressed);
        ok              = expectEqual(bits, 57, "high bit compressed bit size") && ok;
        ok              = expectBytes(compressed.data(), std::array<uint8, 8>{ 0x01, 0x45, 0x03, 0xA0, 0xF5, 0x87, 0xEF, 0x01 }, "high bit compressed bytes") && ok;

        const auto bytes = decompressBytes(compressed, bits - 8, decompressed);
        ok               = expectEqual(bytes, 6, "high bit decompressed byte size") && ok;
        ok               = expectVectorPrefix(decompressed, input, "high bit decompressed bytes") && ok;
    }

    {
        std::vector<int8> invalid{ 0x00, 0x01, 0x02 };
        std::vector<int8> output(16);
        ok = expectEqual(decompressBytes(invalid, 16, output), -1, "invalid marker") && ok;
    }

    return ok;
}
