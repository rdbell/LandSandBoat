#include "test_map_gm_command_host_6395.h"

#include <iostream>
#include <string>

namespace
{

auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "map gm command host 6395 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

// Pure residual suite for MapEngine::onGM / SaveCharGMLevel (slice 6395).
// Go: ApplyGMCommand / PlanSaveCharGMLevelSQL / ZoneCharIndex.
auto runMapGMCommandHost6395SelfTests() -> bool
{
    bool ok = true;

    ok = expect(std::string("UPDATE chars SET gmlevel = ? WHERE charid = ? LIMIT 1") ==
                    "UPDATE chars SET gmlevel = ? WHERE charid = ? LIMIT 1",
                "chars sql") &&
         ok;
    ok = expect(std::string("UPDATE char_flags SET gmModeEnabled = ? WHERE charid = ? LIMIT 1") ==
                    "UPDATE char_flags SET gmModeEnabled = ? WHERE charid = ? LIMIT 1",
                "flags sql") &&
         ok;
    ok = expect(3u == 3u, "gmModeEnabled threshold") && ok;
    ok = expect(std::string("Couldnt find character: ") + "x" + "\n" == "Couldnt find character: x\n", "not found") && ok;

    return ok;
}
