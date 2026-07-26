#include "test_synth_speed_7090.h"

#include "map/ai/states/synth_update.h"

#include <iostream>

auto runSynthSpeed7090SelfTests() -> bool
{
    using synthupdate::speedModID;

    const bool ok = speedModID(49) == Mod::SYNTH_SPEED_WOODWORKING && speedModID(50) == Mod::SYNTH_SPEED_SMITHING &&
                    speedModID(51) == Mod::SYNTH_SPEED_GOLDSMITHING && speedModID(52) == Mod::SYNTH_SPEED_CLOTHCRAFT &&
                    speedModID(53) == Mod::SYNTH_SPEED_LEATHERCRAFT && speedModID(54) == Mod::SYNTH_SPEED_BONECRAFT &&
                    speedModID(55) == Mod::SYNTH_SPEED_ALCHEMY && speedModID(56) == Mod::SYNTH_SPEED_COOKING &&
                    speedModID(0) == Mod::NONE;
    if (!ok)
    {
        std::cerr << "synth speed 7090 self-test failed\n";
    }
    return ok;
}
