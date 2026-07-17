#include "test_map_compress_failure_2788.h"

#include "map/map_networking_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map compress failure 2788 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runMapCompressFailure2788SelfTests() -> bool
{
    using mapnetworkinghelpers::CompressPacketResultPlan;
    using mapnetworkinghelpers::CompressSucceeded;
    using mapnetworkinghelpers::PlanCompressPacket;
    using mapnetworkinghelpers::ShouldRejectCompressFailure;

    bool ok = true;

    // PlanCompressPacket(zlibFailed): Reject when zlib returned -1, else Proceed
    ok = expect(PlanCompressPacket(true) == CompressPacketResultPlan::Reject, "zlibFailed true -> Reject") && ok;
    ok = expect(PlanCompressPacket(false) == CompressPacketResultPlan::Proceed, "zlibFailed false -> Proceed") && ok;

    // ShouldRejectCompressFailure is the bool form of the same gate
    ok = expect(ShouldRejectCompressFailure(true), "reject when result == -1") && ok;
    ok = expect(!ShouldRejectCompressFailure(false), "do not reject when result != -1") && ok;

    // CompressSucceeded: result != -1
    ok = expect(!CompressSucceeded(-1), "failed sentinel is not success") && ok;
    ok = expect(CompressSucceeded(0), "0 bit-size result succeeds") && ok;
    ok = expect(CompressSucceeded(1), "positive result succeeds") && ok;
    ok = expect(CompressSucceeded(8), "minimum useful bit count succeeds") && ok;
    ok = expect(CompressSucceeded(-2), "only -1 is the failure sentinel") && ok;

    // compressPacket early-return path: Reject <=> return nullopt
    // Host wires: if (PlanCompressPacket(result == -1) == Reject) return nullopt;
    {
        constexpr int32 failedResult = -1;
        ok = expect(PlanCompressPacket(failedResult == -1) == CompressPacketResultPlan::Reject, "host compare failedResult") && ok;
        ok = expect(ShouldRejectCompressFailure(failedResult == -1), "host compare failedResult bool") && ok;
        ok = expect(!CompressSucceeded(failedResult), "host CompressSucceeded failedResult") && ok;
    }
    {
        constexpr int32 okResult = 64;
        ok = expect(PlanCompressPacket(okResult == -1) == CompressPacketResultPlan::Proceed, "host compare okResult") && ok;
        ok = expect(!ShouldRejectCompressFailure(okResult == -1), "host compare okResult bool") && ok;
        ok = expect(CompressSucceeded(okResult), "host CompressSucceeded okResult") && ok;
    }

    return ok;
}
