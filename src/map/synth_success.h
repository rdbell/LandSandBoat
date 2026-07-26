#pragma once

#include "enums/synthesis_result.h"

namespace synthsuccesshelpers
{

constexpr auto MessageForCraftMode(const bool isDesynthesis) -> SynthesisResult
{
    return isDesynthesis ? SynthesisResult::SuccessDesynth : SynthesisResult::Success;
}

} // namespace synthsuccesshelpers
