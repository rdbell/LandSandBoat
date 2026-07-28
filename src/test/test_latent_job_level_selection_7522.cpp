#include "test_latent_job_level_selection_7522.h"

#include "map/latent_job_level_selection.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>

auto runLatentJobLevelSelection7522SelfTests() -> bool
{
    const auto expect = [](const bool value, const char* label) {
        if (!value)
        {
            std::cerr << "latent job-level selection 7522 self-test failed: " << label << '\n';
        }
        return value;
    };

    constexpr std::array jobLevelConditions{
        xi::Latent::JobMultiple,
        xi::Latent::JobMultipleAtNight,
        xi::Latent::JobLevelBelow,
        xi::Latent::JobLevelAbove,
        xi::Latent::InGarrison,
    };

    for (std::uint16_t id = 0; id <= static_cast<std::uint16_t>(xi::Latent::InGarrison); ++id)
    {
        const auto condition = static_cast<xi::Latent>(id);
        const auto expected  = std::find(jobLevelConditions.begin(), jobLevelConditions.end(), condition) != jobLevelConditions.end();
        if (!expect(latenthelpers::ShouldProcessJobLevelLatent(condition) == expected, "condition catalog"))
        {
            return false;
        }
    }

    return true;
}
