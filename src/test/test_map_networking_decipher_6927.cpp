#include "test_map_networking_decipher_6927.h"

#include "map/map_networking_decipher.h"

#include <common/blowfish.h>
#include <common/md52.h>
#include <common/mmo.h>

#include <algorithm>
#include <array>
#include <cstring>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map networking decipher 6927 self-test failed: " << label << '\n';
    }
    return condition;
}

auto initTestCipher(blowfish_t& cipher) -> void
{
    auto key = std::array<int8, 16>{};
    for (std::size_t index = 0; index < key.size(); ++index)
    {
        key[index] = static_cast<int8>(index);
    }
    blowfish_init(key.data(), static_cast<int16>(key.size()), cipher.P, cipher.S[0]);
}

} // namespace

auto runMapNetworkingDecipher6927SelfTests() -> bool
{
    blowfish_t cipher{};
    initTestCipher(cipher);

    alignas(uint32) auto packet = std::array<uint8, FFXI_HEADER_SIZE + 32>{};
    std::fill(packet.begin(), packet.begin() + FFXI_HEADER_SIZE, 0xA5);
    for (std::size_t index = FFXI_HEADER_SIZE; index < packet.size() - 16; ++index)
    {
        packet[index] = static_cast<uint8>(index);
    }
    md5(packet.data() + FFXI_HEADER_SIZE, packet.data() + packet.size() - 16, 16);

    const auto plain = packet;
    blowfish_encipher_blocks(reinterpret_cast<uint32*>(packet.data() + FFXI_HEADER_SIZE), 4, cipher.P, cipher.S[0]);

    bool ok = true;
    ok      = expect(mapnetworkingdecipherhelpers::DecipherAndVerify(packet, &cipher), "valid encrypted envelope") && ok;
    ok      = expect(packet == plain, "valid envelope restores all bytes") && ok;

    blowfish_encipher_blocks(reinterpret_cast<uint32*>(packet.data() + FFXI_HEADER_SIZE), 4, cipher.P, cipher.S[0]);
    packet[FFXI_HEADER_SIZE] ^= 0x01;
    ok = expect(!mapnetworkingdecipherhelpers::DecipherAndVerify(packet, &cipher), "tampered encrypted payload rejects") && ok;

    auto shortPacket = std::array<uint8, FFXI_HEADER_SIZE + 15>{};
    ok               = expect(!mapnetworkingdecipherhelpers::DecipherAndVerify(shortPacket, &cipher), "short envelope rejects") && ok;
    ok               = expect(!mapnetworkingdecipherhelpers::DecipherAndVerify(packet, nullptr), "nil key rejects") && ok;

    return ok;
}
