#include "test_synth_result_dispatch_7084.h"

#include "map/synth_result_dispatch.h"

#include <iostream>

auto runSynthResultDispatch7084SelfTests() -> bool
{
    using synthresultdispatchhelpers::PacketResult;

    const bool ok = PacketResult(0) == 1 && PacketResult(1) == 0 && PacketResult(2) == 2 &&
                    PacketResult(3) == 2 && PacketResult(4) == 2 && PacketResult(99) == 99;
    if (!ok)
    {
        std::cerr << "synth result dispatch 7084 self-test failed\n";
    }
    return ok;
}
