#include "test_latent_ws_plan_7528.h"

#include "map/latent_ws_plan.h"

#include <cstdint>
#include <iostream>

auto runLatentWsPlan7528SelfTests() -> bool
{
    const auto expect = [](const bool value, const char* label) {
        if (!value)
        {
            std::cerr << "latent during-WS plan 7528 self-test failed: " << label << '\n';
        }
        return value;
    };

    const auto expected = [](const xi::Latent condition, const bool isDuringWs) {
        if (condition != xi::Latent::DuringWs)
        {
            return latenthelpers::DuringWsLatentAction::Ignore;
        }
        return isDuringWs ? latenthelpers::DuringWsLatentAction::Activate : latenthelpers::DuringWsLatentAction::Deactivate;
    };

    for (std::uint16_t id = 0; id <= static_cast<std::uint16_t>(xi::Latent::InGarrison); ++id)
    {
        const auto condition = static_cast<xi::Latent>(id);
        if (!expect(latenthelpers::DetermineDuringWsLatentAction(condition, true) == expected(condition, true), "enter condition catalog") ||
            !expect(latenthelpers::DetermineDuringWsLatentAction(condition, false) == expected(condition, false), "leave condition catalog"))
        {
            return false;
        }
    }

    return expect(latenthelpers::DetermineDuringWsLatentAction(xi::Latent::DuringWs, true) == latenthelpers::DuringWsLatentAction::Activate, "enter activates") &&
           expect(latenthelpers::DetermineDuringWsLatentAction(xi::Latent::DuringWs, false) == latenthelpers::DuringWsLatentAction::Deactivate, "leave deactivates");
}
