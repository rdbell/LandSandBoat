#include "map/instance_loader.h"

#include <iostream>

auto runInstanceLoader2639SelfTests() -> bool
{
    const auto noOverlay = instanceloader::SpawnQueryPlanFor(50, 0);
    const auto overlay   = instanceloader::SpawnQueryPlanFor(50, 99);
    const auto mob       = instanceloader::MobLoadQueryPlanFor(overlay, 0x1234);
    const auto npc       = instanceloader::NpcLoadQueryPlanFor(overlay, 0x1234);

    const auto noOverlayValid = noOverlay.realZoneId == 50 &&
                                noOverlay.effectiveZoneId == 50 &&
                                noOverlay.npcMin == (50U << 12) + 0x1000000 &&
                                noOverlay.npcMax == noOverlay.npcMin + 1024;
    const auto overlayValid   = overlay.realZoneId == 50 &&
                                overlay.effectiveZoneId == 99 &&
                                overlay.npcMin == (99U << 12) + 0x1000000 &&
                                overlay.npcMax == overlay.npcMin + 1024;
    const auto bindingsValid  = mob.templateZoneId == 50 && mob.instanceId == 0x1234 && mob.effectiveZoneId == 99 && npc.instanceId == 0x1234 && npc.npcMin == overlay.npcMin && npc.npcMax == overlay.npcMax;
    if (!noOverlayValid || !overlayValid || !bindingsValid)
    {
        std::cerr << "instance loader 2639 self-test failed\n";
    }
    return noOverlayValid && overlayValid && bindingsValid;
}
