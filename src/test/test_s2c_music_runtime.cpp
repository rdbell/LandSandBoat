#include "test_s2c_music_runtime.h"

#include <array>
#include <cstdint>
#include <iostream>

#include "map/packets/s2c/0x05f_music.h"

namespace
{

auto testPlanPreservesSlotAndTrack() -> bool
{
    constexpr auto slots = std::array{ MusicSlot::ZoneDay, MusicSlot::ZoneNight, MusicSlot::CombatSolo, MusicSlot::CombatParty, MusicSlot::Mount, MusicSlot::Dead, MusicSlot::MogHouse, MusicSlot::Fishing };
    bool           ok    = true;
    for (std::size_t index = 0; index < slots.size(); ++index)
    {
        const auto plan = musichelpers::PlanFor({ .slot = slots[index], .track = UINT16_MAX });
        if (plan.Slot != slots[index] || plan.MusicNum != UINT16_MAX)
        {
            std::cerr << "s2c MUSIC runtime self-test failed: slot or track changed at index " << index << '\n';
            ok = false;
        }
    }
    return ok;
}

} // namespace

auto runS2CMusicRuntimeSelfTests() -> bool
{
    return testPlanPreservesSlotAndTrack();
}
