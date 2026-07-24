#include "test_zone_conditional_npc_6257.h"

#include "map/zone_conditional_npc.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "zone conditional NPC 6257 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Pins SpawnConditionalNPCs' production-wired identity and per-player gates.
auto runZoneConditionalNPC6257SelfTests() -> bool
{
    using namespace zoneconditionalnpc;

    bool ok = true;
    ok      = expect(Classify("Moogle", 1.5f, 0x52) == ConditionalNPC::MogHouseMoogle, "Mog House Moogle identity matches") && ok;
    ok      = expect(Classify("Moogle", 1.5f, 0x51) == ConditionalNPC::None, "other Moogle face is excluded") && ok;
    ok      = expect(Classify("Moogle", 1.4f, 0x52) == ConditionalNPC::None, "other Moogle position is excluded") && ok;
    ok      = expect(Classify("Symphonic_Curator", 0.0f, 0x00) == ConditionalNPC::SymphonicCurator, "curator identity matches") && ok;
    ok      = expect(Classify("Other", 1.5f, 0x52) == ConditionalNPC::None, "unrelated NPC is excluded") && ok;

    ok = expect(ShouldShowMogHouseMoogle(true, false), "Moogle is shown on Mog House first floor") && ok;
    ok = expect(!ShouldShowMogHouseMoogle(false, false), "Moogle is hidden outside Mog House") && ok;
    ok = expect(!ShouldShowMogHouseMoogle(true, true), "Moogle is hidden on second floor") && ok;

    ok = expect(ShouldShowSymphonicCurator(true, true), "curator requires home-nation Mog House orchestrion") && ok;
    ok = expect(!ShouldShowSymphonicCurator(false, true), "curator excludes foreign Mog House") && ok;
    ok = expect(!ShouldShowSymphonicCurator(true, false), "curator requires placed orchestrion") && ok;
    return ok;
}
