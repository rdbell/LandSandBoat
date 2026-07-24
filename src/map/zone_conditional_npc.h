#pragma once

#include <cstdint>
#include <string_view>

namespace zoneconditionalnpc
{

enum class ConditionalNPC : std::uint8_t
{
    None,
    MogHouseMoogle,
    SymphonicCurator,
};

// Classify mirrors SpawnConditionalNPCs' intentionally specific NPC identity
// checks. The Moogle's position and face distinguish it from unrelated Moogles.
constexpr auto Classify(const std::string_view name, const float positionZ, const std::uint8_t face) -> ConditionalNPC
{
    if (name == "Moogle" && positionZ == 1.5f && face == 0x52)
    {
        return ConditionalNPC::MogHouseMoogle;
    }

    if (name == "Symphonic_Curator")
    {
        return ConditionalNPC::SymphonicCurator;
    }

    return ConditionalNPC::None;
}

// ShouldShowMogHouseMoogle mirrors the Mog House floor gate.
constexpr auto ShouldShowMogHouseMoogle(const bool inMogHouse, const bool onSecondFloor) -> bool
{
    return inMogHouse && !onSecondFloor;
}

// ShouldShowSymphonicCurator mirrors the home-nation Mog House and
// orchestrion-placement gate.
constexpr auto ShouldShowSymphonicCurator(const bool inHomeNationMogHouse, const bool orchestrionPlaced) -> bool
{
    return inHomeNationMogHouse && orchestrionPlaced;
}

} // namespace zoneconditionalnpc
