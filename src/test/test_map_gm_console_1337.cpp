#include "test_map_gm_console_1337.h"

#include "map/map_gm_console.h"

#include <iostream>

namespace
{

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "map gm console 1337 self-test failed: " << label << '\n';
    }
    return condition;
}

} // namespace

auto runMapGMConsole1337SelfTests() -> bool
{
    bool ok = true;

    ok = expect(mapapp::GMCommandArgCount == 3, "arg count") && ok;
    ok = expect(mapapp::GMLevelMin == 0 && mapapp::GMLevelMax == 5, "level bounds") && ok;

    ok = expect(mapapp::ClassifyGMCommandArgsValid(3), "valid argc") && ok;
    ok = expect(!mapapp::ClassifyGMCommandArgsValid(2), "too few") && ok;
    ok = expect(!mapapp::ClassifyGMCommandArgsValid(4), "too many") && ok;
    ok = expect(!mapapp::ClassifyGMCommandArgsValid(0), "zero") && ok;

    ok = expect(mapapp::ClampGMLevel(0) == 0, "clamp 0") && ok;
    ok = expect(mapapp::ClampGMLevel(3) == 3, "clamp mid") && ok;
    ok = expect(mapapp::ClampGMLevel(5) == 5, "clamp 5") && ok;
    ok = expect(mapapp::ClampGMLevel(-1) == 0, "clamp low") && ok;
    ok = expect(mapapp::ClampGMLevel(99) == 5, "clamp high") && ok;

    ok = expect(mapapp::FormatGMUsage() == "Usage: gm <char_name> <level>. example: gm Testo 1\n", "usage") && ok;
    ok = expect(mapapp::FormatGMCharNotFound("Testo") == "Couldnt find character: Testo\n", "not found typo") && ok;
    ok = expect(mapapp::FormatGMPromote("Alice", 2) == "> Promoting Alice to GM level 2\n", "promote") && ok;
    ok = expect(mapapp::FormatGMPlayerNotice(4) == "You have been set to GM level 4.", "player notice") && ok;
    ok = expect(mapapp::FormatReloadRecipesNotice() == "> Reloading crafting recipes\n", "reload") && ok;

    return ok;
}
