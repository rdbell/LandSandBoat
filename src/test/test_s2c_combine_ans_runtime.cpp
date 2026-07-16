#include "test_s2c_combine_ans_runtime.h"
#include <iostream>
#include "map/packets/s2c/combine_ans_runtime.h"
auto runS2CCombineAnsRuntimeSelfTests() -> bool
{
    auto packet = GP_SERV_COMMAND_COMBINE_ANS::PacketData{};
    combineanshelpers::ApplyActiveSynth(packet, {});
    if (packet.CrystalNo != 0 || packet.UpKind[0] != 0) { std::cerr << "s2c COMBINE_ANS runtime self-test failed: no synth\n"; return false; }
    auto facts = combineanshelpers::Facts{ .hasActiveSynth = true, .requiredSkills = { 5, 9, 9, 4, 7, 2, 1, 0 }, .crystalID = 0x1234 };
    facts.ingredients[0] = { .itemID = 10, .broken = true };
    facts.ingredients[1] = { .itemID = 20, .broken = false };
    facts.ingredients[7] = { .itemID = 80, .broken = true };
    combineanshelpers::ApplyActiveSynth(packet, facts);
    if (packet.UpKind[0] != 50 || packet.UpKind[1] != 51 || packet.UpKind[2] != 53 || packet.UpKind[3] != 49 || packet.CrystalNo != 0x1234 || packet.MaterialNo[0] != 10 || packet.MaterialNo[1] != 20 || packet.MaterialNo[7] != 80 || packet.BreakNo[0] != 10 || packet.BreakNo[1] != 0 || packet.BreakNo[7] != 80) { std::cerr << "s2c COMBINE_ANS runtime self-test failed: synth details\n"; return false; }
    return true;
}
