#include "test_status_effect_id_range_2932.h"

#include "map/status_effect_capacity.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "status effect ShouldRejectEffectIDOutOfRange 2932 self-test failed: " << label << '\n';
    }
    return condition;
}

// Inline status-effect ID range gate for dual-wire cross-check (slice 2932):
//   statusID >= maxEffectID
auto inlineShouldRejectEffectIDOutOfRange(const uint16 statusID, const uint16 maxEffectID) -> bool
{
    return statusID >= maxEffectID;
}

} // namespace

// Pure dual-wire expansion for statuseffecthelpers::ShouldRejectEffectIDOutOfRange
// (statusID >= MAX_EFFECTID; slice 2932).
auto runStatusEffectIDRange2932SelfTests() -> bool
{
    using statuseffecthelpers::MaxEffectID;
    using statuseffecthelpers::ShouldRejectEffectIDOutOfRange;

    bool ok = true;

    ok = expect(MaxEffectID == 814, "MaxEffectID pin == 814") && ok;

    const struct
    {
        uint16      statusID;
        uint16      maxEffectID;
        bool        want;
        const char* label;
    } cases[] = {
        // Classic poles with production MaxEffectID pin.
        { 0, MaxEffectID, false, "zero id allows" },
        { static_cast<uint16>(MaxEffectID - 1), MaxEffectID, false, "last valid id allows" },
        { MaxEffectID, MaxEffectID, true, "max boundary rejects" },
        { static_cast<uint16>(MaxEffectID + 1), MaxEffectID, true, "above max rejects" },

        // Residual 1369 pins.
        { MaxEffectID, MaxEffectID, true, "residual max id" },
        { 0, MaxEffectID, false, "residual ok id" },

        // Injected maxEffectID.
        { 10, 10, true, "custom max boundary rejects" },
        { 9, 10, false, "custom below max allows" },
        { 11, 10, true, "custom above max rejects" },
        { 814, 814, true, "literal 814 pin rejects" },
        { 813, 814, false, "literal 813 allows" },
        { 815, 814, true, "literal 815 rejects" },
    };

    for (const auto& c : cases)
    {
        const bool got     = ShouldRejectEffectIDOutOfRange(c.statusID, c.maxEffectID);
        const bool inlineF = inlineShouldRejectEffectIDOutOfRange(c.statusID, c.maxEffectID);
        const bool wantPin = c.statusID >= c.maxEffectID;

        ok = expect(got == c.want, c.label) && ok;
        ok = expect(got == inlineF, "ShouldRejectEffectIDOutOfRange dual-wire == inline LSB formula") && ok;
        ok = expect(got == wantPin, "ShouldRejectEffectIDOutOfRange == pin formula statusID >= maxEffectID") && ok;
    }

    // Pin composition: boundary identity with production MaxEffectID.
    ok = expect(ShouldRejectEffectIDOutOfRange(MaxEffectID, MaxEffectID), "statusID == MaxEffectID must reject") && ok;
    ok = expect(!ShouldRejectEffectIDOutOfRange(static_cast<uint16>(MaxEffectID - 1), MaxEffectID),
                "statusID == MaxEffectID-1 must not reject") &&
         ok;
    ok = expect(ShouldRejectEffectIDOutOfRange(static_cast<uint16>(MaxEffectID + 1), MaxEffectID),
                "statusID == MaxEffectID+1 must reject") &&
         ok;
    ok = expect(!ShouldRejectEffectIDOutOfRange(0, MaxEffectID), "statusID == 0 must not reject") && ok;

    // Dense compose around MaxEffectID boundary (production pin domain sample).
    const uint16 sampleIDs[] = { 0, 1, 251, 255, 813, 814, 815, 999 };
    for (const uint16 statusID : sampleIDs)
    {
        const bool got  = ShouldRejectEffectIDOutOfRange(statusID, MaxEffectID);
        const bool want = statusID >= MaxEffectID;
        ok              = expect(got == want, "compose free == pin formula") && ok;
        ok              = expect(got == inlineShouldRejectEffectIDOutOfRange(statusID, MaxEffectID),
                    "compose free == inline") &&
             ok;
    }

    // Host-style compose: reject when statusID is out of [0, MaxEffectID).
    // Production injects statuseffecthelpers::MaxEffectID at both call sites.
    const struct
    {
        uint16      statusID;
        bool        wantReject;
        const char* label;
    } composeCases[] = {
        { 0, false, "KO id allows" },
        { 3, false, "poison id allows" },
        { 255, false, "None id allows" },
        { 813, false, "last catalog slot allows" },
        { 814, true, "MAX_EFFECTID boundary rejects" },
        { 815, true, "first past max rejects" },
    };

    for (const auto& c : composeCases)
    {
        const bool got = ShouldRejectEffectIDOutOfRange(c.statusID, MaxEffectID);
        ok             = expect(got == c.wantReject, c.label) && ok;
        ok             = expect(got == (c.statusID >= MaxEffectID), "compose free == statusID >= MaxEffectID pin") && ok;
        ok             = expect(got == inlineShouldRejectEffectIDOutOfRange(c.statusID, MaxEffectID), "compose free == inline") && ok;
    }

    return ok;
}
