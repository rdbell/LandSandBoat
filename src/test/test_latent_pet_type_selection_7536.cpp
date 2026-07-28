#include "test_latent_pet_type_selection_7536.h"

#include "map/latent_pet_type_selection.h"

#include <cstdint>
#include <iostream>

auto runLatentPetTypeSelection7536SelfTests() -> bool
{
    const auto expect = [](const bool value, const char* label) {
        if (!value)
        {
            std::cerr << "latent pet-type selection 7536 self-test failed: " << label << '\n';
        }
        return value;
    };

    for (std::uint16_t id = 0; id <= static_cast<std::uint16_t>(xi::Latent::InGarrison); ++id)
    {
        const auto condition = static_cast<xi::Latent>(id);
        const auto expected  = condition == xi::Latent::PetId;
        if (!expect(latenthelpers::ShouldProcessPetTypeLatent(condition) == expected, "condition catalog"))
        {
            return false;
        }
    }

    return true;
}
