#include "map/instance_loader.h"

#include <iostream>

auto runInstanceLoader2639SelfTests() -> bool
{
    const auto noOverlay = instanceloader::SpawnQueryPlanFor(50, 0);
    const auto overlay   = instanceloader::SpawnQueryPlanFor(50, 99);

    const auto noOverlayValid = noOverlay.realZoneId == 50 &&
                                noOverlay.effectiveZoneId == 50 &&
                                noOverlay.npcMin == (50U << 12) + 0x1000000 &&
                                noOverlay.npcMax == noOverlay.npcMin + 1024;
    const auto overlayValid = overlay.realZoneId == 50 &&
                              overlay.effectiveZoneId == 99 &&
                              overlay.npcMin == (99U << 12) + 0x1000000 &&
                              overlay.npcMax == overlay.npcMin + 1024;
    if (!noOverlayValid || !overlayValid)
    {
        std::cerr << "instance loader 2639 self-test failed\n";
    }
    return noOverlayValid && overlayValid;
}
