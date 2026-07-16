#include "test_s2c_battlefield_runtime.h"
#include "map/packets/s2c/0x075_battlefield.h"
#include <iostream>

auto runS2CBattlefieldRuntimeSelfTests() -> bool
{
    bool ok = battlefieldhelpers::HelpDescription(18) == 0 && battlefieldhelpers::HelpDescription(19) == 0 && battlefieldhelpers::HelpDescription(20) == 1;
    ok      = battlefieldhelpers::FenceValue(1.5f) == 1500 && ok;
    GP_SERV_COMMAND_BATTLEFIELD packet{};
    packet.addCountdown(60, 10);
    packet.addBars({ { "one", 1 }, { "two", 2 }, { "three", 3 }, { "four", 4 }, { "five", 5 }, { "six", 6 } });
    packet.addScoreboard({ 4, -2 }, { 1, 2, 3, 4, 5, 6 });
    packet.addFence(1.0f, -2.0f, 3.0f, 4.0f, true);
    packet.addHelpText(55, 20);
    const auto& data = packet.data();
    ok               = (data.Flags & (OBJECTIVEUTILITY_COUNTDOWN | OBJECTIVEUTILITY_PROGRESS | OBJECTIVEUTILITY_FENCE | OBJECTIVEUTILITY_HELP)) == 0x0F && data.Mode == 0x1000 && data.Duration == 60 && data.DurationWarn == 10 && data.FenceX == 1000 && data.FenceY == -2000 && data.FenceRadius == 3000 && data.FenceRotation == 4000 && data.FenceColor == 1 && data.MesNumTitle == 55 && data.MesNumDescription == 1 && data.Scoreboard.MarchlandScore == 4 && data.Scoreboard.StrongholdScore == -2 && ok;
    if (!ok)
        std::cerr << "s2c BATTLEFIELD runtime self-test failed\n";
    return ok;
}
