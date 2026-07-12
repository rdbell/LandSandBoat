#pragma once

#include <cstdint>

// Pure isValidSelfTargetWeaponskill / CanUseWeaponskill halves.
// Parity: internal/weaponskill IsValidSelfTarget / CanUseWeaponskill.

namespace weaponskillusehelpers
{

constexpr int WSIDStarlight = 163;
constexpr int WSIDMoonlight = 164;
constexpr int WSIDDagan     = 173;
constexpr int WSIDMyrkr     = 190;

constexpr auto IsValidSelfTargetWeaponskill(const int wsid) -> bool
{
    switch (wsid)
    {
        case WSIDStarlight:
        case WSIDMoonlight:
        case WSIDDagan:
        case WSIDMyrkr:
            return true;
        default:
            return false;
    }
}

struct CanUseWeaponskillInput
{
    std::uint16_t skillLevel{};
    std::uint8_t  unlockID{};
    bool          mainOnly{};
    std::uint8_t  mainJobReq{};
    std::uint8_t  subJobReq{};
    std::uint16_t charSkill{};
    bool          hasLearned{};
    std::uint8_t  mLevel{};
};

constexpr auto CanUseWeaponskill(const CanUseWeaponskillInput in) -> bool
{
    const bool skillOK = (in.skillLevel > 0 && in.charSkill >= in.skillLevel &&
                          (in.unlockID == 0 || in.hasLearned)) ||
                         (in.skillLevel == 0 &&
                          (in.unlockID == 0 || (in.hasLearned && in.mLevel >= 75)));
    const bool jobOK = in.mainJobReq > 0 || (in.subJobReq > 0 && !in.mainOnly);
    return skillOK && jobOK;
}

} // namespace weaponskillusehelpers
