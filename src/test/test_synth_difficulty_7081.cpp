#include "test_synth_difficulty_7081.h"

#include "map/synth_difficulty.h"

#include <iostream>

auto runSynthDifficulty7081SelfTests() -> bool
{
    using synthdifficultyhelpers::Calculate;
    using synthdifficultyhelpers::ModForSkill;

    const bool ok = ModForSkill(49) == Mod::WOOD &&
                    ModForSkill(50) == Mod::SMITH &&
                    ModForSkill(51) == Mod::GOLDSMITH &&
                    ModForSkill(52) == Mod::CLOTH &&
                    ModForSkill(53) == Mod::LEATHER &&
                    ModForSkill(54) == Mod::BONE &&
                    ModForSkill(55) == Mod::ALCHEMY &&
                    ModForSkill(56) == Mod::COOK &&
                    ModForSkill(0) == Mod::NONE &&
                    Calculate(100, 1099, 5) == -14 &&
                    Calculate(100, 2560, 0) == 100 &&
                    Calculate(100, 1099, -5) == -4;
    if (!ok)
    {
        std::cerr << "synth difficulty 7081 self-test failed\n";
    }
    return ok;
}
