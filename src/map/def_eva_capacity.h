#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>

// Pure CBattleEntity::DEF / EVA math with injectable inputs (no entity pointers).
// Parity: internal/defeva (slice 1638).

namespace defevahelpers
{

constexpr float DefaultPlayerAlliesVitDefMultiplier = 1.5f;
constexpr float MobVitDefMultiplier                 = 0.5f;
constexpr std::int32_t BaseDEF                      = 8;

// PC-only level DEF factor from CBattleEntity::DEF.
inline auto PCLevelDEFBonus(const std::uint8_t level) -> std::int32_t
{
    const auto lv = static_cast<std::int32_t>(level);
    if (level < 51)
    {
        return lv;
    }
    if (level < 61)
    {
        return 2 * lv - 42;
    }
    if (level < 91)
    {
        return lv + 18;
    }
    // (level-89)/2 is integer division before floor in LSB; floor is a no-op.
    return lv + 18 + (lv - 89) / 2;
}

// Pure DEF once vit/factor/level/mods/counterstance are injected.
// Counterstance path returns DEF/2 without max(1, …).
// Else: max(1, DEF + DEF*DEFP/100 + min<int16>(DEF*FOOD_DEFP/100, FOOD_DEF_CAP)).
inline auto ResolveDEF(
    const std::int16_t vit,
    const float        vitFactor,
    const bool         isPC,
    const std::uint8_t level,
    const std::int16_t defMod,
    const std::int16_t defP,
    const std::int16_t foodDefP,
    const std::int16_t foodDefCap,
    const bool         counterstance) -> std::int32_t
{
    auto def = BaseDEF + static_cast<std::int32_t>(std::floor(static_cast<float>(vit) * vitFactor));
    if (isPC)
    {
        def += PCLevelDEFBonus(level);
    }
    def += defMod;

    if (counterstance)
    {
        return def / 2;
    }

    const auto foodBonus = std::min<std::int16_t>(
        static_cast<std::int16_t>(def * foodDefP / 100),
        foodDefCap);
    const auto total = def + (def * defP / 100) + foodBonus;
    return std::max<std::int32_t>(1, total);
}

// PC/automaton SKILL_EVASION soft curve: above 200, 200 + (skill-200)*0.9.
inline auto SkillEvasionFromSkill(const std::int16_t skill) -> std::int16_t
{
    if (skill > 200)
    {
        return static_cast<std::int16_t>(200 + (skill - 200) * 0.9);
    }
    return skill;
}

// True for TYPE_MOB or (TYPE_PET && !automaton).
inline auto UseModAsEVABase(const bool isMob, const bool isPet, const bool isAutomaton) -> bool
{
    return isMob || (isPet && !isAutomaton);
}

// Pure EVA: mod-as-base (mob/non-automaton pet) vs skill soft-curve (PC/automaton),
// plus AGI/2, with terminal Mod::EVA only on the skill path.
inline auto ResolveEVA(
    const std::int16_t agi,
    const std::int16_t skill,
    const std::int16_t evaMod,
    const bool         useModAsBase) -> std::int16_t
{
    std::int16_t evasion = 0;
    if (useModAsBase)
    {
        evasion = evaMod;
    }
    else
    {
        evasion = SkillEvasionFromSkill(skill);
    }
    evasion = static_cast<std::int16_t>(evasion + agi / 2);

    const std::int16_t add = useModAsBase ? static_cast<std::int16_t>(0) : evaMod;
    const auto         total = static_cast<std::int32_t>(evasion) + add;
    return static_cast<std::int16_t>(std::max<std::int32_t>(1, total));
}

} // namespace defevahelpers
