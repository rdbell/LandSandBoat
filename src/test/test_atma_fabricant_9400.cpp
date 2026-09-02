#include "map/atma_fabricant_capacity.h"
#include "omega_self_test_registry.h"

#include <iostream>

auto runAtmaFabricant9400SelfTests() -> bool
{
    using atmafabricant::TriggerAction;

    const auto noVisitant = atmafabricant::PlanTrigger(false, 7416);
    const auto withVisitant = atmafabricant::PlanTrigger(true, 7416);
    const bool ok = noVisitant.action == TriggerAction::NoVisitantMessage &&
                    noVisitant.eventID == 0 && noVisitant.messageID == 7416 &&
                    withVisitant.action == TriggerAction::StartEvent &&
                    withVisitant.eventID == atmafabricant::EventID && withVisitant.messageID == 0;
    if (!ok)
    {
        std::cerr << "Atma Fabricant 9400 self-test failed\n";
    }
    return ok;
}

OMEGA_REGISTER_SELF_TEST("atma-fabricant-9400", runAtmaFabricant9400SelfTests);
