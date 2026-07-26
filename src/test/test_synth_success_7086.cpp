#include "test_synth_success_7086.h"

#include "map/synth_success.h"

#include <iostream>

auto runSynthSuccess7086SelfTests() -> bool
{
    using synthsuccesshelpers::MessageForCraftMode;

    const bool ok = MessageForCraftMode(false) == SynthesisResult::Success &&
                    MessageForCraftMode(true) == SynthesisResult::SuccessDesynth;
    if (!ok)
    {
        std::cerr << "synth success 7086 self-test failed\n";
    }
    return ok;
}
