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

auto md5Chunks(const std::vector<std::vector<uint8>>& chunks) -> std::array<uint8, 16>
{
    md5_context ctx{};
    md5_starts(&ctx);
    for (const auto& chunk : chunks)
    {
        md5_update(&ctx, const_cast<uint8*>(chunk.data()), static_cast<uint32>(chunk.size()));
    }

    std::array<uint8, 16> output{};
    md5_finish(&ctx, output.data());
    return output;
}

auto md5RepeatedFinish(std::vector<uint8> input) -> std::array<uint8, 16>
{
    md5_context ctx{};
    md5_starts(&ctx);
    md5_update(&ctx, input.data(), static_cast<uint32>(input.size()));

    std::array<uint8, 16> output{};
    md5_finish(&ctx, output.data());
    md5_finish(&ctx, output.data());
    return output;
}

auto md5UpdateAfterFinish(std::vector<uint8> input, std::vector<uint8> trailingInput) -> std::array<uint8, 16>
{
    md5_context ctx{};
    md5_starts(&ctx);
    md5_update(&ctx, input.data(), static_cast<uint32>(input.size()));

    std::array<uint8, 16> output{};
    md5_finish(&ctx, output.data());
    md5_update(&ctx, trailingInput.data(), static_cast<uint32>(trailingInput.size()));
    md5_finish(&ctx, output.data());
    return output;
}

auto byteRange(std::size_t length) -> std::vector<uint8>
{
    std::vector<uint8> output(length);
    for (std::size_t i = 0; i < length; ++i)
    {
        output[i] = static_cast<uint8>(i);
    }
    return output;
}

auto patternedBytes(std::size_t length) -> std::vector<uint8>
{
    std::vector<uint8> output(length);
    for (std::size_t i = 0; i < length; ++i)
    {
        output[i] = static_cast<uint8>((i * 37 + 11) % 256);
    }
    return output;
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

    ok = expectBytes(md5Chunks({}), std::array<uint8, 16>{
                                        0xD4, 0x1D, 0x8C, 0xD9, 0x8F, 0x00, 0xB2, 0x04,
                                        0xE9, 0x80, 0x09, 0x98, 0xEC, 0xF8, 0x42, 0x7E,
                                    },
                     "md5 streaming empty input") &&
         ok;

    ok = expectBytes(md5Chunks({ { 'O', 'm', 'e' }, { 'g', 'a' }, { 'X', 'I' } }), std::array<uint8, 16>{
                                                                                                  0x0B, 0x07, 0x06, 0xEB, 0xF5, 0x2C, 0x48, 0xF3,
                                                                                                  0x4C, 0xD4, 0x88, 0x3C, 0x6C, 0x10, 0xDF, 0x8F,
                                                                                              },
                     "md5 chunked string") &&
         ok;

    ok = expectBytes(md5Chunks({ {}, { 'a', 'b', 'c' } }), std::array<uint8, 16>{
                                                                             0x90, 0x01, 0x50, 0x98, 0x3C, 0xD2, 0x4F, 0xB0,
                                                                             0xD6, 0x96, 0x3F, 0x7D, 0x28, 0xE1, 0x7F, 0x72,
                                                                         },
                     "md5 empty update") &&
         ok;

    ok = expectBytes(md5Chunks({ patternedBytes(55) }), std::array<uint8, 16>{
                                                                 0xD8, 0x72, 0xAA, 0x04, 0x73, 0xA2, 0x4D, 0xA9,
                                                                 0x95, 0xCE, 0x4A, 0xC5, 0x18, 0xAD, 0xE7, 0x67,
                                                             },
                     "md5 55-byte update") &&
         ok;

    ok = expectBytes(md5Chunks({ patternedBytes(56) }), std::array<uint8, 16>{
                                                                 0xE2, 0x35, 0x67, 0x64, 0x58, 0x46, 0x67, 0x7C,
                                                                 0x20, 0x5D, 0xE8, 0x0F, 0x97, 0x79, 0x08, 0x1B,
                                                             },
                     "md5 56-byte update") &&
         ok;

    ok = expectBytes(md5Chunks({ patternedBytes(63) }), std::array<uint8, 16>{
                                                                 0x47, 0x75, 0xB6, 0x62, 0x78, 0xA8, 0xFC, 0x13,
                                                                 0x2F, 0xF8, 0x09, 0x23, 0x37, 0x82, 0x16, 0xCD,
                                                             },
                     "md5 63-byte update") &&
         ok;

    ok = expectBytes(md5Chunks({ byteRange(64) }), std::array<uint8, 16>{
                                                             0xB2, 0xD3, 0xF5, 0x6B, 0xC1, 0x97, 0xFD, 0x98,
                                                             0x5D, 0x59, 0x65, 0x07, 0x9B, 0x5E, 0x71, 0x48,
                                                         },
                     "md5 64-byte update") &&
         ok;

    ok = expectBytes(md5Chunks({ byteRange(65) }), std::array<uint8, 16>{
                                                             0x8B, 0xD7, 0x05, 0x38, 0x01, 0xC7, 0x68, 0x42,
                                                             0x0F, 0xAF, 0x81, 0x6F, 0xAD, 0xBA, 0x97, 0x1C,
                                                         },
                     "md5 65-byte update") &&
         ok;

    ok = expectBytes(md5Chunks({ patternedBytes(120) }), std::array<uint8, 16>{
                                                                  0xBF, 0x24, 0x0B, 0x8B, 0x74, 0x07, 0xFB, 0xA3,
                                                                  0x80, 0x5A, 0xDE, 0xEC, 0x63, 0xA0, 0x3B, 0x77,
                                                              },
                     "md5 120-byte update") &&
         ok;

    ok = expectBytes(md5Chunks({ { 0x00, 0xFF, 0x10 }, {}, byteRange(64), { 't', 'a', 'i', 'l' } }), std::array<uint8, 16>{
                                                                                                                    0xE3, 0x65, 0x29, 0x5C, 0xCF, 0x26, 0x85, 0x15,
                                                                                                                    0xCD, 0x01, 0x7E, 0x2D, 0x01, 0xC4, 0x61, 0xFF,
                                                                                                                },
                     "md5 mixed chunks") &&
         ok;

    ok = expectBytes(md5RepeatedFinish({ 'a', 'b', 'c' }), std::array<uint8, 16>{
                                                                           0x2E, 0xF3, 0xC9, 0x17, 0xC1, 0xC6, 0xAF, 0xE9,
                                                                           0x1D, 0x4C, 0xC6, 0x6D, 0x87, 0x33, 0xC4, 0x03,
                                                                       },
                     "md5 repeated finish") &&
         ok;

    ok = expectBytes(md5UpdateAfterFinish({ 'a', 'b', 'c' }, { 't', 'a', 'i', 'l' }), std::array<uint8, 16>{
                                                                                                      0xCB, 0x63, 0xBC, 0x19, 0x3E, 0xBC, 0xAF, 0x8F,
                                                                                                      0x40, 0x92, 0x32, 0xCE, 0x91, 0x5A, 0xD0, 0x16,
                                                                                                  },
                     "md5 update after finish") &&
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
