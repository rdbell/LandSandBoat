#pragma once

#include <common/blowfish.h>
#include <common/mmo.h>
#include <common/utils.h>

#include <cstddef>
#include <span>

// Pure in-place map-envelope decipher/authentication helper shared by the
// legacy recv_parse path and protocol self-tests.
namespace mapnetworkingdecipherhelpers
{

inline auto DecipherAndVerify(std::span<uint8> packet, blowfish_t* pbfkey) -> bool
{
    constexpr auto digestSize = std::size_t{ 16 };
    if (pbfkey == nullptr || packet.size() < FFXI_HEADER_SIZE + digestSize)
    {
        return false;
    }

    const auto encryptedBlockCount = (packet.size() - FFXI_HEADER_SIZE) / 8;
    blowfish_decipher_blocks(
        reinterpret_cast<uint32*>(packet.data() + FFXI_HEADER_SIZE),
        encryptedBlockCount,
        pbfkey->P,
        pbfkey->S[0]);

    const auto payloadSize = packet.size() - FFXI_HEADER_SIZE - digestSize;
    return checksum(
               packet.data() + FFXI_HEADER_SIZE,
               static_cast<uint32>(payloadSize),
               reinterpret_cast<char*>(packet.data() + packet.size() - digestSize)) == 0;
}

} // namespace mapnetworkingdecipherhelpers
