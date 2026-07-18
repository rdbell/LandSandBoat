#pragma once

#include <cstdint>
namespace targetfindvalidtargethelpers
{
enum class Result : std::uint8_t { None, Candidate, Pet };
constexpr auto Resolve(const bool exists, const bool requestPet, const bool hasPet, const bool ignoreBattleID, const bool sameBattleID, const bool valid) -> Result
{
    if (!exists) return Result::None;
    if (requestPet && hasPet) return Result::Pet;
    return (ignoreBattleID || sameBattleID) && valid ? Result::Candidate : Result::None;
}
} // namespace targetfindvalidtargethelpers
