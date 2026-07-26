#pragma once

#include "common/cbasetypes.h"

namespace synthresultdispatchhelpers
{

constexpr auto PacketResult(const uint8 synthResult) -> uint8
{
    switch (synthResult)
    {
        case 0:       // SYNTHESIS_FAIL
            return 1; // RESULT_FAIL
        case 1:       // SYNTHESIS_SUCCESS
            return 0; // RESULT_SUCCESS
        case 2:       // SYNTHESIS_HQ
        case 3:       // SYNTHESIS_HQ2
        case 4:       // SYNTHESIS_HQ3
            return 2; // RESULT_HQ
        default:
            return synthResult;
    }
}

} // namespace synthresultdispatchhelpers
