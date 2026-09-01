#include "test_login_delete_character_packet_8720.h"

#include "login/login_packets.h"
#include "omega_self_test_registry.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "login delete-character packet 8720 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLoginDeleteCharacterPacket8720SelfTests() -> bool
{
    bool ok = true;
    ok      = expect(sizeof(lpkt_deletechr) == 52, "fixed packet size") && ok;
    ok      = expect(offsetof(lpkt_deletechr, packet_size) == 0, "packet size offset") && ok;
    ok      = expect(offsetof(lpkt_deletechr, identifer) == 12, "identifier offset") && ok;
    ok      = expect(offsetof(lpkt_deletechr, ffxi_id) == 28, "character ID offset") && ok;
    ok      = expect(offsetof(lpkt_deletechr, ffxi_id_world) == 32, "world character ID offset") && ok;
    ok      = expect(offsetof(lpkt_deletechr, passwd) == 36, "password offset") && ok;

    auto packet          = lpkt_deletechr{};
    packet.packet_size   = 52;
    packet.terminator    = loginPackets::getTerminator();
    packet.command       = 0x14;
    packet.ffxi_id       = 0xAB12CD34;
    packet.ffxi_id_world = 0x10203040;
    for (std::size_t i = 0; i < std::size(packet.passwd); ++i)
    {
        packet.passwd[i] = static_cast<std::uint8_t>(0xE0 + i);
    }

    auto bytes = std::array<std::uint8_t, sizeof(packet)> {};
    std::memcpy(bytes.data(), &packet, bytes.size());
    ok = expect(bytes[28] == 0x34 && bytes[29] == 0xCD && bytes[30] == 0x12 && bytes[31] == 0xAB,
                "little-endian character ID") &&
         ok;
    ok = expect(bytes[32] == 0x40 && bytes[33] == 0x30 && bytes[34] == 0x20 && bytes[35] == 0x10,
                "little-endian world character ID") &&
         ok;
    ok = expect(bytes[36] == 0xE0 && bytes[51] == 0xEF, "raw password bytes") && ok;

    return ok;
}

OMEGA_REGISTER_SELF_TEST("login-delete-character-packet-8720", runLoginDeleteCharacterPacket8720SelfTests);
