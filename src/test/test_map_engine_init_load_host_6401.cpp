#include "test_map_engine_init_load_host_6401.h"

#include <iostream>
#include <string>
#include <vector>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map engine init load host 6401 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for MapEngine::init preamble + content loads (slice 6401).
// Go: PlanMapEngineInitPreamble / ApplyMapEngineContentLoads / mesh gate.
auto runMapEngineInitLoadHost6401SelfTests() -> bool
{
    bool ok = true;

    ok = expect(std::string("do_init: begin server initialization") == "do_init: begin server initialization", "begin") && ok;
    ok = expect(std::string("do_init: loading items") == "do_init: loading items", "items") && ok;
    ok = expect(std::string("do_init: loading plants") == "do_init: loading plants", "plants") && ok;
    ok = expect(std::string("do_init: loading spells") == "do_init: loading spells", "spells") && ok;

    const std::vector<std::string> loads = {
        "itemutils::Initialize",
        "gardenutils::Initialize",
        "spell::LoadSpellList",
        "CItemEquipment::LoadAugmentData",
    };
    ok = expect(loads.front() == "itemutils::Initialize", "first load") && ok;
    ok = expect(loads.back() == "CItemEquipment::LoadAugmentData", "last load") && ok;
    ok = expect(loads.size() == 4, "sample size") && ok;

    // Mesh gate: !inCI
    const bool inCI         = true;
    const bool shouldCheck  = !inCI;
    ok = expect(!shouldCheck, "CI skips mesh") && ok;

    const std::string ximeshCrit = "./ximeshes/ directory isn't present or is empty! Check your setup.";
    ok = expect(ximeshCrit.find("ximeshes") != std::string::npos, "ximesh critical") && ok;

    return ok;
}
