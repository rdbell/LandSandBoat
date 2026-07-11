#include "test_login_view_lobby_ack_1317.h"

#include "common/md52.h"
#include "login/view_lobby_ack.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "login view lobby ack 1317 self-test failed: " << label << '\n';
    }
    return condition;
}

auto expectEqualBytes(const uint8* actual, const uint8* expected, const std::size_t size, const char* label) -> bool
{
    if (std::memcmp(actual, expected, size) != 0)
    {
        std::cerr << "login view lobby ack 1317 self-test failed: " << label << '\n';
        return false;
    }
    return true;
}

} // namespace

auto runLoginViewLobbyAck1317SelfTests() -> bool
{
    using gate = loginHelpers::character_deletion_gate;
    bool ok    = true;

    ok = expect(loginHelpers::ViewLobbyAckPacketSize == 0x20, "packet size") && ok;
    ok = expect(loginHelpers::ViewLobbyAckResult == 0x03, "result byte") && ok;
    ok = expect(loginHelpers::DeleteKeyIncrement == 4, "delete key bump") && ok;

    ok = expect(loginHelpers::ClassifyCharacterDeletionGate(true) == gate::ALLOW, "deletion enabled") && ok;
    ok = expect(loginHelpers::ClassifyCharacterDeletionGate(false) == gate::DENIED, "deletion disabled") && ok;

    std::array<uint8, loginHelpers::ViewLobbyAckPacketSize> packet{};
    // Poison then regenerate to prove full overwrite.
    packet.fill(0xAB);
    loginHelpers::GenerateViewLobbyAckPacket(packet.data());

    ok = expect(packet[0] == 0x20, "size field") && ok;
    ok = expect(packet[1] == 0 && packet[2] == 0 && packet[3] == 0, "size high bytes zero") && ok;
    ok = expect(packet[4] == 'I' && packet[5] == 'X' && packet[6] == 'F' && packet[7] == 'F', "IXFF terminator") && ok;
    ok = expect(packet[8] == 0x03, "result at offset 8") && ok;
    ok = expect(packet[9] == 0 && packet[10] == 0 && packet[11] == 0, "padding before hash") && ok;

    // Independent MD5 over the pre-hash frame (zeros in identifier region).
    std::array<uint8, loginHelpers::ViewLobbyAckPacketSize> preHash{};
    preHash[0] = 0x20;
    preHash[4] = 'I';
    preHash[5] = 'X';
    preHash[6] = 'F';
    preHash[7] = 'F';
    preHash[8] = 0x03;
    uint8 wantHash[16];
    md5(preHash.data(), wantHash, static_cast<int32>(loginHelpers::ViewLobbyAckPacketSize));
    ok = expectEqualBytes(packet.data() + 12, wantHash, 16, "MD5 identifier") && ok;

    // Golden full packet: preHash with hash spliced at 12.
    std::array<uint8, loginHelpers::ViewLobbyAckPacketSize> golden = preHash;
    std::memcpy(golden.data() + 12, wantHash, 16);
    ok = expectEqualBytes(packet.data(), golden.data(), loginHelpers::ViewLobbyAckPacketSize, "full golden packet") && ok;

    // Second generation is stable.
    std::array<uint8, loginHelpers::ViewLobbyAckPacketSize> packet2{};
    loginHelpers::GenerateViewLobbyAckPacket(packet2.data());
    ok = expectEqualBytes(packet.data(), packet2.data(), loginHelpers::ViewLobbyAckPacketSize, "deterministic") && ok;

    return ok;
}
