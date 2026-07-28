#include "test_latent_roll_song_selection_7526.h"

#include "map/latent_roll_song_selection.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>

auto runLatentRollSongSelection7526SelfTests() -> bool
{
    const auto expect = [](const bool value, const char* label) {
        if (!value)
        {
            std::cerr << "latent roll-song selection 7526 self-test failed: " << label << '\n';
        }
        return value;
    };

    constexpr std::array rollSongConditions{
        xi::Latent::SongRollActive,
        xi::Latent::ElevenRollActive,
    };

    for (std::uint16_t id = 0; id <= static_cast<std::uint16_t>(xi::Latent::InGarrison); ++id)
    {
        const auto condition = static_cast<xi::Latent>(id);
        const auto expected  = std::find(rollSongConditions.begin(), rollSongConditions.end(), condition) != rollSongConditions.end();
        if (!expect(latenthelpers::ShouldProcessRollSongLatent(condition) == expected, "condition catalog"))
        {
            return false;
        }
    }

    return true;
}
