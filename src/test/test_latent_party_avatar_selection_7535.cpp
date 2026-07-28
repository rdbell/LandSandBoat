#include "test_latent_party_avatar_selection_7535.h"

#include "map/latent_party_avatar_selection.h"

#include <cstdint>
#include <iostream>

auto runLatentPartyAvatarSelection7535SelfTests() -> bool
{
    const auto expect = [](const bool value, const char* label) {
        if (!value)
        {
            std::cerr << "latent party-avatar selection 7535 self-test failed: " << label << '\n';
        }
        return value;
    };

    for (std::uint16_t id = 0; id <= static_cast<std::uint16_t>(xi::Latent::InGarrison); ++id)
    {
        const auto condition = static_cast<xi::Latent>(id);
        const auto expected  = condition == xi::Latent::AvatarInParty;
        if (!expect(latenthelpers::ShouldProcessPartyAvatarLatent(condition) == expected, "condition catalog"))
        {
            return false;
        }
    }

    return true;
}
