#include "test_latent_party_job_selection_7534.h"

#include "map/latent_party_job_selection.h"

#include <cstdint>
#include <iostream>

auto runLatentPartyJobSelection7534SelfTests() -> bool
{
    const auto expect = [](const bool value, const char* label) {
        if (!value)
        {
            std::cerr << "latent party-job selection 7534 self-test failed: " << label << '\n';
        }
        return value;
    };

    for (std::uint16_t id = 0; id <= static_cast<std::uint16_t>(xi::Latent::InGarrison); ++id)
    {
        const auto condition = static_cast<xi::Latent>(id);
        const auto expected  = condition == xi::Latent::JobInParty;
        if (!expect(latenthelpers::ShouldProcessPartyJobLatent(condition) == expected, "condition catalog"))
        {
            return false;
        }
    }

    return true;
}
