#include "test_map_networking_compression_failure_2720.h"

#include "map/map_networking_capacity.h"

#include <iostream>

auto runMapNetworkingCompressionFailure2720SelfTests() -> bool
{
    using mapnetworkinghelpers::CompressionFailurePlan;
    using mapnetworkinghelpers::PlanCompressionFailure;

    const bool ok = PlanCompressionFailure(true) == CompressionFailurePlan::DropOldestPacketAndRetry &&
                    PlanCompressionFailure(false) == CompressionFailurePlan::ClearOutputAndFail;
    if (!ok)
    {
        std::cerr << "map networking compression failure 2720 self-test failed\n";
    }
    return ok;
}
