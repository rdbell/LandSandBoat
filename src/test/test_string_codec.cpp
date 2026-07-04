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

#include "test_string_codec.h"

#include "common/utils.h"

#include <array>
#include <cstring>
#include <iostream>
#include <string>

namespace
{

template <std::size_t N>
auto expectBytes(const std::array<uint8, N>& actual, const std::array<uint8, N>& expected, const std::string& label) -> bool
{
    if (actual != expected)
    {
        std::cerr << "string codec self-test failed: " << label << " byte mismatch\n";
        return false;
    }

    return true;
}

template <std::size_t N>
auto fixedBytes(const std::string& value) -> std::array<uint8, N>
{
    std::array<uint8, N> output{};
    std::memcpy(output.data(), value.data(), std::min(value.size(), output.size()));
    return output;
}

auto encodeLinkshell(const std::string& value) -> std::array<uint8, LinkshellStringLength>
{
    std::array<uint8, LinkshellStringLength> encoded{};
    EncodeStringLinkshell(value, reinterpret_cast<char*>(encoded.data()));
    return encoded;
}

auto decodeLinkshell(const std::array<uint8, LinkshellStringLength>& encoded) -> std::array<uint8, LinkshellStringLength>
{
    std::array<uint8, LinkshellStringLength> decoded{};
    DecodeStringLinkshell(std::string(reinterpret_cast<const char*>(encoded.data()), encoded.size()), reinterpret_cast<char*>(decoded.data()));
    return decoded;
}

auto encodeSignature(const std::string& value) -> std::array<uint8, SignatureStringLength>
{
    std::array<uint8, SignatureStringLength> encoded{};
    EncodeStringSignature(value, reinterpret_cast<char*>(encoded.data()));
    return encoded;
}

auto decodeSignature(const std::array<uint8, SignatureStringLength>& encoded) -> std::array<uint8, SignatureStringLength>
{
    std::array<uint8, SignatureStringLength> decoded{};
    DecodeStringSignature(std::string(reinterpret_cast<const char*>(encoded.data()), encoded.size()), reinterpret_cast<char*>(decoded.data()));
    return decoded;
}

auto packSoultrapper(const std::string& value) -> std::array<uint8, 14>
{
    std::array<uint8, 14> packed{};
    PackSoultrapperName(value, packed.data());
    return packed;
}

auto unpackSoultrapper(const std::array<uint8, 14>& packed) -> std::string
{
    return UnpackSoultrapperName(packed.data());
}

} // namespace

auto runStringCodecSelfTests() -> bool
{
    bool ok = true;

    const auto linkshellMixed = std::array<uint8, LinkshellStringLength>{
        0xA4, 0xD1, 0x47, 0x07, 0x28, 0xF7, 0xD7, 0x7E, 0xFF,
    };
    ok = expectBytes(encodeLinkshell("OmegaXI2026"), linkshellMixed, "linkshell mixed encode") && ok;
    ok = expectBytes(decodeLinkshell(linkshellMixed), fixedBytes<LinkshellStringLength>("OmegaXI2026"), "linkshell mixed decode") && ok;

    const auto linkshellAlphabet = std::array<uint8, LinkshellStringLength>{
        0x04, 0x20, 0xC4, 0x14, 0x61, 0xC8, 0x24, 0xA2, 0xCC, 0x34,
        0xE3, 0xD0, 0x45, 0x24, 0xD4, 0xFC,
    };
    ok = expectBytes(encodeLinkshell("abcdefghijklmnopqrstuvwxyz"), linkshellAlphabet, "linkshell alphabet encode") && ok;
    ok = expectBytes(decodeLinkshell(linkshellAlphabet), fixedBytes<LinkshellStringLength>("abcdefghijklmnopqrst"), "linkshell alphabet decode") && ok;

    const auto linkshellInvalid = std::array<uint8, LinkshellStringLength>{
        0x6D, 0xC0, 0x03, 0x10, 0x0F,
    };
    ok = expectBytes(encodeLinkshell("AB_cd!"), linkshellInvalid, "linkshell invalid encode") && ok;
    ok = expectBytes(decodeLinkshell(linkshellInvalid), fixedBytes<LinkshellStringLength>("A"), "linkshell invalid decode") && ok;

    const auto linkshellEmpty = std::array<uint8, LinkshellStringLength>{ 0xFC };
    ok = expectBytes(encodeLinkshell(""), linkshellEmpty, "linkshell empty encode") && ok;
    ok = expectBytes(decodeLinkshell(linkshellEmpty), std::array<uint8, LinkshellStringLength>{}, "linkshell empty decode") && ok;

    const auto linkshellEmbeddedNul = std::array<uint8, LinkshellStringLength>{ 0x05 };
    ok = expectBytes(encodeLinkshell("apa"), linkshellEmbeddedNul, "linkshell embedded nul encode") && ok;
    ok = expectBytes(decodeLinkshell(linkshellEmbeddedNul), fixedBytes<LinkshellStringLength>("a"), "linkshell embedded nul decode") && ok;

    const auto signatureMixed = std::array<uint8, SignatureStringLength>{
        0x67, 0x1A, 0x6B, 0x96, 0x24, 0xC3, 0x04, 0x31, 0xC0,
    };
    ok = expectBytes(encodeSignature("OmegaXI2026"), signatureMixed, "signature mixed encode") && ok;
    ok = expectBytes(decodeSignature(signatureMixed), fixedBytes<SignatureStringLength>("OmegaXI2026"), "signature mixed decode") && ok;

    const auto signatureAlphabet = std::array<uint8, SignatureStringLength>{
        0x96, 0x69, 0xE8, 0xA6, 0xAA, 0xEC, 0xB6, 0xEB, 0xF0, 0xC7, 0x2C, 0xC0,
    };
    ok = expectBytes(encodeSignature("abcdefghijklmnopqrstuvwxyz"), signatureAlphabet, "signature alphabet encode") && ok;
    ok = expectBytes(decodeSignature(signatureAlphabet), fixedBytes<SignatureStringLength>("abcdefghijklmno"), "signature alphabet decode") && ok;

    const auto signatureInvalid = std::array<uint8, SignatureStringLength>{
        0x2C, 0xC0, 0x27, 0xA0,
    };
    ok = expectBytes(encodeSignature("AB_cd!"), signatureInvalid, "signature invalid encode") && ok;
    ok = expectBytes(decodeSignature(signatureInvalid), fixedBytes<SignatureStringLength>("AB"), "signature invalid decode") && ok;

    ok = expectBytes(encodeSignature(""), std::array<uint8, SignatureStringLength>{}, "signature empty encode") && ok;
    ok = expectBytes(decodeSignature(std::array<uint8, SignatureStringLength>{}), std::array<uint8, SignatureStringLength>{}, "signature empty decode") && ok;

    const auto signatureEmbeddedNul = std::array<uint8, SignatureStringLength>{ 0x97 };
    ok = expectBytes(encodeSignature("al0"), signatureEmbeddedNul, "signature embedded nul encode") && ok;
    ok = expectBytes(decodeSignature(signatureEmbeddedNul), fixedBytes<SignatureStringLength>("al"), "signature embedded nul decode") && ok;

    const auto goblin = std::array<uint8, 14>{
        0x8F, 0xBF, 0x16, 0xCD, 0x3B, 0xA1, 0x6F, 0xEB, 0xBB, 0xA7, 0x99,
    };
    ok = expectBytes(packSoultrapper("Goblin_Bounty_Hunter"), goblin, "soultrapper goblin pack") && ok;
    ok = (unpackSoultrapper(goblin) == "GoblinBountyH") && ok;

    const auto thunder = std::array<uint8, 14>{
        0xA9, 0xA3, 0xAE, 0xEC, 0x99, 0x79, 0x45, 0xD9, 0x97, 0x6E, 0x5D, 0xDD,
    };
    ok = expectBytes(packSoultrapper("Thunder_Elemental"), thunder, "soultrapper thunder pack") && ok;
    ok = (unpackSoultrapper(thunder) == "ThunderElement") && ok;

    const auto omega = std::array<uint8, 14>{
        0x9F, 0xB7, 0x2E, 0x7C, 0x36, 0x24, 0xB2, 0x60, 0xC9, 0xB0,
    };
    ok = expectBytes(packSoultrapper("OmegaXI2026"), omega, "soultrapper omega pack") && ok;
    ok = (unpackSoultrapper(omega) == "OmegaXI2026") && ok;

    const auto invalidSoul = std::array<uint8, 14>{
        0x85, 0x87, 0x22, 0x19, 0xD8, 0x76, 0xE5,
    };
    ok = expectBytes(packSoultrapper("Bad!Name"), invalidSoul, "soultrapper invalid pack") && ok;
    ok = (unpackSoultrapper(invalidSoul) == "Bad") && ok;

    const auto longSoul = std::array<uint8, 14>{
        0xC3, 0x8B, 0x1E, 0x4C, 0xB9, 0xB3, 0xE8, 0xD3, 0xAB, 0x5E, 0xCD, 0xBB, 0xB7, 0x80,
    };
    ok = expectBytes(packSoultrapper("abcdefghijklmnopqr"), longSoul, "soultrapper long pack") && ok;
    ok = (unpackSoultrapper(longSoul) == "abcdefghijklmno") && ok;

    return ok;
}
