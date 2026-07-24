#include "test_spawn_weather_despawn_6267.h"

#include "map/spawn_weather_despawn.h"

#include <iostream>

namespace
{
auto expect(const bool condition, const char* const label) -> bool
{
    if (!condition)
    {
        std::cerr << "spawn weather despawn 6267 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

// Pins SpawnHandler::onWeatherChange's production-wired elemental and fog
// despawn gates plus its one-second scheduling delay.
auto runSpawnWeatherDespawn6267SelfTests() -> bool
{
    bool ok = true;
    ok = expect(spawnweatherdespawn::shouldDespawn(true, false, 0x04, false, false),
                "unmastered elemental with mismatched weather despawns") && ok;
    ok = expect(!spawnweatherdespawn::shouldDespawn(true, false, 0x04, true, false),
                "elemental matching weather remains") && ok;
    ok = expect(!spawnweatherdespawn::shouldDespawn(true, true, 0x04, false, false),
                "mastered elemental remains") && ok;
    ok = expect(!spawnweatherdespawn::shouldDespawn(false, false, 0x04, false, false),
                "non-elemental weather flag alone remains") && ok;
    ok = expect(spawnweatherdespawn::shouldDespawn(false, false, 0x08, true, false),
                "fog mob outside fog despawns") && ok;
    ok = expect(!spawnweatherdespawn::shouldDespawn(false, false, 0x08, false, true),
                "fog mob in fog remains") && ok;
    ok = expect(spawnweatherdespawn::shouldDespawn(true, false, 0x0C, false, false),
                "elemental mismatch retains despawn precedence with fog flag") && ok;
    ok = expect(spawnweatherdespawn::delay() == std::chrono::seconds{ 1 },
                "matching mobs schedule despawn after one second") && ok;
    return ok;
}
