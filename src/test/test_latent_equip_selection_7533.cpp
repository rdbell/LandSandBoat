#include "test_latent_equip_selection_7533.h"

#include "map/latent_equip_selection.h"

#include <cstdint>
#include <iostream>

auto runLatentEquipSelection7533SelfTests() -> bool
{
    const auto expect = [](const bool value, const char* label) {
        if (!value)
        {
            std::cerr << "latent equip selection 7533 self-test failed: " << label << '\n';
        }
        return value;
    };

    return expect(latenthelpers::ShouldProcessEquipLatent(0, 0), "first slot") &&
           expect(latenthelpers::ShouldProcessEquipLatent(17, 17), "last equipment slot") &&
           expect(latenthelpers::ShouldProcessEquipLatent(18, 18), "max slot sentinel") &&
           expect(latenthelpers::ShouldProcessEquipLatent(255, 255), "uint8 maximum") &&
           expect(!latenthelpers::ShouldProcessEquipLatent(4, 5), "adjacent mismatch") &&
           expect(!latenthelpers::ShouldProcessEquipLatent(255, 0), "high mismatch");
}
