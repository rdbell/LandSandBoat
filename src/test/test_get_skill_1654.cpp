#include "test_get_skill_1654.h"

#include "map/get_skill_capacity.h"

#include <iostream>

namespace
{
using namespace getskillhelpers;

auto expect(const bool condition, const char* label) -> bool
{
    if (!condition)
    {
        std::cerr << "get skill 1654 self-test failed: " << label << '\n';
    }
    return condition;
}
} // namespace

auto runGetSkill1654SelfTests() -> bool
{
    bool ok = true;

    // --- pins ---
    ok = expect(MaxSkillType == 64, "MaxSkillType") && ok;
    ok = expect(SkillValueMask == 0x7FFF, "SkillValueMask") && ok;
    ok = expect(SkillCappedBlueFlag == 0x8000, "SkillCappedBlueFlag") && ok;
    ok = expect((SkillValueMask & SkillCappedBlueFlag) == 0, "mask clears blue") && ok;
    ok = expect((SkillValueMask | SkillCappedBlueFlag) == 0xFFFF, "mask|blue full") && ok;

    // --- IsSkillIDInRange ---
    ok = expect(IsSkillIDInRange(0) && IsSkillIDInRange(1) && IsSkillIDInRange(63), "in range") && ok;
    ok = expect(!IsSkillIDInRange(64) && !IsSkillIDInRange(65) && !IsSkillIDInRange(0xFFFF), "oob") && ok;

    // --- MaskSkillValue ---
    ok = expect(MaskSkillValue(0) == 0, "mask zero") && ok;
    ok = expect(MaskSkillValue(200) == 200, "mask plain") && ok;
    ok = expect(MaskSkillValue(0x8000) == 0, "mask blue only") && ok;
    ok = expect(MaskSkillValue(static_cast<std::uint16_t>(0x8000 | 250)) == 250, "mask blue|250") && ok;
    ok = expect(MaskSkillValue(0xFFFF) == 0x7FFF, "mask full") && ok;
    ok = expect(MaskSkillValue(0x8123) == 0x0123, "mask 0x8123") && ok;

    // --- GetSkill in range ---
    ok = expect(GetSkill(1, 200) == 200, "h2h plain") && ok;
    ok = expect(GetSkill(3, static_cast<std::uint16_t>(0x8000 | 300)) == 300, "sword blue capped") && ok;
    ok = expect(GetSkill(0, 42) == 42, "id 0") && ok;
    ok = expect(GetSkill(63, 7) == 7, "id 63") && ok;
    ok = expect(GetSkill(25, 0) == 0, "zero entry") && ok;
    ok = expect(GetSkill(12, 0xFFFF) == 0x7FFF, "full word") && ok;

    // Craft packed encoding is returned as-is (GetSkill does not unpack).
    constexpr std::uint16_t craftPacked = static_cast<std::uint16_t>((50 * 0x20) + 3);
    ok = expect(GetSkill(48, craftPacked) == craftPacked, "fishing craft packed") && ok;

    // --- GetSkill out of range ignores entry ---
    ok = expect(GetSkill(64, 999) == 0, "oob 64") && ok;
    ok = expect(GetSkill(65, static_cast<std::uint16_t>(0x8000 | 500)) == 0, "oob 65") && ok;
    ok = expect(GetSkill(0xFFFF, 1) == 0, "oob max") && ok;

    // --- blue only / high value ---
    ok = expect(GetSkill(10, 0x8000) == 0, "blue only") && ok;
    ok = expect(GetSkill(33, 0x7FFF) == 0x7FFF, "high no flag") && ok;
    ok = expect(GetSkill(22, 100) == 100, "automaton melee") && ok;
    ok = expect(GetSkill(29, 412) == 412, "evasion") && ok;

    return ok;
}
