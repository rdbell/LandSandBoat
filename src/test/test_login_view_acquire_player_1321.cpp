#include "test_login_view_acquire_player_1321.h"

#include "common/cbasetypes.h"
#include "login/character_select.h"

#include <array>
#include <cstdint>
#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "login view acquire player 1321 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runLoginViewAcquirePlayer1321SelfTests() -> bool
{
    using gate = loginHelpers::data_session_presence_gate;
    bool ok    = true;

    ok = expect(loginHelpers::DataSessionNotifyPacketSize == 0x05, "notify size") && ok;
    ok = expect(loginHelpers::DataAcquirePlayerNotifyCommand == 0x01, "acquire command") && ok;
    ok = expect(loginHelpers::DataSelectNotifyCommand == 0x02, "select command distinct") && ok;

    ok = expect(loginHelpers::ClassifyDataSessionPresence(true) == gate::PRESENT, "present") && ok;
    ok = expect(loginHelpers::ClassifyDataSessionPresence(false) == gate::MISSING, "missing") && ok;

    std::array<uint8, loginHelpers::DataSessionNotifyPacketSize> packet{};
    packet.fill(0xFF);
    loginHelpers::GenerateDataAcquirePlayerNotifyPacket(packet.data());
    ok = expect(packet[0] == 0x01, "acquire byte 0") && ok;
    ok = expect(packet[1] == 0 && packet[2] == 0 && packet[3] == 0 && packet[4] == 0, "acquire rest zero") && ok;

    // Select notify remains command 0x02 and does not collide.
    std::array<uint8, loginHelpers::DataSessionNotifyPacketSize> select{};
    select.fill(0xAB);
    loginHelpers::GenerateDataSelectNotifyPacket(select.data());
    ok = expect(select[0] == 0x02, "select still 0x02") && ok;
    ok = expect(select[1] == 0 && select[2] == 0 && select[3] == 0 && select[4] == 0, "select rest zero") && ok;

    // Generic builder.
    std::array<uint8, loginHelpers::DataSessionNotifyPacketSize> generic{};
    loginHelpers::GenerateDataSessionNotifyPacket(generic.data(), 0x7F);
    ok = expect(generic[0] == 0x7F, "generic command") && ok;
    ok = expect(generic[1] == 0 && generic[4] == 0, "generic zero pad") && ok;

    return ok;
}
