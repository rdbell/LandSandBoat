#include "test_latent_time_selection_7520.h"

#include "map/latent_time_selection.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>

auto runLatentTimeSelection7520SelfTests() -> bool
{
    const auto expect = [](const bool value, const char* label) {
        if (!value)
        {
            std::cerr << "latent time selection 7520 self-test failed: " << label << '\n';
        }
        return value;
    };

    constexpr std::array weekDayConditions{
        xi::Latent::Firesday,
        xi::Latent::Earthsday,
        xi::Latent::Watersday,
        xi::Latent::Windsday,
        xi::Latent::Darksday,
        xi::Latent::Iceday,
        xi::Latent::Lightningsday,
        xi::Latent::Lightsday,
    };
    constexpr std::array refreshes{
        latenthelpers::LatentTimeRefresh::Day,
        latenthelpers::LatentTimeRefresh::MoonPhase,
        latenthelpers::LatentTimeRefresh::WeekDay,
        latenthelpers::LatentTimeRefresh::Hour,
    };

    for (const auto refresh : refreshes)
    {
        for (std::uint16_t id = 0; id <= static_cast<std::uint16_t>(xi::Latent::InGarrison); ++id)
        {
            const auto condition = static_cast<xi::Latent>(id);
            auto expected        = false;
            switch (refresh)
            {
                case latenthelpers::LatentTimeRefresh::Day:
                    expected = condition == xi::Latent::TimeOfDay;
                    break;
                case latenthelpers::LatentTimeRefresh::MoonPhase:
                    expected = condition == xi::Latent::MoonPhase;
                    break;
                case latenthelpers::LatentTimeRefresh::WeekDay:
                    expected = std::find(weekDayConditions.begin(), weekDayConditions.end(), condition) != weekDayConditions.end();
                    break;
                case latenthelpers::LatentTimeRefresh::Hour:
                    expected = condition == xi::Latent::HourOfDay;
                    break;
            }

            if (!expect(latenthelpers::ShouldProcessTimeLatent(refresh, condition) == expected, "condition catalog"))
            {
                return false;
            }
        }
    }

    return expect(!latenthelpers::ShouldProcessTimeLatent(static_cast<latenthelpers::LatentTimeRefresh>(99), xi::Latent::TimeOfDay), "unknown refresh");
}
